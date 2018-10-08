
#include <pic18.h>
#include	"main.h"
#include	"mcu_pic18f65k80.h"
#include	"initial.h"
#include	"ad_conversion.h"
#include	"system\system.h"
#include	"loader_45k80\loader_45k80_main.h"
#include    "pwm.h"
#include    "can\my_can.h"

#define MAX_CH 5

uint8_t rx_db_enableSet[MAX_CH];
uint16_t dutycycleCompared[MAX_CH];

uint16_t not_rx_temp_chkTimer; // 온도 보드 통신 두절 여부 체크 타이머 변수 09-07
bool b_temp_comm_not;
uint16_t not_rx_pannel_chkTimer; // 판넬 보드 통신 두절 여부 체크 타이머 변수 09-07
bool b_pannel_comm_not;
bool bef_b_pannel_comm_not;
bool b_pannel_comm_break_flag; // 판넬 통신이 딱 두절 되는 순간 !! => 전원을 off한거와 같은 효과
bool b_pannel_comm_break_flag_ch[5] = {0,};



enum {
	H_OFF_STATE = 0,
	H_ON_STATE  = 1,
};
bool stateHeaterOnState[5] = {H_OFF_STATE, };


enum {
	CH_SET_NONE = 0,
	CH_SET_DIS  = 1,
	CH_SET_EN   = 2,
};


typedef struct {
    unsigned heater_onoff : 1;
    unsigned heaterled_onoff : 1;
    unsigned errorled_onoff : 1;
    unsigned b_turned_on_power : 1;
} HgcPt1000;
HgcPt1000 pt1000[4];

enum {
	NONE = 0,
	RX_FROM_LOADER,
	RX_TEMP_INFO,
	RX_PANNEL_INFO,
};


Heater heater[MAX_CH];


uint16_t rxGoodSucessTimer;



enum {
    HEATER_NONE,
    HEATER_ON,
    HEATER_OFF
};



bool bDoAdcSpeed[MAX_CH];




void initPwm(void) {

    InitPwmx(1);
    InitPwmx(2);
    InitPwmx(3);
    InitPwmx(4);
    InitPwmx(5);
}

void offPwmAll(void) {
    DutyCyclex[0]	= 0;
    DutyCyclex[1]	= 0;
    DutyCyclex[2]	= 0;
    DutyCyclex[3]	= 0;
    DutyCyclex[4]	= 0;

    OutPWMx(1);
    OutPWMx(2);
    OutPWMx(3);
    OutPWMx(4);
    OutPWMx(5);
}


volatile uint16_t teat_correct;
uint16_t getCurTempTo(uint16_t now) {
/*
	아날로그 입력값 대비 온도 값을 리턴해준다.
	입력 		: 현재 아날로그 입력 값
	return  : 유저 실제 온도 값 ex) 30 도
*/
	if (now < analogTempList[1]) { // 1200
		return 0;
	} else if (now < analogTempList[2]) { //  1300
		return 1;
	} else if (now < analogTempList[3]) {
		return 2;
	} else if (now < analogTempList[4]) {
		return 3;
	} else if (now < analogTempList[5]) {
		return 4;
	} else if (now < analogTempList[6]) {
		return 5;
	} else if (now < analogTempList[7]) {
		return 6;
	} else if (now < analogTempList[8]) {
		return 7;
	} else if (now < analogTempList[9]) {
		return 8;
	} else if (now < analogTempList[10]) {
		return 9;
	} else if (now < analogTempList[11]) { // 1100
		return 10;
	} else if (now < analogTempList[12]) { // 1200
		return 11;
	} else if (now < analogTempList[13]) { //  1300
		return 12;
	} else if (now < analogTempList[14]) {
		return 13;
	} else if (now < analogTempList[15]) {
		return 14;
	} else if (now < analogTempList[16]) {
		return 15;
	} else if (now < analogTempList[17]) {
		return 16;
	} else if (now < analogTempList[18]) {
		return 17;
	} else if (now < analogTempList[19]) {
		return 18;
	} else if (now < analogTempList[20]) {
		return 19;
	} else if (now < analogTempList[21]) {
		return 20;
	} else if (now < analogTempList[22]) {
		return 21;
	} else if (now < analogTempList[23]) {
		return 22;
	} else if (now < analogTempList[24]) {
		return 23;
	} else if (now < analogTempList[25]) {
		return 24;
	} else if (now < analogTempList[26]) {
		return 25;
	} else if (now < analogTempList[27]) {
		return 26;
	} else if (now < analogTempList[28]) { // <----
		return 27;
	} else if (now < analogTempList[29]) {
		return 28;
	} else if (now < analogTempList[30]) {
		return 29;
	} else if (now < analogTempList[31]) {
		return 30;
	} else if (now < analogTempList[32]) {
		return 31;
	} else if (now < analogTempList[33]) {
		return 32;
	} else if (now < analogTempList[34]) {
		return 33;
	} else if (now < analogTempList[35]) {
		return 34;
	} else if (now < analogTempList[36]) {
		return 35;
	} else if (now < analogTempList[37]) {
		return 36;
	} else if (now < analogTempList[38]) {
		return 37;
	} else if (now < analogTempList[39]) {
		return 38;
	} else if (now < analogTempList[40]) {
		return 39;
	} else if (now < analogTempList[41]) {
		return 40;
	} else if (now < analogTempList[42]) {
		return 41;
	} else if (now < analogTempList[43]) {
		return 42;
	} else if (now < analogTempList[44]) {
		return 43;
	} else if (now < analogTempList[45]) {
		return 44;
	} else if (now < analogTempList[46]) {
		return 45;
	} else if (now < analogTempList[47]) {
		return 46;
	} else if (now < analogTempList[48]) {
		return 47;
	} else if (now < analogTempList[49]) {
		return 48;
	} else if (now < analogTempList[50]) {
		return 49;
	} else if (now < analogTempList[51]) {
		return 50;
	} else if (now < analogTempList[52]) {
		return 51;
	} else if (now < analogTempList[53]) {
		return 52;
	} else if (now < analogTempList[54]) {
		return 53;
	} else if (now < analogTempList[55]) {
		return 54;
	} else if (now < analogTempList[56]) {
		return 55;
	} else if (now < analogTempList[57]) {
		return 56;
	} else if (now < analogTempList[58]) {
		return 57;
	} else if (now < analogTempList[59]) {
		return 58;
	}
	return 59;
}


