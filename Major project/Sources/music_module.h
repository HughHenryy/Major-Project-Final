#ifndef MUSIC_HEADER
#define MUSIC_HEADER

// 6 notes and 1 rest in the song
#define A 27272
#define B 30612
#define C 34280
#define D 36363
#define E 45570
#define F 45977
#define R 0


// numbers to display on LED
#define a 0x7D // 01111101 binary = 6
#define b 0x6D // 01101101 binary = 5
#define c 0x66 // 4
#define d 0x4F // 3
#define e 0x5B // 2
#define f 0x06 // 1
#define r 0x3F // 0

extern int note;
extern int play_signal;

void init_music(void);
void booth_music(int *note_p, int *play_p);
void delay(void);
void play_music (int *note_p, int *play_p);

__interrupt void TC6_ISR(void);

#endif