#include <mc9s12dp256.h>        /* derivative information */
#include <stdio.h>
// heading files
#include "simple_serial.h"
#include "lidar_processing.h"
#include "lcd_module.h"
#include "music_module.h"
#include "serial_encoder.h"

// status register
int current_status;
int pre_status = VACANCY;
unsigned long booth_distance[NUM_OF_BOOTH];
int boothNum = 0;
int start_laser = 1;
int usage_mode = 2;


// this function will print out the message to Putty
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
	
	switch (*c_status){
		case VACANCY:
			sprintf(buffer, "Booth %i - Distance: % lu - Object not found!\r\n", (*booth_num + 1), *sample);
			*play_p = 1;
			booth_music(music_p, play_p);
			if (usage_mode == 1){
				lcdDisp((*booth_num + 1));
  				MSDelay(10000);
  				Init_LCD();	
			}
		
			break;
		case OCCUPIED:
			sprintf(buffer, "Booth %i - Distance: % lu - Object found!\r\n", (*booth_num + 1), *sample);
			break;	
	}

	if (usage_mode == 1){
		SerialOutputString(buffer, &SCI1);	
	}
	else{
	  SendDataMsg(angle_p, sample, booth_dis);	
	}
	
	

	*p_status = *c_status;
	
}

void sample_filter (int *angle_p, unsigned long *sample, unsigned long *base, unsigned long *filted, int *count, int *start_signal, int *start_laser, int *music_p, int *play_p, unsigned long *booth_dis, int *booth_num){
	char buffer[64];
	int count_limit;
	// set up filter count for the lidar
	if (*start_signal == 1){
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
				*base = *base + *sample;
				break;
			default:
				*filted = *filted + *sample;
				break;
		}
		*count = *count + 1;			
	}
	else{
		*start_laser = 0;
		switch (*start_signal){
			case 1:
				*base = (*base + *sample)/(*count + 1);
				booth_dis = booth_dis + *booth_num;
				*booth_dis = *base;
				sprintf(buffer, "Base distance of %i: %lu\r\n", (*booth_num + 1), *base);
				if (usage_mode == 1){
  					SerialOutputString(buffer, &SCI1);	
				}
				else{
				  	SendTextMsg(buffer);	
				}
				if (*booth_num >= (NUM_OF_BOOTH-1)){
					*start_signal = 0;	
				}
				*base = 0;
				break;
			default:
				*filted = (*filted + *sample)/(*count + 1);
				message(angle_p, filted, &current_status, &pre_status, music_p, play_p, booth_dis, booth_num);
				break;
		}
		*count = 0;
		*filted = 0;
	}
	
}