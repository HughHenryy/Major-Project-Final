#ifndef SERVO_H
#define SERVO_H

extern int iterator_counter;

void PWMinitialise(void);

// sets servo in elevation and azimuth
// note: this requires verification and calibration 
void setServoPose(int azimuth, int elevation);


void Init_TC4 (void);

// interrupt used for cycling through the servo positions
__interrupt void TC4_ISR(void);

#endif