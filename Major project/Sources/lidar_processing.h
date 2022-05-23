#ifndef LIDAR_HEADER
#define LIDAR_HEADER

#define NUM_OF_BOOTH 3
// set up the 2 stage of the booth
enum{
	VACANCY,
	OCCUPIED
};
// set up extern variables
extern unsigned long booth_distance[NUM_OF_BOOTH];
extern int boothNum;
extern int start_laser;
extern int usage_mode;

// this function will print out the message
void message (int *angle_p, unsigned long *sample, int *c_status, int *p_status, int *music_p, int *play_p, unsigned long *booth_dis, int *booth_num);

// this function acts as a filter for lidar value
void sample_filter (int *angle_p, unsigned long *sample, unsigned long *base, unsigned long *filted, int *count, int *start_signal, int *start_laser, int *music_p, int *play_p, unsigned long *booth_dis, int *booth_num);

#endif
