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

void flow_1(void){
	pulse_count_1++;
}

void flow_2(void){
	pulse_count_2++;
}

void flow_3(void){
	pulse_count_3++;
}

float timedifference_msec(struct timeval t0, struct timeval t1){
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

void *get_water_sensor(void *arguments){
	sleep(1);																		// sleep for 1 second after the interrupt start to prevent error
	pulse_count_1 = 0;
	pulse_count_2 = 0;
	pulse_count_3 = 0;

	flow_rate_1 = 0.0;
	flow_rate_2 = 0.0;
	flow_rate_3 = 0.0;

	ml_1 = 0;
	ml_2 = 0;
	ml_3 = 0;

	ml_total_1 = 0;
	ml_total_2 = 0;
	ml_total_3 = 0;

	calibration_factor = 5.0;														// set your pulse frequency here, reference is 4.5

	gettimeofday(&oldTime, 0);

	/*========================*/
	/* Calculating Water Flow */
	/*========================*/
	while(1){
		gettimeofday(&currentTime, 0);
		elapsed = timedifference_msec(oldTime, currentTime);
		if (elapsed > 1000){														// check every 1 second
			gettimeofday(&oldTime, 0);												// update current time
    		flow_rate_1 = ((1000.0 / (elapsed)) * pulse_count_1) / calibration_factor;
    		flow_rate_2 = ((1000.0 / (elapsed)) * pulse_count_2) / calibration_factor;
    		flow_rate_3 = ((1000.0 / (elapsed)) * pulse_count_3) / calibration_factor;

			ml_1 = (flow_rate_1 / 60) * 1000;
			ml_total_1 += ml_1;

			ml_2 = (flow_rate_2 / 60) * 1000;
			ml_total_2 += ml_2;

			ml_3 = (flow_rate_3 / 60) * 1000;
			ml_total_3 += ml_3;

      		printf("Flow rate 1: %f L/min; Current Liquid Flowing: %d ml/sec; Output Liquid Quantity: %lu ml\n", flow_rate_1, ml_1, ml_total_1);
      		printf("Flow rate 2: %f L/min; Current Liquid Flowing: %d ml/sec; Output Liquid Quantity: %lu ml\n", flow_rate_2, ml_2, ml_total_2);
      		printf("Flow rate 3: %f L/min; Current Liquid Flowing: %d ml/sec; Output Liquid Quantity: %lu ml\n\n", flow_rate_3, ml_3, ml_total_3);

      		pulse_count_1 = 0; 														// Reset Counter
      		pulse_count_2 = 0; 														// Reset Counter
      		pulse_count_3 = 0; 														// Reset Counter
		}
		usleep(100);
	}
	pthread_exit(NULL);
	return NULL;
}

void *interrupt_func(void *arguments){
	struct gpio_struct *args = arguments;
	unsigned int gpio_1 = args -> gpio_1;
	unsigned int gpio_2 = args -> gpio_2;
	unsigned int gpio_3 = args -> gpio_3;

	struct pollfd fdset[2];
	int nfds = 2;																// number of pollfd structures in the fds array
	int gpio_fd1, gpio_fd2, gpio_fd3, timeout, rc;
	char *buf[MAX_BUF];
	int len;

	gpio_export(gpio_1);
	gpio_set_dir(gpio_1, 0);														// set pin to input
	gpio_set_value(gpio_1, 1);
	gpio_set_edge(gpio_1, "falling");												// set with falling interrupt

	gpio_export(gpio_2);
	gpio_set_dir(gpio_2, 0);														// set pin to input
	gpio_set_value(gpio_2, 1);
	gpio_set_edge(gpio_2, "falling");												// set with falling interrupt

	gpio_export(gpio_3);
	gpio_set_dir(gpio_3, 0);														// set pin to input
	gpio_set_value(gpio_3, 1);
	gpio_set_edge(gpio_3, "falling");												// set with falling interrupt


	gpio_fd1 = gpio_fd_open(gpio_1);												// the open file descriptor for GPIO pin
	gpio_fd2 = gpio_fd_open(gpio_2);												// the open file descriptor for GPIO pin
	gpio_fd3 = gpio_fd_open(gpio_3);												// the open file descriptor for GPIO pin

	timeout = POLL_TIMEOUT;

	while (1) {
		memset((void*)fdset, 0, sizeof(fdset));									// clears the fdset block of memory

		fdset[0].fd = STDIN_FILENO;
		fdset[0].events = POLLIN;
      
		fdset[1].fd = gpio_fd1;
		fdset[1].events = POLLPRI;

		fdset[2].fd = gpio_fd2;
		fdset[2].events = POLLPRI;

		fdset[3].fd = gpio_fd3;
		fdset[3].events = POLLPRI;

		rc = poll(fdset, nfds, timeout);      

		if (rc < 0) {															// if poll is unsuccessful, stop thread
			printf("\npoll() failed!\n");
			
			pthread_exit(NULL);
			return NULL;
		}
      
		if (rc == 0) {															// if poll returns 0 then call timed out, loop again in this case
			// printf("Responding ...\n");
		}
            
		if (fdset[1].revents & POLLPRI) {
			len = read(fdset[1].fd, buf, MAX_BUF);
			flow_1();
		}

		if (fdset[2].revents & POLLPRI) {
			len = read(fdset[2].fd, buf, MAX_BUF);
			flow_2();
		}

		if (fdset[3].revents & POLLPRI) {
			len = read(fdset[3].fd, buf, MAX_BUF);
			flow_3();
		}

		if (fdset[0].revents & POLLIN) {										// keyboard input from the terminal to stop this program
			(void)read(fdset[0].fd, buf, 1);

			gpio_fd_close(gpio_fd1);
			gpio_unexport(gpio_1);

			gpio_fd_close(gpio_fd2);
			gpio_unexport(gpio_2);

			gpio_fd_close(gpio_fd3);
			gpio_unexport(gpio_3);

			printf("Program successfully ended\n");

			pthread_exit(NULL);
			return NULL;
		}

		fflush(stdout);															// flash the standard output and keep looping until we freeze the program ^c
	}
}