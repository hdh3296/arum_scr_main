

#ifndef __AD_CONVERSION_H__
#define __AD_CONVERSION_H__

#include    "main.h"



typedef unsigned int uint;
typedef unsigned char uchar;

#define TRUE 1
#define FALSE 0
#define b_conversion_ad DONE
#define ADC_CH_MAX 10
extern uint16_t adc_updated_mv[ADC_CH_MAX];
extern unsigned char adc_bUpdated[ADC_CH_MAX];
extern uint8_t AD_channel;
extern void adc_loop(void);
extern void initAdc(void);
extern void set_input_ad(unsigned int ad);



#endif

