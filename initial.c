
#include <pic18.h>
#include "initial.h"


void init_canPort(void) {
pin_CAN_TX_tris	= 0;
pin_CAN_RX_tris = 1;
pin_CAN_TX      = 0;
pin_CAN_RX 		= 0;

}

void initAnalogPin(void) {
    pin_AN0_ZINC_tris = 1;
    pin_AN0_ZINC          = 0;
    pin_AN1_VR_SET_tris = 1;
    pin_AN1_VR_SET          = 0;
    pin_AN2_CURR_tris = 1;
    pin_AN2_CURR          = 0;
    pin_AN3_VOLT_tris = 1;
    pin_AN3_VOLT          = 0;
    pin_34_X_tris = 1;
    pin_34_X          = 0;

    pin_37_X_tris = 1;
    pin_37_X          = 0;
    pin_38_X_tris = 1;
    pin_38_X          = 0;
    pin_39_X_tris = 1;
    pin_39_X          = 0;
    pin_20_X_tris = 1;
    pin_20_X          = 0;

}

void initPort(void) {
    TRISA = 0xff;
    TRISB = 0xff;
    TRISC = 0xff;
    TRISD = 0xff;
    TRISE = 0xff;
    TRISF = 0xff;
    TRISG = 0xff;
    TRISE = 0x0f;
    TRISB = 0xff;
    LATA = 0xff;
    LATB = 0xff;
    LATC = 0xff;
    LATD = 0xff;
    LATE = 0xff;
    LATF = 0xff;
    LATG = 0xff;

    pin_GATE_R_PH_tris = 0;
    pin_GATE_R_PH = 0;
    pin_GATE_S_PH_tris = 0;
    pin_GATE_S_PH = 0;
    pin_GATE_T_PH_tris = 0;
    pin_GATE_T_PH = 0;


    pin_Rx_tris = 1;
    pin_RX = 0;
    pin_TX_tris = 0;
    pin_TX = 0;

	pin_AUTO_tris = 1;
	pin_AUTO = 0;
	pin_KEY_POWER_tris = 1;
	pin_KEY_POWER = 0;
	pin_KEY_ALARM_tris	= 1;
	pin_KEY_ALARM		= 0;
	pin_50HZ_tris	= 1;
	pin_50HZ			= 0;

    pin_RUN_LED_tris = 0;
    pin_RUN_LED = 0;

	pin_RY_RUN_pin		= 0;
	pin_RY_RUN			= RY_OFF;
	pin_RY_ALARM_pin	= 0;
	pin_RY_ALARM		= RY_OFF;


    initAnalogPin();



}


void initTimer0(void) {
    TMR0IE = 1;
    TMR0IF = 0;
    TMR0ON = 1; // TMR0 on/off
    T08BIT = 0; // 8/16 bit select    ..(16bit)
    T0CS = 0; // TMR0 Source Select ..(internal clock)
    T0SE = 0; // TMR0 Source Edge Select
    PSA = 0; // Prescaler Assignment ..(enable)
    T0PS2 = 0; // Prescaler..............(1:2)
    T0PS1 = 1;
    T0PS0 = 0;
    TMR0L = MSEC_L;
    TMR0H = MSEC_H;
}




void initTimer1(void) {
    TMR1IF = 0;
	TMR1IP = 1;
    TMR1IE = 1;
    TMR1CS0 = 0;
    TMR1CS1 = 0;
	T1CONbits.SOSCEN = 0;	//  SOSC Oscillator Enable bit
	T1CONbits.RD16 = 1;
    T1CKPS1 = 1; // Prescaler ........(1/8)
    T1CKPS0 = 1;

    TMR1L = MSEC_L_1;
    TMR1H = MSEC_H_1;
    TMR1ON = 1; // TMR0 on/off


}

void    disableZero_RST_extInt(void) {
	INT0IF = 0;
	INT0IE = 0;
}




void initInt0(void) {
	INTEDG0 = 1;	// 1 = rising edge
	INT0IF  = 0;
	INT0IE  = 1;
	pin_ZERO_CROSS_R_tris	= 1; // �Է�
}

void initInt1(void) {
	INTEDG1 = 1;	// rising edge
	INT1IF	= 0;
	INT1IE	= 1;
	pin_ZERO_CROSS_S_tris = 1;
}

void initInt2(void) {
	INTEDG2 = 1;	// rising edge
	INT2IF	= 0;
	INT2IE	= 1;
	pin_ZERO_CROSS_T_tris = 1;
}