uint16_t getAnalogTo(uint16_t temp) {
/*
	유저가 셋팅한 온도 값에 대해서 마이컴에 아날로그 값으로 돌려준다.

	return : 아날로그 (온도)
*/
	if (temp < 1) {
		return analogTempList[0]; // <= 0 ~ 10 도
	} else if (temp < 2) {
		return analogTempList[1];
	} else if (temp < 3) {
		return analogTempList[2];
	} else if (temp < 4) {
		return analogTempList[3];
	} else if (temp < 5) {
		return analogTempList[4];
	} else if (temp < 6) {
		return analogTempList[5];
	} else if (temp < 7) {
		return analogTempList[6];
	} else if (temp < 8) {
		return analogTempList[7];
	} else if (temp < 9) {
		return analogTempList[8];
	} else if (temp < 10) {
		return analogTempList[9];
	} else if (temp < 11) {
		return analogTempList[10]; // <= 0 ~ 10 도
	} else if (temp < 12) {
		return analogTempList[11];
	} else if (temp < 13) {
		return analogTempList[12];
	} else if (temp < 14) {
		return analogTempList[13];
	} else if (temp < 15) {
		return analogTempList[14];
	} else if (temp < 16) {
		return analogTempList[15];
	} else if (temp < 17) {
		return analogTempList[16];
	} else if (temp < 18) {
		return analogTempList[17];
	} else if (temp < 19) {
		return analogTempList[18];
	} else if (temp < 20) {
		return analogTempList[19];
	} else if (temp < 21) {
		return analogTempList[20];
	} else if (temp < 22) {
		return analogTempList[21];
	} else if (temp < 23) {
		return analogTempList[22];
	} else if (temp < 24) {
		return analogTempList[23];
	} else if (temp < 25) {
		return analogTempList[24];
	} else if (temp < 26) {
		return analogTempList[25];
	} else if (temp < 27) {
		return analogTempList[26];
	} else if (temp < 28) {
		return analogTempList[27];
	} else if (temp < 29) {
		return analogTempList[28];
	} else if (temp < 30) {
		return analogTempList[29];
	} else if (temp < 31) {
		return analogTempList[30];
	} else if (temp < 32) {
		return analogTempList[31];
	} else if (temp < 33) {
		return analogTempList[32];
	} else if (temp < 34) {
		return analogTempList[33];
	} else if (temp < 35) {
		return analogTempList[34];
	} else if (temp < 36) {
		return analogTempList[35];
	} else if (temp < 37) {
		return analogTempList[36];
	} else if (temp < 38) {
		return analogTempList[37];
	} else if (temp < 39) {
		return analogTempList[38];
	} else if (temp < 40) {
		return analogTempList[39];
	} else if (temp < 41) {
		return analogTempList[40];
	} else if (temp < 42) {
		return analogTempList[41];
	} else if (temp < 43) {
		return analogTempList[42];
	} else if (temp < 44) {
		return analogTempList[43];
	} else if (temp < 45) {
		return analogTempList[44];
	} else if (temp < 46) {
		return analogTempList[45];
	} else if (temp < 47) {
		return analogTempList[46];
	} else if (temp < 48) {
		return analogTempList[47];
	} else if (temp < 49) {
		return analogTempList[48];
	} else if (temp < 50) {
		return analogTempList[49];
	} else if (temp < 51) {
		return analogTempList[50];
	} else if (temp < 52) {
		return analogTempList[51];
	} else if (temp < 53) {
		return analogTempList[52];
	} else if (temp < 54) {
		return analogTempList[53];
	} else if (temp < 55) {
		return analogTempList[54];
	} else if (temp < 56) {
		return analogTempList[55];
	} else if (temp < 57) {
		return analogTempList[56];
	} else if (temp < 58) {
		return analogTempList[57];
	} else if (temp < 59) {
		return analogTempList[58];
	} else if (temp < 60) {
		return analogTempList[59];
	}
	return 4999;
}

#define ASCTOHEX(x) ((x <= '9') ? (x - '0') : (x - '7'))

uint16_t getTempAnalog(uint8_t first, uint8_t second) {
    uint16_t hi = first << 8;
    uint16_t low = second;

    return (hi | low);
}


uint8_t getHexTo(uint8_t first, uint8_t second) {
    uint8_t i, j;

    i = ASCTOHEX(first);
    i = ((i << 4) & 0xf0);
    j = ASCTOHEX(second);
    return (i | j);
}



void temp_proc(void) {

    uint16_t i, j;
    j = 0;
    for (i=0; i<5; i++) {
		// 서브로 부터 온도 아날로그 저장
        heater[i].db_nowTemp_mV
			= getTempAnalog(getHexTo(rxGoodBuffer[10+j], rxGoodBuffer[11+j]),
                           	getHexTo(rxGoodBuffer[12+j], rxGoodBuffer[13+j]));
        j += 10;
    }
}


uint16_t temp_getAnalogTo(uint16_t temp, uint8_t ch) {
	uint16_t corrT =  getCorrentSetT(ch);
	if (positiveCorrT(ch)) {
		if (temp > corrT) {
			temp = temp - corrT;
		}
	} else {
		temp = temp + getCorrentSetT(ch);
	}

    return getAnalogTo(temp);
}


void outputFinalPwmTo(uint8_t ch, uint16_t dutycycle) {
    DutyCyclex[ch] = dutycycle;
    OutPWMx(ch + 1);
}



