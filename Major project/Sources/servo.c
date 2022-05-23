
#include "derivative.h"
#include <math.h> 
#include "servo.h"
#include "lidar_processing.h"


#define ZERO_ELEVATION_DUTY 4350
#define ZERO_AZIMUTH_DUTY 2000

int delay_flag = 1;
unsigned int delay_count = 0;
// variables to make the servo move back and forth
// note: This is just to demonstrate the function of the servo
int iterator_counter = 0;
int toggle = 0;

void PWMinitialise(void){
    // set PP5 and PP7 for pwm output 
    PWMCLK = 0; // select clock A
    PWMPOL = 0xA0; // PWM5 and PWM7 output start high
    PWMCTL = 0xC0; // 16-bit PWM: use PWM4 and PWM5 for PWM5, PWM6 and PWM7 for PWM7
    PWMCAE = 0; // Center aligned
    PWMPRCLK = 0x33; // PWM Clock prescaler to 8 

    // set the PWM period appropriate for servos
    PWMPER45 = 0xEA6A;
    PWMPER67 = 0xEA6A;

    // set the initial duty cycle for both servos
    PWMDTY45 = ZERO_ELEVATION_DUTY;
    PWMDTY67 = ZERO_AZIMUTH_DUTY;
    
    PWME  |= 0xFF;      // enable PWM0
}

void setServoPose(int azimuth, int elevation){  
    //PWMDTY45 = (int)(ZERO_ELEVATION_DUTY + elevation);  // Sets elevation to duty cycle using PWM 45
    PWMDTY67 = (int)(ZERO_AZIMUTH_DUTY + azimuth);   // Sets azimuth to duty cycle using PWM 67
    
    // For statistic lidar
    PWMDTY45 = (int)(ZERO_ELEVATION_DUTY);
    //PWMDTY67 = (int)(ZERO_AZIMUTH_DUTY);
}


void Init_TC4 (void) {
  TSCR1_TEN = 1;
  
  TSCR2 = 0x00;   // prescaler 1, before 32 = 0x04
  TIOS_IOS4 = 1;   // set channel 6 to output compare
    
  TCTL1_OL4 = 1;    // Output mode for ch6
  TIE_C4I = 1;   // enable interrupt for channel 6

  TFLG1 |= TFLG1_C4F_MASK;
}

// the interrupt for timer 6 which is used for cycling the servo
#pragma CODE_SEG __NEAR_SEG NON_BANKED /* Interrupt section for this module. Placement will be in NON_BANKED area. */
__interrupt void TC4_ISR(void) { 
   
  TC4 = TCNT + 24000;   // interrupt delay depends on the prescaler
  TFLG1 |= TFLG1_C4F_MASK;
  
  // if the delay flag is detected
  if (delay_flag == 0){
  	if (toggle == 0){
  		iterator_counter = iterator_counter + 1000; // make the next location of the servo, the servo will rotate to the right
  		boothNum = boothNum + 1; // increase the booth number
  		start_laser = 1; // set the start laser flag to 1
  	}
	else{
	    iterator_counter = iterator_counter - 1000; // make the next location of the servo, the servo will rotate to the left
	    boothNum = boothNum - 1; // decrease the booth number
	    start_laser = 1; // set the start laser flag to 1	
	}
	
	// if the servo reach the final booth
	if (iterator_counter >= 2000) {
	   	toggle = 1;
	} 
	// if the servo reach the first booth
	else if (iterator_counter <= 0) {
		toggle = 0;
	}
	// rotate the servo	
  	setServoPose(iterator_counter, iterator_counter);
  	delay_flag = 1; // start the delay
  	delay_count = 0; // count the number of delay	
  }
  else{
  	delay_count = delay_count + 1; // increase the delay count
  	// delay for 8 s
	if (delay_count >= 8000){
  		delay_flag = 0;
  	}
  }    
}


