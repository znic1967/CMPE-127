#include "tasks.hpp"
#include "utilities.h"
#include "io.hpp"
#include "gpio.hpp"
#include <stdio.h>
#include <cstring>

void write_to_sram(char address[], char data[]);
void read_from_sram(char address[]);
void pin_setter(char byte[]);
int bit_checker(char bits[]);
void disable373s();
void setGPIOs();
void setAsOutput();
void setAsInput();

int main(void) {

	setGPIOs();
	disable373s();

	printf("Welcome to the SJOne Board Interface.\n");
	char selector='0';
	char address[8]="";
	char data[8]="";
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
		if (selector!='1'||'2'||'e')
		{
			printf("Enter correct selector value.\n");
		}
	}
	printf("SJOne Board Interface Exited.\n");
	printf("Interface brought to you from countless 4AM SCE wiring wrapping sessions.\n");

}

void write_to_sram(char address[], char data[])
{
	setGPIOs();
	disable373s();
	setAsOutput();
	dir_w.setHigh(); //SJOne->SRAM
	pin_setter(address); //Set Address on SJOne

	//Pass Address to SRAM
	bus_eL.setLow();
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

	toggle_clock(1);
	cmd_w.setHigh(); //Starts the state machine
	delay_ms(1000); //Let state machine finish
	toggle_clock(0);

	printf("Write Operation Complete.\n");
}
void read_from_sram(char address[])
{
	setGPIOs();
	disable373s();
	dir_w.setHigh(); //Set as output for commands.
	pin_setter(address);

	//Pass Address to SRAM
	bus_eL.setLow();
	addr_w.setHigh();
	delay_ms(1); //Allow address to get to SRAM
	addr_w.setLow(); //Address latched.

	//Reset state machine
	pin_setter(00000000);
	cmd_w.setHigh();
	delay_ms(1);
	cmd_w.setLow();

	pin_setter(01001000); //Latches cmd register
	dir_w.setLow(); //SJOne<-SRAM
	dataIn_eL.setLow();

	toggle_clock(1);
	cmd_w.setHigh(); //Starts the state machine
	delay_ms(1000); //Let state machine finish
	toggle_clock(0);

	printf("Read Operation Complete.\n");
}

void setGPIOs()
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
	GPIO  bus_eL(P0_1);
	GPIO  addr_w(P2_1);
	GPIO  dataOut_w(P2_2);
	GPIO  dataIn_eL(P2_3);
	GPIO  cmd_w(P2_40);
	GPIO clk(P2_6);

	dir_w.setAsOutput();
	bus_eL.setAsOutput();
	addr_w.setAsOutput();
	dataOut_w.setAsOutput();
	dataIn_eL.setAsOutput();
	cmd_w.setAsOutput();
	clk.setAsOutput();
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

void pin_setter(char byte[])
{
	char bits[8]=byte;
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

int bit_checker(char bits[])
{
	int counter=0;
	char str[100]=bits;
	for(int i=0; i<(unsigned)strlen(str); i++) counter++;
	if (counter==8) return 1;
	else return 0;
}

void toggle_clock(int control)
{
		while(control==1)
		{
			clk.setHigh();
			delay_ms(100);
			clk.setLow();
		}
	}
}

void disable373s()
{
	bus_enL.setHigh();
	addr_w.setLow();
	dataOut_w.setLow();
	dataIn_eL.setHigh();
	cmd_w.setLow();
}