void setOnOffNoneByTemp(uint8_t ch) {
/*
	온도 값에 따라
	on / off / noen
*/
    uint16_t off_mv = heater[ch].db_setOffTemp_mV;
    uint16_t on_mv  = heater[ch].db_setOnTemp_mV;
    uint16_t now_mv = heater[ch].db_nowTemp_mV;

    // pwm on
    if (now_mv < on_mv) {
        heater[ch].db_heaterCmd_on_off_none = HEATER_ON;
		return;
    }
	// pwm off
    if (now_mv >= off_mv) {
        heater[ch].db_heaterCmd_on_off_none = HEATER_OFF;
		return;
    }
}

void setOnOffNoneByTemp_whenFirstEnable(uint8_t ch) {
    uint16_t high_mv = heater[ch].db_setOffTemp_mV;
    uint16_t low_mv  = heater[ch].db_setOnTemp_mV;
    uint16_t now_mv  = heater[ch].db_nowTemp_mV;

	// frist enable !! 시점에
	if (stateHeaterOnState[ch] == H_OFF_STATE) {
		if (now_mv < high_mv) {

			heater[ch].db_heaterCmd_on_off_none = HEATER_ON;
			return;
		}
	}

}



void heater_setOnOffNone(uint8_t ch) {

    setOnOffNoneByTemp(ch);
}


bool isPnannel_comm_good(void) {
	return (b_pannel_comm_not == FALSE);
}

// pwm enable/disable 정보는 어디??
uint8_t whatChEnDisNone_byPannel(uint8_t ch) {
	if (b_pannel_comm_break_flag_ch[ch]) {
		b_pannel_comm_break_flag_ch[ch] = 0;
		return CH_SET_DIS;
	}
	// 이건, 통신이 될때만 의미가 있다.
	if (isPnannel_comm_good()) {
		// 통신이 잘 될 때
	    if (rx_db_enableSet[ch] & 0x80) { // <-- @ㅁ 임시로 판넬 pwm enable switch
	        return CH_SET_EN;
	    } else if (rx_db_enableSet[ch] & 0x40) {
			return CH_SET_DIS;
	    }
	}

    return CH_SET_NONE;
}


uint8_t getTempShortBreakError(uint16_t temp_mv) {
    if ((temp_mv >= 4900) || b_temp_comm_not) // 99도
        return TERR_BREAK;
    if (temp_mv <= 100)
        return TERR_SHORT;

	return TERR_NONE;
}

void heater_setTempError(uint8_t ch) {
/*
    온도 에러 단선 또는 숏트
*/
    heater[ch].db_tempShortBreak = getTempShortBreakError(heater[ch].db_nowTemp_mV);
}


bool can_isTxEnd(void) {
    return CanTxDataTxPointer >= Can1TxDataTotalCnt;
}


bool can_isTxDisable(void) {
    return (CanTxDataTxPointer >= Can1TxDataTotalCnt) || (bCanTxAct == FALSE);
}

void can_tx_loop(void) {
    uint16_t i;

    if (can_isTxDisable()) return;

    CanTx0();

}



void can_txReady(void) {
    uint16_t i;

    for (i=0; i<MAX_SAVE_BUF; i++) {
        can_txBuffer[i] = get485TxBuffer(i);
    }
    bCanTxAct = 1;
    Can1TxDataTotalCnt = totalTxCnt;
    CanTxDataTxPointer = 0;
}


void updateRxGoodBuffer(uint8_t in_buf[]) {
    uint16_t i;

    for (i=0; i<RX_BUF_MAX; i++) {
        rxGoodBuffer[i] = in_buf[i];
    }
}


bool can_rxGoodSucess(void) {
    if (can_bRxGood) {
        can_bRxGood =0;
        if (rxGoodSucessTimer > RX_GOOD_SUCESS_T) {
            rxGoodSucessTimer = 0;
            return 1;
        }
    }
    return 0;
}




uint8_t getRxProtocolWhat(uint8_t rxBuf[]) {
/*
    rx 데이터가 무슨 데이터인지 확인 용
*/
    // loader
    if (rxBuf[1] == 'A') {
        return RX_FROM_LOADER;
    }
    if ((rxBuf[1] == 'w') && (rxBuf[2] == 'F')) {
         return RX_FROM_LOADER;
    }
    if ((rxBuf[1] == 'r') && (rxBuf[2] == 'F')) {
        return RX_FROM_LOADER;
    }


    // 온도
    if ((rxBuf[1] == 'T') && (rxBuf[2] == 'S')) {
         return RX_TEMP_INFO;
    }

    // 판넬
    if ((rxBuf[1] == 'P') && (rxBuf[2] == 'S')) {
        return RX_PANNEL_INFO;
    }


    // 그 외
    return NONE;

}


void udtLdrProtocol(uint8_t buf[]) {
    uint16_t i;

    for (i=0; i<RX_BUF_MAX; i++) {
        rxGoodBuffer[i] = buf[i];
    }
}

uint8_t  rxWhatProtocol() {

    // uart rx good !
    if (uart_RxGoodSucess()) {
        updateRxGoodBuffer(_uartRxBuffer);
        return getRxProtocolWhat(_uartRxBuffer);
    }

    // can rx chk, when rx good !
    if (can_rxGoodSucess()) {
        updateRxGoodBuffer(_can_rxBuffer);
        return getRxProtocolWhat(_can_rxBuffer);
    }

	return NONE;
}


void pannel_saveRxData(void) {
    uint16_t i, j;
    j = 0;
    for (i=0; i<5; i++) {
        rx_db_enableSet[i] = getHexTo(rxGoodBuffer[16+j], rxGoodBuffer[17+j]);
        j += 10;
    }
}


void loop_rx_proc(void) {

    switch (rxWhatProtocol()) {
        case RX_FROM_LOADER:
            ldr_chkRxKey();
            break;
        case RX_TEMP_INFO:
			not_rx_temp_chkTimer = 0;
            temp_proc();
            break;
        case RX_PANNEL_INFO:
			not_rx_pannel_chkTimer = 0;
            pannel_saveRxData();
            break;
        default:
            break;
    }
}

void loop_tx_proc(void) {
    if (uart_isIdelState()) {
        procTxComunication();
    }
}


void rxtx_loop(void) {

    loop_rx_proc();

    loop_tx_proc();
}



