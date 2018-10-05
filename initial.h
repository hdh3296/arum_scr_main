

#ifndef __INITIAL_H__
#define __INITIAL_H__

#include    "main.h"


extern volatile bit RB0x @ ((unsigned) &PORTB * 8) + 0;
extern volatile bit RB1x @ ((unsigned) &PORTB * 8) + 1;

#define MSEC_H 0xfc
#define MSEC_L 0x18

#define MSEC_H_1 0xff
#define MSEC_L_1 214 // 213,4 = 46us


/* ======== ��Ʈ ============  */
#define pin_RUN_LED             RC3
#define pin_RUN_LED_tris     TRISC3

#define pin_GATE_R_PH            RC0 	// SSR_0
#define pin_GATE_R_PH_tris    TRISC0

#define pin_RX RE6		// rx
#define pin_Rx_tris TRISE6
#define pin_TX RE7		// tx
#define pin_TX_tris TRISE7


// PCR main ���� ��Ʈ ����
#define pin_VRMS_0              RA0
#define pin_VRMS_0_tris     TRISA0
#define pin_VRMS_1              RA1
#define pin_VRMS_1_tris     TRISA1
#define pin_VRMS_2              RA2
#define pin_VRMS_2_tris     TRISA2
#define pin_VRMS_3              RA3
#define pin_VRMS_3_tris     TRISA3
#define pin_VRMS_4              RA5
#define pin_VRMS_4_tris     TRISA5

#define pin_ARMS_0              RE0
#define pin_ARMS_0_tris     TRISE0
#define pin_ARMS_1              RE1
#define pin_ARMS_1_tris     TRISE1
#define pin_ARMS_2              RE2
#define pin_ARMS_2_tris     TRISE2


#define pin_ARMS_4              RB4
#define pin_ARMS_4_tris     TRISB4

#define pin_CAN_RX 				RE4
#define pin_CAN_RX_tris 		TRISE4
#define pin_CAN_TX 				RE5
#define pin_CAN_TX_tris			TRISE5

// ZERO_CROSS_R / S / T �� ���� ##
#define pin_ZERO_CROSS_R		RB0x // �ܺ� ���ͷ�Ʈ 0 = INT0
#define pin_ZERO_CROSS_R_tris	TRISB0
#define pin_ZERO_CROSS_S        RB1x
#define pin_ZERO_CROSS_S_tris   TRISB1
#define pin_ZERO_CROSS_T        RB2
#define pin_ZERO_CROSS_T_tris   TRISB2


#define EN_485 0
#define DIS_485 1

extern void initPort(void);
extern void initTimer0(void);
extern void initTimer1(void);
extern void initInt0(void);

#endif

