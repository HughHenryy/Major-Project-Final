#ifndef SERIAL_ENCODER_HEADER
#define SERIAL_ENCODER_HEADER

// struct od the header message
struct MSG_HEADER{
  int sentinel;
  char msg_type[8];
  unsigned int msg_size;
  unsigned int header_time;
  int end_sentinel;
};

// struct of the data message
struct MSG_DATA{
  int sentinel;
  int angle;
  unsigned long sample;
  unsigned long base;
  unsigned int last_sample_time;
};

// send text message to the serial port
void SendTextMsg(char* text_message);

// send data message to the serial port
void SendDataMsg(int *angle_in, unsigned long *sample_in, unsigned long *base_in);

#endif