void outPwm_ch(uint8_t ch, uint16_t ducycycle) {
    DutyCyclex[1]	= ducycycle; // ch1 (0~1)
    OutPWMx(ch+1); // ch1 (1~5)
}


void heater_setNowInVoltage_mV(Heater pcr[], uint8_t ch) {
    if (adc_bUpdated[ch]) {
		// adconversion 이 완료 되면 해당 값 가져온다.
        pcr[ch].adc_nowAnalog_mV = adc_updated_mv[ch];
        pcr[ch].adc_bUdted_mV = adc_bUpdated[ch];
        adc_bUpdated[ch] = FALSE;
    }
}

void heater_setNowInCurrent_mA(Heater pcr[], uint8_t ch) {
    if (adc_bUpdated[ch + 5]) {
        pcr[ch].anlog_nowAmp_mV = adc_updated_mv[ch + 5];
        pcr[ch].adc_bUdted_mA = adc_bUpdated[ch + 5];
        adc_bUpdated[ch + 5] = FALSE;
    }
}


void setPwmOut(uint8_t ch, uint16_t ducycycle) {
    outPwm_ch(ch, ducycycle); //
}

/////////////

uint16_t getNowGap(uint16_t now, uint16_t goal) {
	uint16_t gap;
	if (now < goal) {
		return  (goal - now);
	}
	return (now - goal);
}

bool get_bDoAdcSpeed(uint8_t ch, uint16_t now, uint16_t goal, uint16_t gijun) {
	uint16_t nowGap = getNowGap(now, goal);
	if (nowGap > gijun) { // 100mV = > 10V, 1A
		return 1;
	}
	return 0;
}


#define DUTI_MAX 0x3ff // 1023
uint16_t increaseDuty(uint16_t duty, uint8_t ch) {
    if (duty < DUTI_MAX) {
		if (bDoAdcSpeed[ch]) {
			duty += 5;
		} else {
	        duty++;
		}
    }
    // 예외 사항
    if (duty > DUTI_MAX)	duty = DUTI_MAX;

    return duty;
}
uint16_t decreaseDuty(uint16_t duty, uint8_t ch) {
    if (duty > 0) {
		if (bDoAdcSpeed[ch]) {
			duty -= 5;
		} else {
	        duty--;
		}
    }
    // 예외 사항
    if (duty > DUTI_MAX)	duty = 0;

    return duty;
}


bool isOverCurrent(uint8_t ch) {
    uint16_t goal_current = heater[ch].analog_goalSetCurrent_mV;
    uint16_t now_current  = heater[ch].anlog_nowAmp_mV;

    if (now_current > goal_current) {
        return 1;
    }
    return 0;
}
bool isOverVoltage(uint8_t ch) {
    uint16_t goalVoltage = heater[ch].analog_goalSetVoltage_mV;
    uint16_t nowVoltage  = heater[ch].adc_nowAnalog_mV;

    if (nowVoltage > goalVoltage) {
        return 1;
    }
    return 0;
}


// 전류
uint16_t getDutyByGoalCurrent(uint8_t ch, uint16_t duty) {
    uint16_t goal = heater[ch].analog_goalSetCurrent_mV;
    uint16_t now  = heater[ch].anlog_nowAmp_mV;

	bDoAdcSpeed[ch] = get_bDoAdcSpeed(ch, now, goal, 10); // 전류 = 1A
    // current
    if (now < goal) {
        return increaseDuty(duty, ch);
    } else if (now > goal) {
        return decreaseDuty(duty, ch);
    }
    return duty; // goal == now
}

// 전압
uint16_t getDutyByGoalVoltage_ch1(uint8_t ch, uint16_t duty) {
    uint16_t goal = heater[ch].analog_goalSetVoltage_mV;
    uint16_t now =  heater[ch].adc_nowAnalog_mV;

	bDoAdcSpeed[ch] = get_bDoAdcSpeed(ch, now, goal, 100); // 전압 = 10V

    // voltage
    if (now < goal) {
		return increaseDuty(duty, ch);
    } else if (now > goal) {
		return decreaseDuty(duty, ch);
    }
    return duty; // goal == now
}

uint16_t getUserSetGoalVolt(uint8_t ch) {
	switch (ch) {
        case 0:
            return iF_ch0_goalVoltage;
        case 1:
            return iF_ch1_goalVoltage;
        case 2:
            return iF_ch2_goalVoltage;
        case 3:
            return iF_ch3_goalVoltage;
        case 4:
            return iF_ch4_goalVoltage;
	}
	return 0;
}
uint16_t getUserSetGoalAmp(uint8_t ch) {
	switch (ch) {
        case 0:
            return iF_ch0_goalCurrent;
        case 1:
            return iF_ch1_goalCurrent;
        case 2:
            return iF_ch2_goalCurrent;
        case 3:
            return iF_ch3_goalCurrent;
        case 4:
            return iF_ch4_goalCurrent;
	}
	return 0;
}

uint16_t correctedFinalGoalAnalogVolt_mv(uint8_t ch) {
	uint16_t normal_set_analog = heater[ch].goalSetVoltage_V * 10;
	uint16_t correctSet = heater[ch].userCorrVoltSet;
	uint16_t correctValue_analog_mv, result;
	correctValue_analog_mv = (correctSet % 100) * 10;

	if (correctSet >= SIGN_POSIT_V_OR_A) {
		// + 기호
		if (normal_set_analog > correctValue_analog_mv) {
			result = normal_set_analog - correctValue_analog_mv;
			return result;
		}
	} else {
		// - 기호
			result = normal_set_analog + correctValue_analog_mv;
			return result;
	}
	return 0;
}

uint16_t correctedFinalGoalAnalogAmp_mv(uint8_t ch) {
	uint16_t normal_set_analog = heater[ch].goalSetAmp_100mA * 10;
	uint16_t correctSet = heater[ch].userCorrAmpSet;
	uint16_t correctValue_analog_mv, result;
	correctValue_analog_mv = (correctSet % 100) * 10;

	if (correctSet >= SIGN_POSIT_V_OR_A) {
		// + 기호
		if (normal_set_analog > correctValue_analog_mv) {
			result = normal_set_analog - correctValue_analog_mv;
			return result;
		}
	} else {
		// - 기호
			result = normal_set_analog + correctValue_analog_mv;
			return result;
	}
	return 0;
}



