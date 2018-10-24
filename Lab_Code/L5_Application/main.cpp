#include "tasks.hpp"
#include "utilities.h"
#include "io.hpp"
#include "gpio.hpp"
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
void write_to_sram(string address, string data);
void read_from_sram(string address);
void pin_setter(string data);
int bit_checker(string bits);
void disable373s();
void setGPIOs();
void setAsOutput();
void setAsInput();
void smReset();
string rGPIO();
void tick();
string read_keypad();
void initialize_LCD ();
void write_to_LCD (string data, char rs);
void read_from_LCD (string data, char rs);


GPIO  a0(P1_29);
GPIO  a1(P1_28);
GPIO  a2(P1_23);
GPIO  a3(P1_22);
GPIO  a4(P1_20);
GPIO  a5(P1_19);
GPIO  a6(P0_30);
GPIO  a7(P0_29);

GPIO  dir_w(P0_0);
GPIO  bus_eL(P0_1);
GPIO  addr_w(P2_1);
GPIO  dataOut_w(P2_2);
GPIO  dataIn_eL(P2_3);
GPIO  cmd_w(P2_4);
GPIO clk(P2_6);


//CMD Reg Pins (0-7): sel_sram, sel_lcd, sel_kp, r, w, lcd_rs, sel_spi_temp_sens_oe, NULL;

int main(void) {

	//Initialize SJOne Board
	dir_w.setAsOutput();
	bus_eL.setAsOutput();
	addr_w.setAsOutput();
	dataOut_w.setAsOutput();
	dataIn_eL.setAsOutput();
	cmd_w.setAsOutput();
	clk.setAsOutput();
	disable373s();

	char selector='0';
	string address="";
	string data="";

	//7 Seg Greeting
	LD.setRightDigit('I');
	LD.setLeftDigit('H');
	
	initialize_LCD();
	cout<<"Welcome to the SJOne Board Interface."<<endl;

	while(selector!='e')
	{
		int kpend=0;
		string keys_pressed="";
		cout<<"\nSelect an option:"<<endl;
		cout<<"1) Write to SRAM."<<endl;
		cout<<"2) Read from SRAM."<<endl;
		cout<<"3) Read from Keypad."<<endl;
		cout<<"4)LCD Test"<<endl;
		cout<<"Enter \"e\" to quit."<<endl;
		cin>>selector;

		if(selector=='1')
		{
			cout<<"Enter 8-bit Address to write to: ";
			cin>>address;
			cout<<endl;
			if(bit_checker(address)) 
			{

				cout<<"Enter 8-bit data to write: ";
				cin>>data;
				cout<<endl;
				if(bit_checker(data))
				{
					cout<<"Writing data <"<<data<<"> to address <"<<address<<">."<<endl;
					write_to_sram(address, data);
				}
				else cout<<"Data length not 8 bits."<<endl;
			}
			else cout<<"Address length not 8 bits."<<endl;
		}
		if (selector=='2')
		{
			cout<<"Enter 8-bit Address to read from: ";
			cin>>address;
			cout<<endl;
			if(bit_checker(address)) 
			{
				read_from_sram(address);
			}
			else cout<<"Address length not 8 bits."<<endl;
		}
		if (selector =='3')
		{
			cout<<"Hold the keys you want read on the keypad."<<endl;
			cout<<"Hit the read key on the SJOne Board"<<endl;
			while(!kpend)
			{
				if (SW.getSwitch(1))
				{
					cout<<"State machine started."<<endl;
					keys_pressed=read_keypad();
					cout<<"The following keys were pressed:"<<keys_pressed<<endl;
					kpend=1;
				}	
			}
		}
		if (selector =='4')
		{
			write_to_LCD("01010111",'1');
			//read_from_LCD("00000000",'1');
			cout<<"Test complete."<<endl;
		}
		else cout<<"\n>>Choose the right selector"<<endl<<endl;
	}

	cout<<"SJOne Board Interface Exited."<<endl;
	cout<<"Interface brought to you from countless 4AM SCE wiring wrapping sessions."<<endl;
	cout<<"Hit reset button to restart program."<<endl;
	while(1)
	{
		delay_ms(1000); //Just Loops at the end.
	}
}

