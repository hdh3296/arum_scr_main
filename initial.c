
#include <pic18.h>
#include "initial.h"


void init_canPort(void) {
pin_CAN_TX_tris	= 0;
pin_CAN_RX_tris = 1;
pin_CAN_TX      = 0;
pin_CAN_RX 		= 0;

}

void initAnalogPin(void) {
    pin_VRMS_0_tris = 1;
    pin_VRMS_0          = 0;
    pin_VRMS_1_tris = 1;
    pin_VRMS_1          = 0;
    pin_VRMS_2_tris = 1;
    pin_VRMS_2          = 0;
    pin_VRMS_3_tris = 1;
    pin_VRMS_3          = 0;
    pin_VRMS_4_tris = 1;
    pin_VRMS_4          = 0;

    pin_ARMS_0_tris = 1;
    pin_ARMS_0          = 0;
    pin_ARMS_1_tris = 1;
    pin_ARMS_1          = 0;
    pin_ARMS_2_tris = 1;
    pin_ARMS_2          = 0;
    pin_ARMS_3_tris = 1;
    pin_ARMS_3          = 0;
    pin_ARMS_4_tris = 1;
    pin_ARMS_4          = 0;
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
    pin_GATE_R_PH = 0; // if 0 = heater off

    pin_RUN_LED_tris = 0;
    pin_RUN_LED = 0;

    pin_Rx_tris = 1;
    pin_RX = 0;
    pin_TX_tris = 0;
    pin_TX = 0;


    initAnalogPin();
    init_canPort();


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
    TMR1IE = 1;
    TMR1IF = 0;
    TMR1ON = 1; // TMR0 on/off
    //T18BIT = 0; // 8/16 bit select    ..(16bit)
    //T1CS = 0; // TMR0 Source Select ..(internal clock)
    //T1SE = 0; // TMR0 Source Edge Select
    //PSA = 0; // Prescaler Assignment ..(enable)
    //T1PS2 = 0; // Prescaler..............(1:2)
    //T1PS1 = 1;
    //T1PS0 = 0;
    TMR1L = MSEC_L_1;
    TMR1H = MSEC_H_1;


}