uint16_t heater_getAnalogGoalSetVoltage_mv(uint8_t ch) {
	uint16_t nomal;
    switch (ch) {
        case 0:
			return correctedFinalGoalAnalogVolt_mv(0);
        case 1:
            return correctedFinalGoalAnalogVolt_mv(1);
        case 2:
            return correctedFinalGoalAnalogVolt_mv(2);
        case 3:
            return correctedFinalGoalAnalogVolt_mv(3);
        case 4:
            return correctedFinalGoalAnalogVolt_mv(4);
    }
    return 0;

}

uint16_t heater_getAnalogGoalAmp_mv(uint8_t ch) {

    switch (ch) {
        case 0:
            return correctedFinalGoalAnalogAmp_mv(0);
        case 1:
            return correctedFinalGoalAnalogAmp_mv(1);
        case 2:
            return correctedFinalGoalAnalogAmp_mv(2);
        case 3:
            return correctedFinalGoalAnalogAmp_mv(3);
        case 4:
            return correctedFinalGoalAnalogAmp_mv(4);
    }
    return 0;
}

bool isUpdated_mA(uint8_t ch) {
    if (heater[ch].adc_bUdted_mA) {
        heater[ch].adc_bUdted_mA = FALSE;
        return 1;
    }
    return 0;
}

void controlPwmByCurrent(uint8_t ch) {
    if (isUpdated_mA(ch)) {
        dutycycleCompared[ch] = getDutyByGoalCurrent(ch, heater[ch].db_finaldutycycle);
    }
}

void controlPwmByVoltage(uint8_t ch) {
    if (heater[ch].adc_bUdted_mV) {
        heater[ch].adc_bUdted_mV = FALSE;

        dutycycleCompared[ch] = getDutyByGoalVoltage_ch1(ch, heater[ch].db_finaldutycycle);
    }
}

enum {
	CONT_VOLT,
	CONT_CURR
};


uint8_t contMode[MAX_CH] = {CONT_VOLT, };

void setContMode(uint8_t ch) {

	switch (contMode[ch]) {
		case CONT_VOLT:
			if (isOverCurrent(ch)) {
				contMode[ch] = CONT_CURR;
			}
			return;
		case CONT_CURR:
			if (isOverVoltage(ch)) {
				contMode[ch] = CONT_VOLT;
			}
			return;
	}
}

void onoffOuputAllPwmBy(uint8_t ch) {
	setContMode(ch);
	switch (contMode[ch]) {
		case CONT_VOLT:
			// 전류 제어
			controlPwmByVoltage(ch);
			return;
		case CONT_CURR:
			// 전압 제어
			controlPwmByCurrent(ch);
			return;
	}
}

void compareNowVoltageCurrent(uint8_t ch) {
	onoffOuputAllPwmBy(ch);
}


enum {
    ERR_NONE  = 0,
    ERR_BREAK = 1,
    ERR_SHORT = 2,
    ERR_NOT_HEATING = 3,
    ERR_ETC,
} ErrorCode;

bool isPwmOnState(uint8_t ch) {
	return heater[ch].db_heatingOnState;
}

uint16_t inputV_EmptyErrorChktiemr[5];
uint16_t inputA_EmptyErrorChktiemr[5];

bool isInputEmpty(uint8_t ch) {

	if (inputV_EmptyErrorChktiemr[ch] > 10000) {
		return 1;
	}

	if (inputA_EmptyErrorChktiemr[ch] > 10000) {
		return 1;
	}

	return 0;
}

bool isPwm_inVolt_inCurr_not_error(uint8_t ch) {
// pwm on 상태 이지만,
// 입력 값이 아무것도 안들어 올 경우
	if (isPwmOnState(ch)) {
		if (isInputEmpty(ch)) {
			return 1;
		}
	}
	return 0;
}

uint8_t heater_chkEtcError(uint8_t ch) {
// 에러 정보 리턴
    // 에러 체크
	if (heater[ch].db_tempShortBreak == 1) { // 단선
        return ERR_BREAK;
    } else if (heater[ch].db_tempShortBreak == 2) { // 숏트
        return ERR_SHORT;
    }
    return ERR_NONE;
}





bool heater_getOnState(uint16_t dutycycle) {

    if (dutycycle > 0) {
        return 1;
    }

    return FALSE;
}



bool isOnCondition(uint8_t ch) {


	if (heater[ch].db_heaterCmd_on_off_none == HEATER_ON) {
		return 1;
	}

	return 0;
}



// @ㅁ 다시 코딩 해야 한다.
uint16_t getFianlDutycycle(uint16_t ch) {
    uint16_t i;

	if (heater[ch].db_errorCode) {
		return 0;
	}


    if (heater[ch].db_bChUse == FALSE) {
		stateHeaterOnState[ch] = H_OFF_STATE;
        return 0;
    }

    if (heater[ch].db_bFlashChEnable_state == FALSE) {
		stateHeaterOnState[ch] = H_OFF_STATE;
        return 0;
    }

	// on do !!!
	if (stateHeaterOnState[ch] == H_OFF_STATE) {
		setOnOffNoneByTemp_whenFirstEnable(ch);
		stateHeaterOnState[ch] = H_ON_STATE;
	}


    if (heater[ch].db_heaterCmd_on_off_none == HEATER_OFF) {
        return 0;
    }

	if (isOnCondition(ch)) {
		stateHeaterOnState[ch] = H_ON_STATE;

		return dutycycleCompared[ch]; // on
	}


    return 0;
}

void outputPwm_loop(void) {
/*
    최종 dutycycle, pwm 최종 출력 함수
*/
    uint8_t ch;

    for (ch = 0; ch <MAX_CH; ch++) {
        heater[ch].db_finaldutycycle = getFianlDutycycle(ch);
        outputFinalPwmTo(ch, heater[ch].db_finaldutycycle);
    }
}




