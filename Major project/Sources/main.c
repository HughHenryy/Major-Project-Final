#include <hidef.h>      /* common defines and macros */
#include <assert.h>
#include "derivative.h"      /* derivative-specific definitions */

// need this for string functions
#include <stdio.h>
// include all heading files
#include "pll.h"
#include "simple_serial.h"
#include "l3g4200d.h"
#include "servo.h"
#include "laser.h"
#include "gyro.h"
#include "iic.h"
#include "lidar_processing.h"
#include "lcd_module.h"
#include "music_module.h"
// create univeral variables
int sample_count = 0; // count how many time laser.c has been called 
unsigned long filted_sample = 0; // final lidar value after filting process

// funtion to print out errors
void printErrorCode(IIC_ERRORS error_code) {
  char buffer[128];  
  switch (error_code) {
    case NO_ERROR: 
      sprintf(buffer, "IIC: No error\r\n");
      break;
    
    case NO_RESPONSE: 
      sprintf(buffer, "IIC: No response\r\n");
      break;
    
    case NAK_RESPONSE:
      sprintf(buffer, "IIC: No acknowledge\r\n");
      break;
    
    case IIB_CLEAR_TIMEOUT:
      sprintf(buffer, "IIC: Timeout waiting for reply\r\n");
      break;
    
    case IIB_SET_TIMEOUT: 
      sprintf(buffer, "IIC: Timeout not set\r\n");
      break;
    
    case RECEIVE_TIMEOUT:
      sprintf(buffer, "IIC: Received timeout\r\n");
      break;
    
    case IIC_DATA_SIZE_TOO_SMALL:
      sprintf(buffer, "IIC: Data size incorrect\r\n");
      break;

    default:
      sprintf(buffer, "IIC: Unknown error\r\n");
      break;
  }
  
  if (usage_mode == 1){
  	SerialOutputString(buffer, &SCI1);	
  }
  else{
  	SendTextMsg(buffer);	
  }
  
}

void main(void) {

  AccelRaw read_accel;
  AccelScaled scaled_accel;

  GyroRaw read_gyro;
  // MagRaw read_magnet;
  
  IIC_ERRORS error_code = NO_ERROR;
 
  char buffer[128]; // message buffer  
  
  unsigned long singleSample; // lidar value
  
  unsigned long baseSample = 0; // base distance of each booth
  unsigned long filtedSample = 0; // lidar value after filting process
  int count = 0; 
  int start_signal = 1; // indicate the initial process - scaning for booths' distance
   
  //assert(error_code != NO_ERROR);

  #ifndef SIMULATION_TESTING

  // make sure the board is set to 24MHz
  //  this is needed only when not using the debugger
  PLL_Init();

  // initialise PWM
  PWMinitialise();
  setServoPose(100, 100);

  #endif
  
  // initialise the simple serial
  SerialInitialise(BAUD_9600, &SCI1);
  
  #ifndef SIMULATION_TESTING
  
  // initialise the sensor suite
  error_code = iicSensorInit();
  
  // write the result of the sensor initialisation to the serial
  if (error_code == NO_ERROR) {
    sprintf(buffer, "NO_ERROR\r\n");
  	if (usage_mode == 1){
  		SerialOutputString(buffer, &SCI1);	
	}
	else{
	  	SendTextMsg(buffer);	
	}
  } else {
    sprintf(buffer, "ERROR %d\r\n");
    if (usage_mode == 1){
  		SerialOutputString(buffer, &SCI1);	
	}
	else{
	  	SendTextMsg(buffer);	
	}
  }

  init_music(); // init the music module for the hardware
  laserInit(); // init the laser in the lidar
  
  #else
  
  #endif

  Init_TC4(); // init compare port 4
  
	EnableInterrupts;
  //COPCTL = 7;
  _DISABLE_COP();
  
  // booths' distance scanning process at the begining of the start 
  while (start_signal == 1){
	// if laser start signal is true
  	if (start_laser == 1){
  		GetLatestLaserSample(&singleSample);
  		sample_filter (&iterator_counter, &singleSample, &baseSample, &filtedSample, &count, &start_signal, &start_laser, &note, &play_signal, &booth_distance[0], &boothNum);	
  	}		
  }
    
  for(;;) {
  
    #ifndef SIMULATION_TESTING
  
    // read the raw values
    error_code = getRawDataGyro(&read_gyro);   
    if (error_code != NO_ERROR) {
      printErrorCode(error_code);   
       
      error_code = iicSensorInit();
      printErrorCode(error_code);   
    }
    
    error_code = getRawDataAccel(&read_accel);
    if (error_code != NO_ERROR) {
      printErrorCode(error_code);   
    
      error_code = iicSensorInit();
      printErrorCode(error_code); 
    }
    
    //error_code = getRawDataMagnet(&read_magnet);
    
    GetLatestLaserSample(&singleSample);
        
    #else
    
    // inject some values for simulation
    read_gyro.x = 123; read_gyro.y = 313; read_gyro.z = 1002;
    read_accel.x = 124; read_accel.y = 312; read_accel.z = 2002;
    read_magnet.x = 125; read_magnet.y = 311; read_magnet.z = 3002;
    
    #endif

    // convert the acceleration to a scaled value
    convertUnits(&read_accel, &scaled_accel);    
    
    // format the string of the sensor data to go the the serial    
    //sprintf(buffer, "%lu, %d, %d, %d, %.2f, %.2f, %.2f\r\n", singleSample, read_gyro.x, read_gyro.y, read_gyro.z, scaled_accel.x, scaled_accel.y, scaled_accel.z);
    //sprintf(buffer, "%lu\n", singleSample);
    // output the data to serial
    //sample_filter (&singleSample, &baseSample, &filtedSample, &count, &start_signal, &boothNum, &note, &play_signal);
    
    // if laser start signal is true, start lidar scaning process
    if (start_laser == 1){
    	sample_filter (&iterator_counter, &singleSample, &baseSample, &filtedSample, &count, &start_signal, &start_laser, &note, &play_signal, &booth_distance[0], &boothNum);	
    }
    
    
    //_FEED_COP(); /* feeds the dog */
  } /* loop forever */
  
  /* please make sure that you never leave main */
}
