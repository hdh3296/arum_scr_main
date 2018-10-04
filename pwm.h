#ifndef __pwm_h__
#define __pwm_h__
#include    "main.h"


#define DUTI_MAXx 0x3ff // 1023


unsigned int	DutyCyclex[5];


extern	void InitPwmx(unsigned int pwmx);
extern  void OutPWMx(unsigned int pwmx);
extern	void PwmOnx(unsigned int pwmx);
extern	void PwmOffx(unsigned int pwmx);


#endif