void writeFlash(uint8_t ch, bool bEnable) {
    uint16_t BlockPt;

    switch (ch) {
        case 0:
            BlockPt = (F_CH0_ENABLE / FLASH_ONE_BLOCK_SIZE);
            FlashBlockRd(BlockPt);
            cSR_ByteData(F_CH0_ENABLE) = bEnable;
            break;
        case 1:
            BlockPt = (F_CH1_ENABLE / FLASH_ONE_BLOCK_SIZE);
            FlashBlockRd(BlockPt);
            cSR_ByteData(F_CH1_ENABLE) = bEnable;
            break;
        case 2:
            BlockPt = (F_CH2_ENABLE / FLASH_ONE_BLOCK_SIZE);
            FlashBlockRd(BlockPt);
            cSR_ByteData(F_CH2_ENABLE) = bEnable;
            break;
        case 3:
            BlockPt = (F_CH3_ENABLE / FLASH_ONE_BLOCK_SIZE);
            FlashBlockRd(BlockPt);
            cSR_ByteData(F_CH3_ENABLE) = bEnable;
            break;
        case 4:
            BlockPt = (F_CH4_ENABLE / FLASH_ONE_BLOCK_SIZE);
            FlashBlockRd(BlockPt);
            cSR_ByteData(F_CH4_ENABLE) = bEnable;
            break;
    }

    FlashBlockWr(BlockPt);
}



void writeFlash_byNotuse(uint8_t ch) {
	uint16_t BlockPt;

	switch (ch) {
		case 0:
			BlockPt = (F_CH0_ENABLE / FLASH_ONE_BLOCK_SIZE);
			FlashBlockRd(BlockPt);
			cSR_ByteData(F_CH0_ENABLE) = 0;
			break;
		case 1:
			BlockPt = (F_CH1_ENABLE / FLASH_ONE_BLOCK_SIZE);
			FlashBlockRd(BlockPt);
			cSR_ByteData(F_CH1_ENABLE) = 0;
			break;
		case 2:
			BlockPt = (F_CH2_ENABLE / FLASH_ONE_BLOCK_SIZE);
			FlashBlockRd(BlockPt);
			cSR_ByteData(F_CH2_ENABLE) = 0;
			break;
		case 3:
			BlockPt = (F_CH3_ENABLE / FLASH_ONE_BLOCK_SIZE);
			FlashBlockRd(BlockPt);
			cSR_ByteData(F_CH3_ENABLE) = 0;
			break;
		case 4:
			BlockPt = (F_CH4_ENABLE / FLASH_ONE_BLOCK_SIZE);
			FlashBlockRd(BlockPt);
			cSR_ByteData(F_CH4_ENABLE) = 0;
			break;
	}

	FlashBlockWr(BlockPt);
}



void heater_set_flashEnable(uint8_t ch) {
	switch (ch) {
		case 0:
			heater[0].db_bFlashChEnable_state = cF_ch0_enable; // 최종 변경 되는 곳이다.
			return;
		case 1:
			heater[1].db_bFlashChEnable_state = cF_ch1_enable;
			return;
		case 2:
			heater[2].db_bFlashChEnable_state = cF_ch2_enable;
			return;
		case 3:
			heater[3].db_bFlashChEnable_state = cF_ch3_enable;
			return;
		case 4:
			heater[4].db_bFlashChEnable_state = cF_ch4_enable;
			return;
	}
}

void whenRxEnFormPan_write(uint8_t ch) {

	if (heater[ch].bChEnStateFromPannel != heater[ch].db_bFlashChEnable_state) {

		writeFlash(ch, heater[ch].bChEnStateFromPannel);
		pin_RUN_LED = ~pin_RUN_LED;

	}

}

void writeSetFlashEnable(uint8_t ch) {

	writeFlash_byNotuse(ch);

}

bool isUseByLdr(uint8_t ch) {
	switch (ch) {
		case 0:
			return cF_ch0_use;
		case 1:
			return cF_ch1_use;
		case 2:
			return cF_ch2_use;
		case 3:
			return cF_ch3_use;
		case 4:
			return cF_ch4_use;
	}

	return 0;
}


uint8_t useState[5];


void heater_setChEnableDisable(uint8_t ch) {

	// 판넬에서 enable/disable set 정보 일때 write 시도 한다.
	// 판넬 통신이 두절 되는 순간에 대해서도 처리 해야 한다. @09-07
	uint8_t ttt;
	ttt = whatChEnDisNone_byPannel(ch);

	if (ttt == CH_SET_EN) {
		heater[ch].bChEnStateFromPannel = 1;
		whenRxEnFormPan_write(ch); // write

	} else if (ttt == CH_SET_DIS) {

		heater[ch].bChEnStateFromPannel = 0;

		whenRxEnFormPan_write(ch);
	}


	if ( heater[ch].db_bFlashChEnable_state && (isUseByLdr(ch) == FALSE)) {
		// notuse 이면, 채널 off write
		writeSetFlashEnable(ch);
		pin_RUN_LED = ~pin_RUN_LED;

	}

	heater_set_flashEnable(ch);

}

// -------------------------------------------
// ---  use/nouse ---------------------------
bool getUseFromLdr(uint8_t ch) {

    switch (ch) {
        case 0:
            return cF_ch0_use;
        case 1:
            return cF_ch1_use;
        case 2:
            return cF_ch2_use;
        case 3:
            return cF_ch3_use;
        case 4:
            return cF_ch4_use;
    }
    return 0;
}


void heater_setUseNouse(uint8_t ch) {

    heater[ch].db_bChUse = getUseFromLdr(ch);
}


void setWatchDockEnable(void) {
    SWDTEN = 1;
}

void setExtInterrupt(void) {
    PEIE = 1;
}

void setAdcStartOrStop(bool set) {
    b_conversion_ad = set;
}

