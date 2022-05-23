#ifndef SERIAL_ENCODER_HEADER
#define SERIAL_ENCODER_HEADER

struct MSG_HEADER{
  int sentinel;
  char msg_type[8];
  unsigned int msg_size;
  unsigned int header_time;
  int end_sentinel;
};

struct MSG_DATA{
  int sentinel;
  int angle;
  unsigned long sample;
  unsigned long base;
  unsigned int last_sample_time;
};

void SendTextMsg(char* text_message);

void SendDataMsg(int *angle_in, unsigned long *sample_in, unsigned long *base_in);

#endif