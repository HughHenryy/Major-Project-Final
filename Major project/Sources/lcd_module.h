#ifndef LCD_HEADER
#define LCD_HEADER
  
#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

/*! \brief initalses the LCD screen and set the cursor to the first row firt column
        
*/
void Init_LCD(void);

/*! \brief moves the printing location of the LCD.
    \param Position to move the cursor to in Hexadecimal
      
*/
void COMWRT4(char);

/*!
	\brief Write a byte of data to the location of the cursor on the LCD.
*/

void DATWRT4(char);

/*! \brief delays the program by a given amount of time.
    \param the scaling value for the delay
*/
void MSDelay(unsigned int);


/*! \brief Write a given string to the LCD screen. Writes the data byte-wise until a null character is written. 
    \param message The string to be written
*/
void writeStringLCD(char * message);
/*! \brief Write a given integer to the LCD screen. 
    \param number the number to be written
*/
void writeSIntLCD(int number);



/*! \brief Write a given string and signed integer to the LCd screen. The string will be written first, followed immediately with the integer.
    \param message The string to be written
    \param number the number to be written
      
*/
void writeStringAndSIntLCD(char * message, int number );


/*  \brief Write a given integer empty booth to the LCD screen. some functions in this header file including writeStringLCD and writeSIntLCD 
    \are used to form the combination of writing both string and integer
*/
void lcdDisp(int emptyBooth);

#endif