void write_to_sram(string address, string data)
{
	string wOp="00010001"; //Must reflect pins on new schematic
	disable373s();
	setAsOutput();
	dir_w.setHigh(); //SJOne->SRAM

	cout<<"Setting Address"<<endl;
	pin_setter(address); //Set Address on SJOne

	//Pass Address to SRAM
	bus_eL.setLow();
	addr_w.setHigh();
	delay_ms(10); //Allow address to get to SRAM
	addr_w.setLow();
	cout<<"Address Latched."<<endl;

	smReset(); //Set sel_sram to 0 to clear 164
	cout<<"Setting Data."<<endl;
	pin_setter(data); //Set Data Bits on SJOne
	dataOut_w.setHigh();
	delay_ms(100);
	dataOut_w.setLow();
	cout<<"Sending State Machine Command."<<endl;
	pin_setter(wOp); //Latches cmd register
	cmd_w.setHigh(); //Starts the state machine

	for(int i=0; i<6; i++) {tick();} //Toggle clock 6 times

	cout<<"Write Operation Complete."<<endl;
}
void read_from_sram(string address)
{
	string rOp="00001001";
	string data="";
	disable373s();
	dir_w.setHigh(); //Set as output for commands.
	setAsOutput();
	cout<<"Reading from address: "<<address<<""<<endl;
	pin_setter(address);

	//Pass Address to SRAM
	bus_eL.setLow();
	addr_w.setHigh();
	delay_ms(1); //Allow address to get to SRAM
	addr_w.setLow(); //Address latched.

	smReset(); //Resets 164

	pin_setter(rOp); 
	cmd_w.setHigh();
	delay_ms(1);
	cmd_w.setLow();//Latches cmd register. 
	//State machine doesn't start until clock toggle.

	bus_eL.setHigh();

	dir_w.setLow(); //SJOne<-SRAM
	dataIn_eL.setLow();
	setAsInput();

	bus_eL.setLow();

	for(int i=0; i<6; i++) {tick();} //Toggle clock 6 times

	data=rGPIO();
	cout<<"Data: "<<data<<endl;
	cout<<"Read Operation Complete."<<endl;
}

