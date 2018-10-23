

#ifndef __INITIAL_H__
#define __INITIAL_H__

#include    "main.h"


extern volatile bit RB0x @ ((unsigned) &PORTB * 8) + 0;
extern volatile bit RB1x @ ((unsigned) &PORTB * 8) + 1;

#define MSEC_H 0xfc
#define MSEC_L 0x18

#define MSEC_H_1 0xff
#define MSEC_L_1 214 // 213,4 = 46us


/* ======== 마이컴 MCU IO 포트 정의 ============  */
#define pin_RUN_LED         RC3
#define pin_RUN_LED_tris    TRISC3

#define pin_RX 				RE6		// rx
#define pin_Rx_tris 		TRISE6
#define pin_TX 				RE7		// tx
#define pin_TX_tris 		TRISE7

#define pin_AN0_ZINC          RA0
#define pin_AN0_ZINC_tris     TRISA0
#define pin_AN1_VR_SET        RA1
#define pin_AN1_VR_SET_tris   TRISA1
#define pin_AN2_CURR          RA2
#define pin_AN2_CURR_tris     TRISA2
#define pin_AN3_VOLT          RA3
#define pin_AN3_VOLT_tris     TRISA3

#define pin_34_X          RA5 // xx
#define pin_34_X_tris     TRISA5

#define pin_37_X          RE0
#define pin_37_X_tris     TRISE0
#define pin_38_X          RE1
#define pin_38_X_tris     TRISE1
#define pin_39_X          RE2
#define pin_39_X_tris     TRISE2

#define pin_20_X          RB4
#define pin_20_X_tris     TRISB4

#define pin_CAN_RX 			RE4
#define pin_CAN_RX_tris 	TRISE4
#define pin_CAN_TX 			RE5
#define pin_CAN_TX_tris		TRISE5

// ZERO_CROSS_R / S / T 핀 설정
#define pin_ZERO_CROSS_R		RB0x // 외부 인터럽트 0 = INT0
#define pin_ZERO_CROSS_R_tris	TRISB0
#define pin_ZERO_CROSS_S        RB1x
#define pin_ZERO_CROSS_S_tris   TRISB1
#define pin_ZERO_CROSS_T        RB2
#define pin_ZERO_CROSS_T_tris   TRISB2

#define pin_GATE_R_PH           RC0 	// SSR_0
#define pin_GATE_R_PH_tris    	TRISC0
#define pin_GATE_S_PH           RC1 	// SSR_0
#define pin_GATE_S_PH_tris    	TRISC1
#define pin_GATE_T_PH           RC2 	// SSR_0
#define pin_GATE_T_PH_tris    	TRISC2

#define pin_AUTO			RD0
#define pin_AUTO_tris		TRISD0
#define pin_KEY_POWER		RD1
#define pin_KEY_POWER_tris  TRISD1
#define pin_KEY_ALARM		RD2
#define pin_KEY_ALARM_tris	TRISD2
#define pin_50_60HZ			RD3
#define pin_50_60HZ_tris	TRISD3


extern void initPort(void);
extern void initTimer0(void);
extern void initTimer1(void);
extern void initInt0(void);
extern void initInt1(void);
extern void initInt2(void);
extern void init_canPort(void);

#endif

