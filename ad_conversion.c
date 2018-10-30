
#include    <pic18.h>
#include	"ad_conversion.h"


uint8_t AD_channel;
bool b_ad_interrupted = 0;
unsigned int in_ad_changed = 0;
bool b_ad_calc_enable;
unsigned char ad_calc_wait_count;
unsigned long SumAD = 0;
unsigned int SumCnt = 0;
uint16_t adc_updated_analog_mV[ADC_CH_MAX] = {
    0,
};
unsigned char adc_bUpdated[ADC_CH_MAX] = {
    0,
};


unsigned int bef_ad_channel = 0;


void initAdc(void) {
	// 전압 : 5채널 피드백 (pwm에 대해서)
    TRISA0 = 1; // VRMS_0
    TRISA1 = 1; // 수동 볼륨 0~99%
    TRISA2 = 1;
    TRISA3 = 1;
    TRISA5 = 1; // VRMS_4, AN4

	// 전류  : 5채널 피드백 (pwm에 대해서)
    TRISE0 = 1; // ARMS_0  , AN5
    TRISE1 = 1; // ARMS_1
    TRISE2 = 1; // ARMS_2
    TRISB1 = 1; // ARMS_3
    TRISB4 = 1; // ARMS_4   , AN9

    ADCON0 = 0x01; //ADON = 1;
    ADCON1 = 0x80; //event= timer1
    ADFM = 1; // period is an 8 bit value, right justify to get result from ADRESH
    ADCS2 = 1;
    ADCS1 = 1;
    ADCS0 = 0; // 1Tad = 2 usec >= 1.6usec
    ACQT2 = 1;
    ACQT1 = 0;
    ACQT0 = 0; // acquisition time >= 16~20 usec
    ANCON0 = 0x0f; // an0-an4, dig5~8
    ANCON1 = 0x00; // an9 => dig9
    ADIP = 0;
    ADIF = 0;
    ADIE = 1;
    b_conversion_ad = 1;
}


bool is_ad_calc_enable() {
    ad_calc_wait_count++;

    if (ad_calc_wait_count >= 3) {
        ad_calc_wait_count = 0;
        return TRUE;
    }

    return FALSE;
}

bool is_update_AD() {
    unsigned long AdVal;
    unsigned char i;
    unsigned char sum_count;

    if (!b_ad_interrupted) {
        return FALSE;
    }

    // 변경시 쓰레기 값이 저장되는 문제 때문에 추가 하였다.
    if (!b_ad_calc_enable) {
        b_ad_calc_enable = is_ad_calc_enable();
    } else {
        SumAD = SumAD + (unsigned long) in_ad_changed; // 12비트 AD
        SumCnt++;


		sum_count = 1000;


        if (SumCnt >= sum_count) {
            AdVal = ((SumAD * 1000) / (0xfff / 5)) / SumCnt; // 12비트 5000 mV 기준
            adc_updated_analog_mV[AD_channel] = (unsigned int) AdVal;
            adc_bUpdated[AD_channel] = TRUE;
            SumAD = 0;
            SumCnt = 0;

            return TRUE;
        }
    }

    b_ad_interrupted = FALSE;
    b_conversion_ad = 1;
    return FALSE;
}


uchar get_AD_channel(uchar AdSel) {
    switch (AdSel) {
        case 0:
            AdSel = 1;
            break;

        case 1:
            AdSel = 2;
            break;

        case 2:
            AdSel = 3;
            break;

        case 3:
            AdSel = 0;
            break;
        default:
            AdSel = 0;
            break;
    }

    return AdSel;
}


void set_AD_channel_register(unsigned char AdSel) {
    switch (AdSel) {
        case 0:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 0;
            CHS1 = 0;
            CHS0 = 0;
            break;

        case 1:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 0;
            CHS1 = 0;
            CHS0 = 1;
            break;

        case 2:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 0;
            CHS1 = 1;
            CHS0 = 0;
            break;

        case 3:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 0;
            CHS1 = 1;
            CHS0 = 1;
            break;

        case 4:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 1;
            CHS1 = 0;
            CHS0 = 0;
            break;
        case 5:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 1;
            CHS1 = 0;
            CHS0 = 1;
            break;

        case 6:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 1;
            CHS1 = 1;
            CHS0 = 0;
            break;

        case 7:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 1;
            CHS1 = 1;
            CHS0 = 1;
            break;

        case 8:
            CHS4 = 0;
            CHS3 = 1;
            CHS2 = 0;
            CHS1 = 0;
            CHS0 = 0;
            break;

        case 9:
            CHS4 = 0;
            CHS3 = 1;
            CHS2 = 0;
            CHS1 = 0;
            CHS0 = 1;
            break;

        default:
            CHS4 = 0;
            CHS3 = 0;
            CHS2 = 0;
            CHS1 = 0;
            CHS0 = 0;
            break;
    }
}


void check_changing_of_AD_channel() {
    // AD 채널이 변경 되었다.
    // 변경시 쓰레기 값이 저장되는 문제 때문에 추가 하였다.
    if (AD_channel != bef_ad_channel) {
        b_ad_calc_enable = FALSE;
        ad_calc_wait_count = 0;
        bef_ad_channel = AD_channel;
    }
}


void adc_loop(void) {
    if (!is_update_AD()) {
        return;
    }

    AD_channel = get_AD_channel(AD_channel); // 채널 변경
    set_AD_channel_register(AD_channel); // 채널 설정
    check_changing_of_AD_channel();
    b_ad_interrupted = FALSE;
    b_conversion_ad = 1;
}


void set_input_ad(unsigned int ad) {
    if (!b_ad_interrupted) {
        in_ad_changed = ad;
        b_ad_interrupted = TRUE;
    }
}