string read_keypad(){
	string row="";
	string kp_Op="00000100";
	string output="";
	string buttons="";

	LE.setAll(0);
	LD.clear();
	for (int i = 0; i < 4; ++i)
	{
		if 		(i == 0) row = "00000001";
		else if (i == 1) row = "00000010";
		else if (i == 2) row = "00000100";
		else if (i == 3) row = "00001000";

		disable373s();
		bus_eL.setHigh();
		dir_w.setHigh(); // A-> B
		setAsOutput();
		bus_eL.setLow();
		pin_setter(row); //Selects row 1.
		dataOut_w.setHigh();
		delay_ms(10);
		dataOut_w.setLow();
		cout<<"Row "<<i+1<<" data out latched."<<endl;

		smReset();
		delay_ms(10);
		pin_setter("00000100"); //Select kp
		cmd_w.setHigh();
		delay_ms(10);
		cmd_w.setLow();
		cout<<"Command Register latched."<<endl;

		bus_eL.setHigh();
		setAsInput();
		dir_w.setLow(); // A <- B
		dataIn_eL.setLow(); 
		bus_eL.setLow(); //Ready for data from SM

		for(int j=0; j<10; j++) {tick(); }//Toggle clock for 8 clock periods.
		cout<<"State machine run #"<<i+1<<"."<<endl;

		dataIn_eL.setLow(); //Enable Data in register
		output=rGPIO();
		delay_ms(10); //Safety

		if (output[0]=='1'&&output[7]=='1'){
			LD.setRightDigit('A');
			buttons+=" A";
		} 
		if (output[0]=='1'&&output[6]=='1'){
			LD.setRightDigit('B');
			buttons+=" B";
		} 
		if (output[0]=='1'&&output[5]=='1'){
			LD.setRightDigit('C');
			buttons+=" C";
		} 
		if (output[0]=='1'&&output[4]=='1'){
			LD.setRightDigit('D');
			buttons+=" D";
		} 

		if (output[1]=='1'&&output[7]=='1'){
			LD.setNumber(3);
			buttons+=" 3";
		}
		if (output[1]=='1'&&output[6]=='1'){
			LD.setNumber(6);
			buttons+=" 6";
		}
		if (output[1]=='1'&&output[5]=='1'){
			LD.setNumber(9);
			buttons+=" 9";
		}
		if (output[1]=='1'&&output[4]=='1'){
			cout<<"# Pressed."<<endl;
			buttons+=" #";
		}

		if (output[2]=='1'&&output[7]=='1'){
			LD.setNumber(2);
			buttons+=" 2";
		}
		if (output[2]=='1'&&output[6]=='1'){
			LD.setNumber(5);
			buttons+=" 5";
		}
		if (output[2]=='1'&&output[5]=='1'){
			LD.setNumber(8);
			buttons+=" 8";
		}
		if (output[2]=='1'&&output[4]=='1'){
			LD.setNumber(0);
			buttons+=" 0";
		}

		if (output[3]=='1'&&output[7]=='1'){
			LD.setNumber(1);
			buttons+=" 1";
		}
		if (output[3]=='1'&&output[6]=='1'){
			LD.setNumber(4);
			buttons+=" 4";
		}
		if (output[3]=='1'&&output[5]=='1'){
			LD.setNumber(7);
			buttons+=" 7";
		}
		if (output[3]=='1'&&output[4]=='1'){
			cout<<"* Pressed."<<endl;
			buttons+=" *";
		} 
		//cout<<"Output bit " <<i<<": "<<output[i]<<" ";
		cout<<"Output "<<i<<": "<<output<<endl;
	}
	return buttons;
}

//CMD Reg Pins (0-7): sel_sram, sel_lcd, sel_kp, r, w, lcd_rs, sel_spi_temp_sens_oe, NULL;
void initialize_LCD () 
{
	delay_ms(200); //Need time for LCD inter reset state machine
	write_to_LCD("00111000",'0'); //Function Set 
	write_to_LCD("00000001",'0'); //Clear Display
	write_to_LCD("00000010",'0'); //Return Home
	write_to_LCD("00001111",'0'); //Display ON + Blinking Cursor
	cout<<"LCD Initialized"<<endl;
}

void write_to_LCD (string data, char rs)
{
	string wOp="00010010"; //Must reflect pins on new schematic
	wOp[5]=rs; //Sets lcd_rs to be send to cmd reg
	cout<<"CMD Input: "<<wOp<<endl;
	disable373s();
	setAsOutput();
	dir_w.setHigh(); //SJOne->SRAM
	bus_eL.setLow();

	smReset();
	pin_setter(data);
	dataOut_w.setHigh();
	delay_ms(1);
	dataOut_w.setLow();
	pin_setter(wOp); //Sets cmd pins for lcd write op.

	cmd_w.setHigh(); 
	delay_ms(1);
	cmd_w.setLow(); //cmd values latched

	for(int i=0; i<5; i++) {tick();} //Toggle clock 4 times
}

