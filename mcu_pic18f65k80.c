
#include <pic18.h>
#include "mcu_pic18f65k80.h"
//configuration
__CONFIG(1,
         (RETEN_OFF & INTOSCSEL_LOW & SOSCSEL_DIG & XINST_OFF & FOSC_HS1 & PLLCFG_ON & FCMEN_OFF & IESO_OFF & 0xffff));
__CONFIG(2, (PWRTEN_ON & BOREN_SBORDIS & BOREN_ON & BORV_1 & BORPWR_LOW & WDTEN_NOSLP & WDTPS_64 & 0xffff));
__CONFIG(3, (CANMX_PORTE & T0CKMX_PORTB & T3CKMX_PORTG & MSSPMSK_MSK7 & MCLRE_ON & 0xffff)); // PORTB digital on RESET
__CONFIG(4, (STVREN_ON & BBSIZ_BB2K & 0xffff));
// DEBUG disabled,
// XINST disabled
// LVP disabled
// STVREN enabled
__CONFIG(5, (CP0_OFF & CP1_OFF & CP2_OFF & CP3_OFF & CPB_OFF & CPD_OFF & 0xffff)); // code unprotect
__CONFIG(6, (WRT0_OFF & WRT1_OFF & WRT2_OFF & WRT3_OFF & WRTC_OFF & WRTB_OFF & WRTD_OFF & 0xffff)); // code unprotect
__CONFIG(7, (EBTR0_OFF & EBTR1_OFF & EBTR2_OFF & EBTR3_OFF & EBTRB_OFF & 0xffff)); // code unprotect
//√ ±‚»≠.
void initMCU(void) {
    ///////////////////////////////////////////////
    // INTCON Register
    GIE = 0; // global interrupt enable
    PEIE = 0; // peripheral interrupt enable
    TMR0IE = 0; // TMR0 overflow interrupt enable
    INT0IE = 0; // external interrupt 0 enable
    RBIE = 0; // RB port change interrupt enable
    TMR0IF = 0; // TMR0 overflow interrupt flag
    INT0IF = 0; // external interrupt 0 flag
    RBIF = 0; // RB port change interrupt flag
    ///////////////////////////////////////////////
    // INTCON2 Register
    RBPU = 1; // port B pull-up enable
    INTEDG0 = 0; // external interrupt 0 edge select
    INTEDG1 = 0; // external interrupt 1 edge select
    INTEDG2 = 0; // external interrupt 2 edge select
    TMR0IP = 1; // TMR0 overflow interrupt priority
    RBIP = 0; // RB port change interrupt priority
    ///////////////////////////////////////////////
    // INTCON3 Register
    INTCON3 = 0;
    /*
      INT2IP=0;	    // external interrupt 2 priority
      INT1IP=0;	    // external interrupt 1 priority
      INT2IE=0;	    // external interrupt 2 enable
      INT1IE=0;	    // external interrupt 1 enable
      INT2IF=0;	    // external interrupt 2 flag
      INT1IF=0;	    // external interrupt 1 flag
    */
    ///////////////////////////////////////////////
    // T0CON Register
    TMR0ON = 1; // TMR0 on/off
    T08BIT = 0; // 8/16 bit select    ..(16bit)
    T0CS = 0; // TMR0 Source Select ..(internal clock)
    T0SE = 0; // TMR0 Source Edge Select
    PSA = 0; // Prescaler Assignment ..(enable)
    T0PS2 = 0; // Prescaler..............(1:2)
    T0PS1 = 0;
    T0PS0 = 1;
    // OSCCON Register
    SCS0 = 0; // system clock switch bit
    SCS1 = 0; // system clock switch bit
    // LVDCON Register
    IRVST = 0; // input reference voltage stable status..(read only)
    HLVDEN = 1; // low voltage detect enable..............(enable)
    HLVDL3 = 1; // low voltage detection limits...........(4.16V - 4.5V)
    HLVDL2 = 1;
    HLVDL1 = 0;
    HLVDL0 = 1;
    // WDTCON Register
    SWDTEN = 0; // software watchdog timer enable.......(disable)
    // RCON Register
    IPEN = 0; // interrupt priority enable............(disable)?????
    RI = 0; // RESET instruction status
    TO = 0; // watchdog timeout flag
    PD = 0; // power-down detection
    POR = 0; // power-on reset status
    BOR = 0; // brown-our reset status
    // T1CON Register
    // 	RD1=1;	// 16 Bit Read/Write Enable
    //T1RD16=1;	// 16 Bit Read/Write Enable
    T1CKPS1 = 1; // Prescaler ........(1/8)
    T1CKPS0 = 1;
    //////// 	T1OSCEN=0;	// Oscillator Enable....(disable)
    ////////  	T1SYNC=0;	    // Sync Selct
    ////////  	TMR1CS=0;	    // TMR Clock Source Select..(internal clock)
    ////////  	TMR1ON=1;	    // TMR on/off...............( tmr run)
    //
    T1CONbits.RD16 = 1; // 16 Bit Read/Write Enable
    T1CONbits.SOSCEN = 1; /////////?????????????????
    nT1SYNC = 0;
    TMR1CS0 = 0;
    TMR1CS1 = 0;
    TMR1ON = 1; // TMR on/off...............( tmr run)
    /*
    static          near bit	T2CKPS0		@ ((unsigned)&T2CON*8)+0;
    static          near bit	T2CKPS1		@ ((unsigned)&T2CON*8)+1;
    static          near bit	TMR2ON		@ ((unsigned)&T2CON*8)+2;
    static          near bit	T2OUTPS0	@ ((unsigned)&T2CON*8)+3;
    static          near bit	T2OUTPS1	@ ((unsigned)&T2CON*8)+4;
    static          near bit	T2OUTPS2	@ ((unsigned)&T2CON*8)+5;
    static          near bit	T2OUTPS3	@ ((unsigned)&T2CON*8)+6;
    */
    // T2CON Register
    T2CON = 0;
    /*
      T2OUTPS3=0;	// Postscale
      T2OUTPS2=0;
      T2OUTPS1=0;
      T2OUTPS0=0;
      TMR2ON=0;	    // TMR2 On/Off............(off)
      T2CKPS1=0;	// Prescale
      T2CKPS0=0;
    */
    // SSPSTAT Register..........(not use)
    SSPSTAT = 0;
    /*
      SMP=0;	    // Sample Bit
      CKE=0;	    // SPI Clk Edge Select
      DA=0;	        // Data/Address Bit
      STOP=0;	    // STOP Bit detected
      START=0;	    // START Bit detected
      RW=0;	        // Read/Write bit Information
      UA=0;	        // Update Adress
      BF=0;	        // Buffer Full Status bit
    */
    // SSPCON1 Register..........(not use)
    WCOL = 0; // write collision detect
    SSPOV = 0; // recieve overflow indicator
    SSPEN = 0; // SSP enable
    CKP = 0; // clock polarity select
    SSPM3 = 0; // SSP contMode select
    SSPM2 = 1;
    SSPM1 = 0;
    SSPM0 = 1;
    // SSPCON2 Register.......(not use)
    SSPCON2 = 0;
    /*
      GCEN=0;	    // general call enable
    //  ACKSTA=0;	    // acknowledge status bit
      ACKDT=0;	    // acknowledge data bit
      ACKEN=0;	    // acknowledge sequence enable
      RCEN=0;	    // recieve enable bit
      PEN=0;	    // STOP condition enable
      RSEN=0;	    // repeated START enable
      SEN=0;	    // START condition enable
    */
    ADCON0 = 0;
    ADCON1 = 0;
    ADCON2 = 0;
    ANCON0 = 0;
    ANCON1 = 0;
    CCP1CON = 0;
    CCP2CON = 0;
    CCP3CON = 0;
    CCP4CON = 0;
    CCP5CON = 0;
    ECCP1AS = 0;
    ECCP1DEL = 0;
    CVRCON = 0;
    /*
    // CMCON Comparator module register.....(not use)
        C2OUT=0;	   // comparator 2 output
        C1OUT=0;	   // comparator 1 output
        C2INV=0;	   // select to invert comp2 output
        C1INV=0;	   // select to invert comp1 output
        CIS=0;	   // comp input switch bit
        CM2=1;	   // comp contMode select bits
        CM1=1;
        CM0=1;
    */
    // T3CON Register.......(not use)
    T3CON = 0;
    /*
      T3RD16=0;	    // 16-Bit Read/Write select
      T3ECCP1=0;	// TMR3 & TMR1 CCPx Enable
      T3CKPS1=0;	// Prescaler
      T3CKPS0=0;
      T3CCP1=0;	    // TMR3 & TMR1 CCPx Enable
      T3SYNC=0;	    // Sync Select
      TMR3CS=0;	    // TMR3 source Select
      TMR3ON=0;	    // TMR3 on/off
    */
    // TXSTA Register
    TXSTA = 0;
    /*
      CSRC=0;      // CLK source select
      TX9=0	;	   // 8/9 bit TX data select
      TXEN=1;	   // transmit enable bit
      SYNC=0;	   // USART contMode select
      BRGH=1;	   // high baud rate select
      TRMT=1;	   // TX shift reg. status bit
      TX9D=1;	   // 9th Bit of TX data
    */
    // RCSTA Register
    RCSTA = 0;
    /*
      SPEN=0;	  // serial port enable...(disable)
      RX9=0;	  // 8/9 bit data reception
      SREN=0;	  // single recieve enable
      CREN=0;	  // continuous recieve enable
      ADDEN=0;	  // address detect enable
      FERR=0;	  // framing error
      OERR=0;	  // overrun error
      RX9D=0;	  // 9th Bit of RX data
    */
    // EECON1 Register
    EEPGD = 0; // FLASH/EEPROM select
    CFGS = 0; // access config regs./access FLASH-EEPROM
    // alternate definition
    CFGS = 0; //Config./Calibration Select
    FREE = 0; // FLASH row erase enable
    WRERR = 0; // write error flag
    WREN = 0; // write enable
    WR = 0; // write control
    RD = 0; // read control
    /*
    // IPR3 Register
      IRXIP=0;	  // CAN invalid rec. message interrupt priority
      WAKIP=0;	  // CANbus activity wake-up interrupt priority
      ERRIP=0;	  // CANbus error interrupt priority
      TXB2IP=0;	  // CAN TX buffer 2 interrupt priority
      TXB1IP=0;	  // CAN TX buffer 1 interrupt priority
      TXB0IP=0;	  // CAN TX buffer 0 interrupt priority
      RXB1IP=0;	  // CAN RX buffer 1 interrupt priority
      RXB0IP=1;	  // CAN RX buffer 0 interrupt priority

    // PIR3 Register
      IRXIF=0;	  // CAN invalid rec. message interrupt flag
      WAKIF=0;	  // CANbus activity wake-up interrupt flag
      ERRIF=0;	  // CANbus error interrupt flag
      TXB2IF=0;	  // CAN TX buffer 2 interrupt flag
      TXB1IF=0;	  // CAN TX buffer 1 interrupt flag
      TXB0IF=0;	  // CAN TX buffer 0 interrupt flag
      RXB1IF=0;	  // CAN RX buffer 1 interrupt flag
      RXB0IF=0;	  // CAN RX buffer 0 interrupt flag

    // PIE3 Register
      IRXIE=0;	  // CAN invalid rec. message interrupt enable
      WAKIE=0;	  // CANbus activity wake-up interrupt enable
      ERRIE=0;	  // CANbus error interrupt enable
      TXB2IE=0;	  // CAN TX buffer 2 interrupt enable
      TXB1IE=0;	  // CAN TX buffer 1 interrupt enable
      TXB0IE=0;	  // CAN TX buffer 0 interrupt enable
      RXB1IE=0;	  // CAN RX buffer 1 interrupt enable
      RXB0IE=0;	  // CAN RX buffer 0 interrupt enable


    // IPR2 Register
      CMIP=0;	     // comparator interrupt priority
      EEIP=0;   	// EEPROM write interrupt priority
      BCLIP=0;   	// bus collision interrupt priority
      HLVDIP=0;	    // low voltage detect interrupt priority
      TMR3IP=0;	    // TMR3 overflow interrupt priority
      ECCP1IP=0;	// ECCP1 interrupt priority


    // PIR2 Register
      CMIF=0;	   // comparator interrupt flag
      EEIF=0;	   // EEPROM write interrupt flag
      BCLIF=0;	   // bus collision interrupt flag
      HLVDIF=0;	   // low voltage detect interrupt flag
      TMR3IF=0;	   // TMR3 overflow interrupt flag
      ECCP1IF=0;   // ECCP1 interrupt flag


    // PIE2 Register
      CMIE=0;	  // comparator interrupt enable
      EEIE=0;	  // EEPROM write interrupt enable
      BCLIE=0; 	  // bus collision interrupt enable
      HLVDIE=0;	  // low voltage detect interrupt enable
      TMR3IE=0;	  // TMR3 overflow interrupt enable
      ECCP1IE=0;  // ECCP1 interrupt enable


    // IPR1 Register
      PSPIP=0;	  // para. slave port rd/wr interrupt priority
      ADIP=0;	  // AD conv. interrupt priority
      RCIP=0;	  // USART RX interrupt priority
      TXIP=0;	  // USART TX interrupt priority
      SSPIP=0;    // master SSP interrupt priority
      CCP1IP=0;	  // CCP1 interrupt priority
      TMR2IP=0;	  // TMR2 - PR2 match interrupt priority
      TMR1IP=0;	  // TMR1 overflow interrupt priority


    // PIR1 Register
      PSPIF=0;	     // para. slave port rd/wr interrupt flag
      ADIF=0;	     // AD conv. interrupt flag
      RCIF=0;	     // USART RX interrupt flag
      TXIF=0;	     // USART TX interrupt flag
      SSPIF=0;	     // master SSP interrupt flag
      CCP1IF=0;	     // CCP1 interrupt flag
      TMR2IF=0;	     // TMR2 - PR2 match interrupt flag
      TMR1IF=0;	     // TMR1 overflow interrupt flag


    // PIE1 Register
      PSPIE=0;	    // para. slave port rd/wr interrupt enable
      ADIE=0;	    // AD conv. interrupt enable
      RCIE=0;	    // USART RX interrupt enable
      TX2_IE=0;	    // USART TX interrupt enable
      SSPIE=0;	    // master SSP interrupt enable
      CCP1IE=0;	    // CCP1 interrupt enable
      TMR2IE=0;	    // TMR2 - PR2 match interrupt enable
      TMR1IE=0;	    // TMR1 overflow interrupt enable
    */
    IPR5 = 0;
    PIR5 = 0;
    PIE5 = 0;
    IPR3 = 0;
    PIR3 = 0;
    PIE3 = 0;
    IPR2 = 0;
    PIR2 = 0;
    PIE2 = 0;
    IPR1 = 0;
    PIR1 = 0;
    PIE1 = 0;
    // TRISE Register
    IBF = 0; // input buffer full status
    OBF = 0; // output buffer full status
    IBOV = 0; // input buffer overflow
    PSPMODE = 0; // parallel slave port contMode select
}


