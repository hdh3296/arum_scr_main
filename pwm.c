
#include 	<pic18.h>
#include 	"pwm.h"



unsigned char *DutyCycleAdr;
unsigned char *CcpCtrlAdr;


void PwmAdrSort(unsigned int pwmx)
{
	switch(pwmx){
		case	1:
			CcpCtrlAdr	= (unsigned char *)&CCP1CON;
			DutyCycleAdr= (unsigned char *)&CCPR1L;
			break;
		case	2:
			CcpCtrlAdr	= (unsigned char *)&CCP2CON;
			DutyCycleAdr= (unsigned char *)&CCPR2L;
			break;
		case	3:
			CcpCtrlAdr	= (unsigned char *)&CCP3CON;
			DutyCycleAdr= (unsigned char *)&CCPR3L;
			break;
		case	4:
			CcpCtrlAdr	= (unsigned char *)&CCP4CON;
			DutyCycleAdr= (unsigned char *)&CCPR4L;
			break;
		case	5:
			CcpCtrlAdr	= (unsigned char *)&CCP5CON;
			DutyCycleAdr= (unsigned char *)&CCPR5L;
			break;
	}
}



void PwmOnx(unsigned int pwmx)
{
	unsigned char buf;

	PwmAdrSort(pwmx);

	buf=*CcpCtrlAdr;
	buf=(buf | 0x0c);
	*CcpCtrlAdr=buf;
}



void PwmOffx(unsigned int pwmx)
{
	unsigned char buf;

	PwmAdrSort(pwmx);

	buf=*CcpCtrlAdr;
	buf=(buf & 0xf0);
	*CcpCtrlAdr=buf;
}


void OutPWMx(unsigned int pwmx)
{
	unsigned int DutyCycleValue;
	unsigned char buf;

	PwmAdrSort(pwmx);


	switch(pwmx){
		case	1:
			DutyCycleValue=DutyCyclex[0];
			break;
		case	2:
			DutyCycleValue=DutyCyclex[1];
			break;
		case	3:
			DutyCycleValue=DutyCyclex[2];
			break;
		case	4:
			DutyCycleValue=DutyCyclex[3];
			break;
		case	5:
			DutyCycleValue=DutyCyclex[4];
			break;
	}


	buf= *CcpCtrlAdr;
	if(DutyCycleValue & 0x01)	buf=(buf | 0x10);
	else						buf=(buf & 0xef);

	if(DutyCycleValue & 0x02)	buf=(buf | 0x20);
	else						buf=(buf & 0xdf);
	*CcpCtrlAdr=buf;

	*DutyCycleAdr = (DutyCycleValue>>2);		//13.04.30
}



void InitPwmx(unsigned int pwmx)
{
	TMR2IE=0;
	TMR2IP=0;
	TMR2IF=0;

	PwmAdrSort(pwmx);

	switch(pwmx){
		case	1:
			TRISD4=0;   // pin_PWM_0
			DutyCyclex[0]=0;
			break;
		case	2:
			TRISC2=0;
			DutyCyclex[1]=0;
			break;
		case	3:
			TRISC6=0;
			DutyCyclex[2]=0;
			break;
		case	4:
			TRISC7=0;
			DutyCyclex[3]=0;
			break;
		case	5:
			TRISB5=0;
			DutyCyclex[4]=0;
			break;
	}

	PwmOnx(pwmx); // PIN_PWM ON

	PR2 = 0xff;
	T2CON = 0x06;		// 가장 안정적이다. 다른 값은 오랜시간 사용시 열이 난다.

	OutPWMx(pwmx); // Q???
}












