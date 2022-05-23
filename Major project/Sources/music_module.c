#include "music_module.h"
#include <mc9s12dp256.h>        /* derivative information */
#include "lcd_module.h"

int note = 0;
int ready_signal = 0;

void init_music(void){
	//TSCR1 = 0x80;                 // enable timer counter and fast flags clear
 	//TIOS = 0x20;                  // enable OC5 (select channel 5 for output compare)
	//TCTL1 = 0x04;                 // configure out-put compare action (toggle the PT5 pin)
	//TIE = 0x20;                   // transmit interrupt enable for channel 5
	//TFLG1 = 0x20;                 // clear output-compare interrupt flag (C5F)
	
	TSCR1_TEN = 1;
	
  
  	TSCR2 = 0x00;   // prescaler 1, before 32 = 0x04
  	TIOS_IOS6 = 1;   // set channel 5 to output compare
    
  	TCTL1_OL6 = 1;    // Output mode for ch5
  	TIE_C6I = 1;   // enable interrupt for channel 5
  	
  	TFLG1 |= TFLG1_C6F_MASK;

	//DDRB = 0xFF;                  //  configure Port B for output 
	//DDRP = 0xFF;                  //  configure Port P for output	  
	DDRT = 0x20;
}

// The timer overflows every 2.73 ms (TCNTmax/CLOCKfreq = 65.5/24MHz)
// For a one second delay, the timer overflow needs to be cleared 366 times (1/2.73e-3)
void delay(void) {
  int i;           // define index variable
  TSCR1 = 0x80;    // enable timer counter and fast flags clear (no need to clear output-compare interrupt flag manually)
  TSCR2 = 0x00;    // configure the prescale factor to 0
  
  for(i=0; i<366; ++i){
    TFLG2 = 0x80;               // Clear timer overflow flag (TOF)
    while(!(TFLG2 & 0xFFFF));   // Wait for overflow flag to be raised (0xFFFF is the TOF mask for TFLG2)
  }   
}

void booth_music(int *note_p, int *play_p){

	// The song is an array of notes/rests
	int song[7] = {A, B, F, E, D, F, R};

	// The time left for the song is an array of digits that represent seconds
	int timeRemaining[7] = {a, b, c, d, e, f, r};


	// Global variable for length of time the song has been playing for
	int timePassed = 0;
	
	int length = 7;
	int i;
	int songLength = sizeof(song);    // length of song equals number of notes
  
	for(i=0; i<songLength; ++i){
    	*note_p = song[i];                 // loop through each note in song array  
    	//PORTB = 0xFF;                   // enable program to write to the 7-segment LEDs
    	//PORTB = timeRemaining[i];	      // count down the time remaining in the song on LED display
    
                                    // ALTERNATIVE METHOD
                                    // set global variable for length of time the song will continue playing for (timeRemaining = 0;)
                                    // increrment time by one second for each note played (timePassed) 
                                    // timeRemaining = songLength - timePassed   
        
    	// End the song after the last note
    	if(song[i] == 0x00){
      		//TIE = 0x00;     // disable all output-compare interrupts (that includes C5I)
      		//TSCR1 = 0x00;   // disable all timer interrupt flags (that includes C5F)
      		//break;          // stop sound by exiting loop and discontinuing communication with speaker
    		*play_p = 0;	
    	}
      
    	MSDelay(1000);    // implement delay so each note plays for 1s
  	}
  	*note_p = 0;
}

void play_music (int *note_p, int *play_p){
	TC6 = TCNT + (word)*note_p;    // Update interrupt pin TC5 with the current note to be played
	TFLG1 |= TFLG1_C6F_MASK;
	if (*play_p == 1){
		PTT ^= PTT_PTT5_MASK;	
	}		
}

// Interrupt Service Routine for output compare
#pragma CODE_SEG __NEAR_SEG NON_BANKED                          
__interrupt void TC6_ISR(void){    // Vector number 13 is for enhanced capture timer channel 5
  
  play_music (&note, &play_signal);
  //TFLG1 |= TFLG1_C5F_MASK;
  //TFLG1 = 0x20;        // Clear C5F coutput-compare interrupt flag 
}
