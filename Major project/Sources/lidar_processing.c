#include <mc9s12dp256.h>        /* derivative information */
#include <stdio.h>
// heading files
#include "simple_serial.h"
#include "lidar_processing.h"
#include "lcd_module.h"
#include "music_module.h"
#include "serial_encoder.h"

// Global variables
int current_status; // current status of the booth
int pre_status = VACANCY; // previous status of the booth
unsigned long booth_distance[NUM_OF_BOOTH]; // array to store the original booth's distance
int boothNum = 0; // current booth
int start_laser = 1; // flag indicate the start of the laser in the lidar
int usage_mode = 2; // flag indicate the mode of the hardware
		    // 1 - is LCD
		    // 2 - is user interface

// this function will print out the message
void message (int *angle_p, unsigned long *sample, int *c_status, int *p_status, int *music_p, int *play_p, unsigned long *booth_dis, int *booth_num){
	char buffer[128];
	booth_dis = booth_dis + *booth_num;
    // if object is detected
	if (*sample <= *booth_dis - 3000){
		*c_status = OCCUPIED;	
	}
	else {
		*c_status = VACANCY;			
	}
	// output message to the buffer base in the current status of the booth
	switch (*c_status){
		// if the current status is VACANCY
		case VACANCY:
			sprintf(buffer, "Booth %i - Distance: % lu - Object not found!\r\n", (*booth_num + 1), *sample);
			*play_p = 1; // set the play flag to the music module
			booth_music(music_p, play_p); // start music module of the hardware
			// if the LCD mode is used
			if (usage_mode == 1){
				// display the message with the booth number to the LCD
				lcdDisp((*booth_num + 1));
  				MSDelay(10000); // delay
  				Init_LCD(); // reset the LCD	
			}
			break;
		// if the current status is OCCUPIED
		case OCCUPIED:
			sprintf(buffer, "Booth %i - Distance: % lu - Object found!\r\n", (*booth_num + 1), *sample);
			break;	
	}
	// if the LCD mode is used
	if (usage_mode == 1){
		SerialOutputString(buffer, &SCI1);	
	}
	// if the user interface mode is used
	else{
	  // send packed data to the serial port
	  SendDataMsg(angle_p, sample, booth_dis);	
	}
	// set the previous status
	*p_status = *c_status;
	
}

// this function acts as a filter for lidar value
void sample_filter (int *angle_p, unsigned long *sample, unsigned long *base, unsigned long *filted, int *count, int *start_signal, int *start_laser, int *music_p, int *play_p, unsigned long *booth_dis, int *booth_num){
	char buffer[64];
	int count_limit; // total number of lidar variable
	// set up filter count for the lidar
	// if the start flag is set
	if (*start_signal == 1){
		// total number of lidar value
		count_limit = 3000;
	}
	else{
	  if (usage_mode == 1){
		  count_limit = 200;
	  } else{
	    count_limit = 350;
	  }
	}
	// signal filter 
	if (*count < count_limit){
		switch (*start_signal){
			case 1:
				*base = *base + *sample; // adding lidar value
				break;
			default:
				*filted = *filted + *sample; // adding lidar value
				break;
		}
		*count = *count + 1; // increase the count			
	}
	else{
		*start_laser = 0; // set the start laser flag to 0
		switch (*start_signal){
			case 1:
				*base = (*base + *sample)/(*count + 1); // adding the final lidar value then calculate the average
				booth_dis = booth_dis + *booth_num; // get the address of the booths' distance array
				*booth_dis = *base; // get the booth distance
				sprintf(buffer, "Base distance of %i: %lu\r\n", (*booth_num + 1), *base);
				// if the LCD mode is used
				if (usage_mode == 1){
  					SerialOutputString(buffer, &SCI1);	
				}
				// if the user interface mode is used, send the  packed serial message to serial port
				else{
				  	SendTextMsg(buffer);	
				}
				// when the final booth is reached, starting protocal is finished
				// From this point, the ordinary process is started
				if (*booth_num >= (NUM_OF_BOOTH-1)){
					*start_signal = 0;	
				}
				// reset the booth distance value
				*base = 0;
				break;
			default:
				*filted = (*filted + *sample)/(*count + 1); // adding the final lidar value then calculate the average
				// send the variables to the create the message
				message(angle_p, filted, &current_status, &pre_status, music_p, play_p, booth_dis, booth_num);
				break;
		}
		*count = 0; // reset the current count
		*filted = 0; // reset the lidar value
	}
	
}
