

#ifndef _SERIAL_H_
#define _SERIAL_H_
#define BAUD 19200
//#define FOSC 8000000L
#define FOSC 32000000L
#define NINE 0     /* Use 9bit communication? FALSE=8bit */
#define OUTPUT 0
#define INPUT 1
#define DIVIDER ((uint16_t)(FOSC/(16UL * BAUD) -1))
#define HIGH_SPEED 1

#if NINE == 1
#define NINE_BITS 0x40

#else

#define NINE_BITS 0
#endif

#if HIGH_SPEED == 1
#define SPEED 0x4

#else

#define SPEED 0
#endif

void init_comms(void);
void putch(uint8_t);
uint8_t getch(void);
uint8_t getche(void);

#endif

