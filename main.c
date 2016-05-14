/*
	The MIT License (MIT)

	Copyright (c) 2016 AaronKow

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#include "water_flow.h"

#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0

unsigned int valve2Pin = 8;		// pin 2
unsigned int valve3Pin = 9;		// pin 3
unsigned int valve4Pin = 10;		// pin 4
unsigned int pump2Pin = 11;		// pin 7

void setup_pins(void){
	gpio_export(valve2Pin);
	gpio_set_dir(valve2Pin, OUTPUT);
	gpio_export(valve3Pin);
	gpio_set_dir(valve3Pin, OUTPUT);
	gpio_export(valve4Pin);
	gpio_set_dir(valve4Pin, OUTPUT);
	gpio_export(pump2Pin);
	gpio_set_dir(pump2Pin, OUTPUT);

	/* Turn on valves & pump2 */
	gpio_set_value(valve2Pin, LOW);
	gpio_set_value(valve3Pin, LOW);
	gpio_set_value(valve4Pin, LOW);
	gpio_set_value(pump2Pin, LOW);
}

int main(int argc, char **argv, char **envp)
{
	if (argc != 4) {										// check for any arguments, if none, display messages
		printf("Error during execution:\n");
		printf("USAGE: <filename> <gpio-pin> <gpio-pin> <gpio-pin>\n\n");
		exit(-1);
	} else {											// if received arguments, display message
		printf("Program Started ... \nPlease press <enter> to exit this program.\n");
	}

	setup_pins();

	struct gpio_struct args;
	args.gpio_1 = atoi(argv[1]);							// get the first argument
	args.gpio_2 = atoi(argv[2]);							// get the first argument
	args.gpio_3 = atoi(argv[3]);							// get the first argument

	pthread_t water_sensor, interrupt_service;

	/* Thread 1 to collect water data from water sensor */
	if( pthread_create(&water_sensor, NULL, &get_water_sensor, NULL) != 0){
		printf("Error: unable to set water sensor thread\n");
		return -1;
	}

	/* Thread 2 responsible for interrupt service */
	if( pthread_create(&interrupt_service, NULL, &interrupt_func, (void *)&args) != 0){
		printf("Error: unable to set interrupt service thread\n");
		return -1;
	}

	pthread_join(interrupt_service, NULL);
	return 0;
}