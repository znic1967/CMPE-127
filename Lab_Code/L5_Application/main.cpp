#include "tasks.hpp"
#include "utilities.h"
#include "io.hpp"
#include <iostream>
#include <string>
using namespace std;

void write_to_sram(string address, string data);
char read_from_sram();
void setDataIn();
void pin_setter(char byte);

//Push Test
int main(void) {
	printf("Welcome to the SJOne Board Interface")	
}

void write_to_sram(char address, char data)
{
	pin_setter(address); //Set Address on SJOne

	//Disable all 373s
	dir_w.setHigh();
	bus_en#.setHigh();
	dataOut_w.setLow();
	dataIn_e#.setHigh();
	cmd_w.setLow();

	//Pass Address to SRAM
	bus_e#.setLow();
	addr_w.setHigh();
	delay_ms(1); //Allow address to get to SRAM
	addr_w.setLow();
	
	pin_setter(data); //Set Data Bits on SJOne
	dataOut_w.setHigh();
	pin_setter(00000000); //Clear cmd register
	delay_ms(1);
	pin_setter()


}
string read_from_sram()
{
	setdataIn();
	//Set Address
	addr_w.setHigh();
	addr_w.setLow();
	//Set Data
	

}
void setDataIn
{
	bus_e#.setHigh();
	
	dataIn_e#.setLow();
	addr_w.setLow();
	setData(byte,&address); 
	setData(byte,&address);
	dataOut_w.setLow();
	cmd_w.setLow();
}
void setDataOut()
{
	dir_w.setHigh();
	bus_e#.setLow();
	addr_w.setLow();
	dataOut_w
}
void setAsOutput()
{
	dir_w.setAsOutput();
	bus_e#.setAsOutput();
	addr_w.setAsOutput();
	dataOut_w.setAsOutput();
	dataIn_e#.setAsOutput();
	cmd_w.setAsOutput();
}
void setAsInput()
{
	dir_w.setAsInput();
	bus_e#.setAsInput();
	addr_w.setAsInput();
	dataOut_w.setAsInput();
	dataIn_e#.setAsInput();
	cmd_w.setAsInput();
}

void setGPIOs
{
	GPIO  a0(P1_29);
	GPIO  a1(P1_28);
	GPIO  a2(P1_23);
	GPIO  a3(P1_22);
	GPIO  a4(P1_20);
	GPIO  a5(P1_19);
	GPIO  a6(P0_30);
	GPIO  a7(P0_29);

	GPIO  dir_w(P0_0);
	GPIO  bus_e#(P0_1);
	GPIO  addr_w(P2_1);
	GPIO  dataOut_w(P2_2);
	GPIO  datain_e#(P2_3);
	GPIO  cmd_w(P2_40);
}

void pin_setter(char* byte)
{
	char* bits=byte;
	int index=7;
	for(int i=0; i<8; i++)
	{
		bits[i]=atoi(data[index]);
		index--;
	}
	if(bits[0]) a0.setHigh();
	else a0.setLow();

	if(bits[1]) a1.setHigh();
	else a1.setLow();

	if(bits[2]) a2.setHigh();
	else a2.setLow();

	if(bits[3]) a3.setHigh();
	else a3.setLow();

	if(bits[4]) a4.setHigh();
	else a4.setLow();

	if(bits[5]) a5.setHigh();
	else a5.setLow();

	if(bits[6]) a6.setHigh();
	else a6.setLow();

	if(bits[7]) a7.setHigh();
	else a7.setLow();

}