//CMD Reg Pins (0-7): sel_sram, sel_lcd, sel_kp, r, w, lcd_rs, sel_spi_temp_sens_oe, NULL;
void read_from_LCD (string data, char rs)
{
	string rOp="00001010"; //Must reflect pins on new schematic
	rOp[5]=rs; //Sets lcd_rs to be send to cmd reg
	cout<<"RS: "<<rs<<endl;
	disable373s();
	setAsOutput();
	dir_w.setHigh(); //SJOne->SRAM
	bus_eL.setLow();

	smReset();
	pin_setter(data);
	dataOut_w.setHigh();
	delay_ms(10);
	dataOut_w.setLow();
	pin_setter(rOp); //Sets cmd pins for lcd write op.

	cmd_w.setHigh(); 
	delay_ms(10);
	cmd_w.setLow(); //cmd values latched
	//state machine doesnt start until clock toggle

	bus_eL.setHigh();
	dir_w.setLow(); //SJOne<-LCD
	dataIn_eL.setLow();
	setAsInput();

	bus_eL.setLow();

	for(int i=0; i<5; i++) {tick();} //Toggle clock 4 times

	data=rGPIO();
	cout<<"Data: "<<data<<endl;
	cout<<"Read Operation Complete."<<endl;
}
void setAsOutput()
{
	a0.setAsOutput();
	a1.setAsOutput();
	a2.setAsOutput();
	a3.setAsOutput();
	a4.setAsOutput();
	a5.setAsOutput();
	a6.setAsOutput();
	a7.setAsOutput();
}

void setAsInput()
{
	a0.setAsInput();
	a1.setAsInput();
	a2.setAsInput();
	a3.setAsInput();
	a4.setAsInput();
	a5.setAsInput();
	a6.setAsInput();
	a7.setAsInput();
}

void pin_setter(string data)
{
	int index=7;
	int bits[8]={}; //Sets to 0s
	for(int i=0; i<8; i++)
	{
		bits[i]=(data[index]-'0');
		index--;
	}
	if(bits[0])
	{
		a0.setHigh();
		//cout<<"A0: 1"<<endl;
	} 
	else
	{
		a0.setLow();
		//cout<<"A0: 0"<<endl;
	} 
	if(bits[1])
	{
		//cout<<"A1: 1"<<endl;
		a1.setHigh();
	} 
	else
	{
		a1.setLow();
		//cout<<"A1: 0"<<endl;
	}
	if(bits[2])
	{
		//cout<<"A2: 1"<<endl;
		a2.setHigh();
	} 
	else
	{
		a2.setLow();
		//cout<<"A2: 0"<<endl;
	}
	if(bits[3])
	{
		//cout<<"A3: 1"<<endl;
		a3.setHigh();
	} 
	else
	{
		a3.setLow();
		//cout<<"A3: 0"<<endl;
	}
	if(bits[4])
	{
		//cout<<"A4: 1"<<endl;
		a4.setHigh();
	} 
	else
	{
		a4.setLow();
		//cout<<"A4: 0"<<endl;
	}
	if(bits[5])
	{
		//cout<<"A5: 1"<<endl;
		a5.setHigh();
	} 
	else
	{
		a5.setLow();
		//cout<<"A5: 0"<<endl;
	}
	if(bits[6])
	{
		//cout<<"A6: 1"<<endl;
		a6.setHigh();
	} 
	else
	{
		a6.setLow();
		//cout<<"A6: 0"<<endl;
	}
	if(bits[7])
	{
		//cout<<"A7: 1"<<endl;
		a7.setHigh();
	} 
	else
	{
		a7.setLow();
		//cout<<"A7: 0"<<endl;
	}
}

int bit_checker(string bits)
{
	int counter=0;
	for(unsigned int i=0; i<bits.length(); i++) counter++;
	if (counter==8) return 1;
	else return 0;
}

void disable373s()
{
	bus_eL.setHigh();
	addr_w.setLow();
	dataOut_w.setLow();
	dataIn_eL.setHigh();
	cmd_w.setLow();
}

void smReset() //Clears all 164's so they are ready to count
{
	string pins="00000000";
	pin_setter(pins);
	cmd_w.setHigh();
	delay_ms(10);
	cmd_w.setLow();
}
string rGPIO()
{
	stringstream converter;
	string data="";
	converter << noboolalpha << a7.read() << noboolalpha << a6.read() << noboolalpha << a5.read() << noboolalpha << a4.read() << noboolalpha << a3.read() << noboolalpha << a2.read() << noboolalpha << a1.read()<< noboolalpha << a0.read();
	data = converter.str();
	return data;
}

void tick() //changed clock to go high
{
	clk.setHigh();
	delay_ms(10);
	clk.setLow();
	delay_ms(10);
}