uint16_t heater_getRealVoltage(uint8_t ch) {
// 현재 입력 아날로그 값을 실제 값으로 변환하여 반환
// ex) 아날로그 : 실제 = 1000 mv : 100 V
// @@ : 중간에 보정 함수를 넣어야 한다.
// ex) 보정 1V 라면, 아날로그 : 실제 = 1000 mv : 101V
	uint16_t in_mV = (heater[ch].anlog_correctedVoltage_mV / 10);
	uint16_t correct = heater[ch].userCorrVoltSet;
	uint16_t corr_value;
	corr_value = correct % 100;
	if (correct >= SIGN_POSIT_V_OR_A) {
		// + 보정
		return in_mV + corr_value;
	} else {	// - 보정
		if (in_mV > corr_value) {
			return in_mV - corr_value;
		}
	}
	return in_mV;
}
uint16_t heater_getRealAmp(uint8_t ch) {
// 현재 입력 아날로그 값을 실제 값으로 변환하여 반환
// ex) 아날로그 : 실제 = 1000 mv : 100 V
// @@ : 중간에 보정 함수를 넣어야 한다.
// ex) 보정 1V 라면, 아날로그 : 실제 = 1000 mv : 101V
	uint16_t in_mV = (heater[ch].anlog_correctedAmp_mV / 10);
	uint16_t correct = heater[ch].userCorrAmpSet;
	uint16_t corr_value;
	corr_value = correct % 100;
	if (correct >= SIGN_POSIT_V_OR_A) {
		// + 보정
		return in_mV + corr_value;
	} else {	// - 보정
		if (in_mV > corr_value) {
			return in_mV - corr_value;
		}
	}
	return in_mV;
}


void chkEmptyUerV(uint8_t ch) {
	if (heater[ch].userNowInVoltage_V > 10) { // 10V
		inputV_EmptyErrorChktiemr[ch] = 0;
		return;
	}

	if (inputV_EmptyErrorChktiemr[ch] < 0xffff) {
		inputV_EmptyErrorChktiemr[ch]++;
	}
}

void chkEmptyUerA(uint8_t ch) {
	if (heater[ch].userNowInAmp_100mA > 10) { // 1A
		inputA_EmptyErrorChktiemr[ch] = 0;
		return;
	}

	if (inputA_EmptyErrorChktiemr[ch] < 0xffff) {
		inputA_EmptyErrorChktiemr[ch]++;
	}
}

void chkEmptyEorrChktiemr(uint8_t ch) {
	if (isPwmOnState(ch)) {

		chkEmptyUerV(ch);
		chkEmptyUerA(ch);

	} else {
		inputV_EmptyErrorChktiemr[ch] = 0;
		inputA_EmptyErrorChktiemr[ch] = 0;
	}
}

void heater_initSetDisable(uint8_t ch) {
	bool bEnable;
	bEnable = 0;
	writeFlash(ch, bEnable);
}

void oscillator_fail_loop(void) {

    if(OSCFIF) {
    	OSCFIF = 0;
    	asm("reset");
    }

}

uint16_t getSetTempLow(uint8_t ch) {
	switch (ch) {
		case 0:
			return cF_ch0_temp_low + 1; // ※ ex. 27.9 부터 on 시키기 위해서
		case 1:
			return cF_ch1_temp_low + 1;
		case 2:
			return cF_ch2_temp_low + 1;
		case 3:
			return cF_ch3_temp_low + 1;
		case 4:
			return cF_ch4_temp_low + 1;
	}
	return 0;
}

uint16_t getSetTempHigh(uint8_t ch) {
	switch (ch) {
		case 0:
			return cF_ch0_temp_high;
		case 1:
			return cF_ch1_temp_high;
		case 2:
			return cF_ch2_temp_high;
		case 3:
			return cF_ch3_temp_high;
		case 4:
			return cF_ch4_temp_high;
	}
	return 0;
}

uint16_t getSetCorrT(uint8_t ch) {
	switch (ch) {
		case 0:
			return cF_ch0_correctT;
		case 1:
			return cF_ch1_correctT;
		case 2:
			return cF_ch2_correctT;
		case 3:
			return cF_ch3_correctT;
		case 4:
			return cF_ch4_correctT;
	}
	return 0;
}

uint16_t getSetCorrV(uint8_t ch) {
	switch (ch) {
		case 0:
			return cF_ch0_correctV;
		case 1:
			return cF_ch1_correctV;
		case 2:
			return cF_ch2_correctV;
		case 3:
			return cF_ch3_correctV;
		case 4:
			return cF_ch4_correctV;
	}
	return 0;
}

uint16_t getSetCorrA(uint8_t ch) {
	switch (ch) {
		case 0:
			return cF_ch0_correctA;
		case 1:
			return cF_ch1_correctA;
		case 2:
			return cF_ch2_correctA;
		case 3:
			return cF_ch3_correctA;
		case 4:
			return cF_ch4_correctA;
	}
	return 0;
}

bool getCheckNotCommTmep(void) {
	return not_rx_temp_chkTimer > 3000;
}
bool getCheckNotCommPannel(void) {
	return not_rx_pannel_chkTimer > 3000;
}

bool getCheckCommBreakPnnel(void) {
	b_pannel_comm_not = getCheckNotCommPannel();
	if (bef_b_pannel_comm_not != b_pannel_comm_not) {
		bef_b_pannel_comm_not = b_pannel_comm_not;
		// 통신 <-> 통신x
		if (b_pannel_comm_not) {
			// 통신 두절 된 상태
			// 판넬 전원 off 순간과 같은 상태
			// 모든 ch를 off 해라.
			return 1;
		}
	}

	return 0;
}


// #1008 부식방지 시스템
uint16_t gateRST_doValue;
void test_manualVol(void) {
	uint16_t volAnalog = heater[1].adc_nowAnalog_mV; // 6~4994
	gateRST_doValue = volAnalog / 28;
}



