#include "tasks.hpp"
#include "utilities.h"
#include "io.hpp"
#include <iostream>
#include <string>
using namespace std;

void write_to_sram(string address, string data);
char read_from_sram();
void setDataIn();

//Push Test
int main(void) {
	printf("Welcome to the SJOne Board Interface")	
}

void write_to_sram(char *address, char *data)
{
	int a_bits[8];
	int b_bits[8];
	//Set Address on SJOne
	dir_w.setHigh();
	dataOut_w.setLow();
	dataIn_w.setLow();
	cmd_w.setLow();

	bus_e#.setLow();
	addr_w.setHigh();
	addr_w.setLow();
	//Set Data Bits on SJOne
	dataOut_w.setHigh();
	//Set Command Register Bits
	//sel_sram=1; w=1;

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
void setData(string byte, int* bits)
{
	string data=byte;
	for(int i=0; i<8;i++)
	{
		bits[i]=data[i];
	}
}
