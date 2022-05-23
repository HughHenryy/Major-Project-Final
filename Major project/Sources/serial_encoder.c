#include <mc9s12dp256.h>        /* derivative information */
#include <stdio.h>
// heading files
#include "simple_serial.h"
#include "lidar_processing.h"
#include "lcd_module.h"
#include "music_module.h"
#include "serial_encoder.h"

void SendTextMsg(char* text_message) {
  struct MSG_HEADER text_header = {0xABCD, "text", 0, 0, 0xDCBA};
  text_header.msg_size = strlen(text_message);
  text_header.header_time = TCNT;
  
  SerialOutputBytes((char*)&text_header, sizeof(struct MSG_HEADER), &SCI1);  
  SerialOutputBytes(text_message, text_header.msg_size, &SCI1);
}

void SendDataMsg(int *angle_in, unsigned long *sample_in, unsigned long *base_in) {
  struct MSG_HEADER data_header = {0xABCD, "DATA", 0, 0, 0xDCBA};
  struct MSG_DATA data_message = {0x9876, 0, 0, 0, 0};
                             
  data_header.msg_size = sizeof(struct MSG_DATA);
  data_header.header_time = TCNT;
  
  data_message.last_sample_time = TCNT;
  data_message.angle = *angle_in;
  data_message.sample = *sample_in;
  data_message.base = *base_in;
  
  SerialOutputBytes((char*)&data_header, sizeof(struct MSG_HEADER), &SCI1);  
  SerialOutputBytes((char*)&data_message, sizeof(struct MSG_DATA), &SCI1);  
}