// -------------------------
// -------------------------
void main(void) {
	uint8_t ch;

    di();
    initMCU();
    initPort();
    initTimer0();
	initTimer1();
	initInt0();
	initInt1();
	initInt2();
	// INT1, INT2 입력 설정
    initAdc();
    initLoaderUart2();
    //initPwm();
	// can 셋팅
	init_canPort();
    InitCAN();
    ei();
    setExtInterrupt();
    setWatchDockEnable();
    setAdcStartOrStop(1);


    //offPwmAll();

	for (ch = 0; ch < 5; ch++) {
	// 전원 키면, enable/disable 값을 초기화 하기
		heater_initSetDisable(ch);
	}

	// 온도/판넬   통신 두절 체크 용 변수 초기화
	not_rx_temp_chkTimer = 0;
	b_temp_comm_not = 0;
	not_rx_pannel_chkTimer = 0;
	b_pannel_comm_not = 0;
	bef_b_pannel_comm_not = 0;
	b_pannel_comm_break_flag = 0;

    while (1) {
        unsigned int i;
        uint8_t ch;

        CLRWDT();
		oscillator_fail_loop();

// ========= 이 기능은 단순히 히터 on, off 기능이다. ========
// 단순히 온도 상한선에 따라서 on, off 해주는 것이 가장 중요한 것이다.

// 히터 on, off 기능
//
        for (ch = 0; ch < MAX_CH; ch++) {
            heater_setChEnableDisable(ch);
            heater_setUseNouse(ch);
            heater_setOnOffNone(ch);
            heater_setNowInVoltage_mV(heater, ch);
            heater_setNowInCurrent_mA(heater, ch);

			heater_setTempError(ch);

			heater[ch].db_errorCode = heater_chkEtcError(ch);
            heater[ch].anlog_correctedVoltage_mV = heater[ch].adc_nowAnalog_mV; // 현재 전압
            heater[ch].anlog_correctedAmp_mV = heater[ch].anlog_nowAmp_mV; // 현재 전류
			// 전압/전류 보정된 최종 실제(유저) 데이터 저장
			heater[ch].userNowInVoltage_V = heater_getRealVoltage(ch);
			heater[ch].userNowInAmp_100mA = heater_getRealAmp(ch);

			// 전압/전류 로더 Goal Set Volt/Amp
			heater[ch].goalSetVoltage_V = getUserSetGoalVolt(ch);
			heater[ch].goalSetAmp_100mA = getUserSetGoalAmp(ch);

            heater[ch].db_heatingOnState = heater_getOnState(heater[ch].db_finaldutycycle);
            heater[ch].analog_goalSetVoltage_mV = heater_getAnalogGoalSetVoltage_mv(ch);
            heater[ch].analog_goalSetCurrent_mV = heater_getAnalogGoalAmp_mv(ch);


			// 셋팅 온도 보정된 아날로그 값 데이터 베이스 저장
			heater[ch].db_setOnTemp_mV = temp_getAnalogTo(getSetTempLow(ch), ch);
			heater[ch].db_setOffTemp_mV = temp_getAnalogTo(getSetTempHigh(ch), ch);
			// 보정값 database에 저장하기
			heater[ch].corrTempSet = getSetCorrT(ch);
			heater[ch].userCorrVoltSet = getSetCorrV(ch);
			heater[ch].userCorrAmpSet  = getSetCorrA(ch);

			// 현재 입력 전압, 전류 입력값을 비교해서
			// 듀티싸이클 값 얻어 오기 위한 목적
			compareNowVoltageCurrent(ch);
        }
// --- system loop ----------------------------------
// adc
// * 10개 채널의 입력 mV 값 읽어 오는게 목적이다.
        adc_loop();

// 통신
        can_tx_loop();
        rxtx_loop();

// pwm 출력
        //outputPwm_loop();

// 통신 두절 판단하디 @09-07
		b_temp_comm_not = getCheckNotCommTmep();

		b_pannel_comm_break_flag = getCheckCommBreakPnnel();
		if (b_pannel_comm_break_flag) {
			b_pannel_comm_break_flag = 0;

			for (ch = 0; ch < MAX_CH; ch++) {
				b_pannel_comm_break_flag_ch[ch] = 1;
			}
		}

// #1008 부식방지 시스템

	test_manualVol();

    }
}

#define PIN_R_PH_HIGH	1
#define PIN_R_PH_LOW	0



volatile unsigned int timer1_test;
void interrupt isr(void) {
    static unsigned int timer_msec = 0;
	uint8_t ch;


	if(INT0IF && INT0IE){
  		INT0IF = 0;

	}
	if(INT1IF && INT1IE){
  		INT1IF = 0;
		pin_GATE_S_PH = ~pin_GATE_S_PH;
	}
	if(INT2IF && INT2IE){
  		INT2IF = 0;
		pin_GATE_T_PH = ~pin_GATE_T_PH;
	}


	if (TMR1IF) {
		TMR1IF = 0;
	    TMR1L = MSEC_L_1;
	    TMR1H = MSEC_H_1;
		timer1_test++;
		if (timer1_test >= gateRST_doValue) {
			timer1_test = 0;
			// #1008 gate 출력 넣어야 함
			pin_GATE_R_PH = ~pin_GATE_R_PH;
		}
	}

    if (TMR0IF) {
        TMR0IF = 0;
        TMR0L = MSEC_L;
        TMR0H = MSEC_H;
        loader_msecTimer();
        timer_msec++;

        if (timer_msec >= 1000) {
            timer_msec = 0;
			pin_RUN_LED = ~pin_RUN_LED;
        }

        NoCanInt++;

        if (rxGoodSucessTimer < 0xffff) {
            rxGoodSucessTimer++;
        }

		for (ch = 0; ch < 5; ch++) {
			chkEmptyEorrChktiemr(ch);
		}

		if (not_rx_temp_chkTimer < 0xffff) {
			not_rx_temp_chkTimer++;
		}
		if (not_rx_pannel_chkTimer < 0xffff) {
			not_rx_pannel_chkTimer++;
		}
    }


    if (ADIF) {
        unsigned int ad;

        ad = ADRES;
        if (ad > 5500) ad = 0; // 07-17 마이너스 값 왔을 경우를 대비해서
         set_input_ad(ad);
        ADIF = 0;
        b_conversion_ad = 0;
    }


    loader_uart_interrupt();


    if (PIR5 > 0) {
        can_rxInterrupt();
    }
}


