#include "tasks.hpp"
#include "utilities.h"
#include "io.hpp"
#include "gpio.hpp"
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
using namespace std;
void write_to_sram(string address, string data);
void read_from_sram(string address);
void pin_setter(string data);
int bit_checker(string bits);
void disable373s();
void setGPIOs();
void setAsOutput();
void setAsInput();
void toggle_clock(int control);
void smReset();
void rGPIO();

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

int main(void) {

	dir_w.setAsOutput();
	bus_eL.setAsOutput();
	addr_w.setAsOutput();
	dataOut_w.setAsOutput();
	dataIn_eL.setAsOutput();
	cmd_w.setAsOutput();
	clk.setAsOutput();
	disable373s();
	cout<<"Test Shit"<<endl;
	printf("Welcome to the SJOne Board Interface.\n");
	char selector='0';
	string address="";
	string data="";
	string test="test";
	while(selector!='e')
	{
		printf("Select an option:\n");
		printf("1) Write to SRAM.\n");
		printf("2) Read from SRAM.\n");
		printf("Enter \"e\" to quit.\n");
		cin>>selector;

		if(selector=='1')
		{
			printf("Enter 8-bit Address to write to: ");
			cin>>address;
			cout<<endl;
			if(bit_checker(address)) 
			{

				printf("Enter 8-bit data to write: ");
				cin>>data;
				cout<<endl;
				if(bit_checker(data))
				{
					cout<<"Writing data <"<<data<<"> to address <"<<address<<">."<<endl;
					write_to_sram(address, data);
				}
				else printf("Data length not 8 bits.\n");
			}
			else printf("Address length not 8 bits.\n");
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
		else cout<<"Choose the right selector"<<endl;
	}
	printf("SJOne Board Interface Exited.\n");
	printf("Interface brought to you from countless 4AM SCE wiring wrapping sessions.\n");
	cout<<"Hit reset button to restart program."<<endl;
	while(1)
	{
		delay_ms(1000);
	}

}

void write_to_sram(string address, string data)
{
	string wOp="01000100";
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

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //1

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //2

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //3

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //4

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //5

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100);

	printf("Write Operation Complete.\n");
}
void read_from_sram(string address)
{
	string rOp="01001000";
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

	smReset();

	pin_setter(rOp); //Latches cmd register
	dir_w.setLow(); //SJOne<-SRAM
	dataIn_eL.setLow();
	setAsInput();
	cmd_w.setHigh();

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //1

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //2

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //3

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //4

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100); //5

	clk.setLow();
	delay_ms(100);
	clk.setHigh();
	delay_ms(100);

	rGPIO(); //Prints GPIO Data
	printf("Read Operation Complete.\n");
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

void smReset()
{
	string pins="00000000";
	//Reset state machine
	pin_setter(pins);
	cmd_w.setHigh();
	delay_ms(10);
	cmd_w.setLow();
}
void rGPIO()
{
	cout<<"Data: "<<a7.read()<<a6.read()<<a5.read()<<a4.read()<<a3.read()<<a2.read()<<a1.read()<<a0.read()<<endl;
}
