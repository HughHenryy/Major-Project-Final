#include <mc9s12dp256.h>        /* derivative information */
#include <stdio.h>
#include <stdlib.h>

#include "lcd_module.h"

void Init_LCD(void){
    DDRK = 0xFF;   
    COMWRT4(0x33);        //reset sequence provided by data sheet
    MSDelay(1);
    COMWRT4(0x32);        //reset sequence provided by data sheet
    MSDelay(1);
    COMWRT4(0x28);        //Function set to four bit data length
                          //2 line, 5 x 7 dot format
    MSDelay(1);
    COMWRT4(0x06);        //entry mode set, increment, no shift
    MSDelay(1);
    COMWRT4(0x0E);        //Display set, disp on, cursor on, blink off
    MSDelay(1);
    COMWRT4(0x01);        //Clear display
    MSDelay(1);
    COMWRT4(0x80);        //set start posistion, home position
    MSDelay(1);
}



void writeStringLCD(char * message){
    
    int i=0;  //initalise i =0
        
    while (message[i] != 0x00) {     //loop while thers charactes left in the string
        DATWRT4(message[i++]);       //print the charactes to the LCD
        MSDelay (2);                //delay for a short period of thime
    }
}



void writeSIntLCD(int number){
    
    char str[8];                      //define chars arrays for storing the strings
    
    sprintf(str, "%d", number);       //store the interger value of the number in string 1
    
    writeStringLCD(str);              //write the number to the LCD
    
  
}



void writeStringAndSIntLCD(char * message, int number ){
  COMWRT4(0x01);                      //move the curose to the top left corrner of the LCD
  writeStringLCD(message);            //print message to LCD
  writeSIntLCD(number);               //write number straight after the string
  COMWRT4(0x80);                      //set start posistion, home position
  MSDelay(1);                         //delay for a short period of time
}


void COMWRT4(char command)
  {
        char x;
        
        x = (command & 0xF0) >> 2;    //shift high nibble to center of byte for Pk5-Pk2
        LCD_DATA =LCD_DATA & ~0x3C;   //clear bits Pk5-Pk2
        LCD_DATA = LCD_DATA | x;      //sends high nibble to PORTK
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~RS;    //set RS to command (RS=0)
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;     //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;    //Drop enable to capture command
        MSDelay(15);                  //wait
        x = (command & 0x0F)<< 2;     // shift low nibble to center of byte for Pk5-Pk2
        LCD_DATA =LCD_DATA & ~0x3C;   //clear bits Pk5-Pk2
        LCD_DATA =LCD_DATA | x;       //send low nibble to PORTK
        LCD_CTRL = LCD_CTRL | EN;     //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;    //drop enable to capture command
        MSDelay(15);
  }

void DATWRT4(char data)
  {
  
        char x;
       
        
        
        x = (data & 0xF0) >> 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | RS;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(5);
       
        x = (data & 0x0F)<< 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(15);
  }


 void MSDelay(unsigned int itime)
  {
    unsigned int i; unsigned int j;
    for(i=0;i<itime;i++)              //cycle through loop incrementing i till it reaches itime 
    for(j=0;j<1000;j++);              //cycle through loop incrementing j till it reaches 1000
 }  
 
 void lcdDisp(int boothNumber) {      // function that displayes the free booth number on lcd 
  
    char * message = "EMPTY BOOTH:";  // string goes to the front of the lcd
    
    int number = boothNumber;         // booth number that goes afterwards
  
    Init_LCD();                       // initialize lcd
    
    writeStringLCD(message);          // display the string "message"
    writeSIntLCD(number);             // display the booth number afterwards

 }
    
    
    
    