#include "tasks.hpp"
#include "utilities.h"
#include "io.hpp"
#include <iostream>
#include <string>
using namespace std;

void write_to_sram(char* address, char* data);
void read_from_sram(char* address);
void pin_setter(char byte);
int bit_checker(char* bits)

//Push Test
int main(void) {
	printf("Welcome to the SJOne Board Interface.\n");
	setGPIOs();
	char selector='0';
	char address[8]='0';
	char data[8]='0';
	while(selector!='e')
	{
		printf("Select an option:\n");
		printf("1) Write to SRAM.\n");
		printf("2) Read from SRAM.\n");
		printf("Enter \"e\" to quit.");
		scanf("%c", selector);

		if(selector=='1')
		{
			printf("Enter 8-bit Address to write to: ");
			scanf("%s", address);
			if(bit_checker(address)) 
			{

				printf("Enter 8-bit data to write: ");
				scanf("%s",data);
				if(bit_checker(data))
				{
					write_to_sram(address, data);
				}
				else printf("Data length not 8 bits.\n");
			}
			else printf("Address length not 8 bits.\n");
		}
		if (selector=='2')
		{
			printf("Enter 8-bit Address to read from: ");
			scanf("%s", address);
			if(bit_checker(address)) 
			{
				read_from_sram(address);
			}
			else printf("Address length not 8 bits.\n");
		}
	}
	printf("SJOne Board Interface Exited.\n");
	printf("Interface brought to you from countless 4AM SCE wiring wrapping sessions.\n");

}

void write_to_sram(char* address, char* data)
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

	//Reset state machine
	pin_setter(00000000);
	cmd_w.setHigh();
	delay_ms(1);
	cmd_w.setLow();

	pin_setter(01000100); //Latches cmd register
	pin_setter(data); //Set Data Bits on SJOne
	dataOut_w.setHigh();
	cmd_w.setHigh(); //Starts the state machine
	printf("Write Operation Complete.\n");
}
void read_from_sram(char address)
{
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
	addr_w.setLow(); //Address latched.

	//Reset state machine
	pin_setter(00000000);
	cmd_w.setHigh();
	delay_ms(1);
	cmd_w.setLow();

	pin_setter(01001000); //Latches cmd register
	dataIn_e#.setLow();
	cmd_w.setHigh(); //Starts the state machine
	printf("Read Operation Complete.\n");
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
	GPIO  dataIn_e#(P2_3);
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

int bit_checker(char* bits)
{
	int counter=0;
	char str[100]=*bits;
	for(int i=0; i<(unsigned)strlen(str); i++) counter++;
	if (counter==8) return 1;
	else return 0;
}
