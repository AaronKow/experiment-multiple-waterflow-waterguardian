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

#ifndef WATER_FLOW_H_
#define WATER_FLOW_H_

#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include "interrupt.h"

struct gpio_struct {
	unsigned int gpio_1;
	unsigned int gpio_2;
	unsigned int gpio_3;
};
struct timeval currentTime, oldTime;

volatile int pulse_count_1;
volatile int pulse_count_2;
volatile int pulse_count_3;
unsigned int ml_1, ml_2, ml_3;						// millilitre
unsigned long ml_total_1, ml_total_2, ml_total_3;	// total millilitre
float elapsed, calibration_factor, flow_rate_1, flow_rate_2, flow_rate_3;
float timedifference_msec(struct timeval t0, struct timeval t1);

void flow_1(void);
void flow_2(void);
void flow_3(void);
void *get_water_sensor(void *arguments);
void *interrupt_func(void *arguments);

#endif