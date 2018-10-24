
#include <pic18.h>
#include	"main.h"
#include	"mcu_pic18f65k80.h"
#include	"initial.h"
#include	"ad_conversion.h"
#include	"system\system.h"
#include	"loader_45k80\loader_45k80_main.h"
#include    "pwm.h"
#include    "can\my_can.h"

#define MAX_GATE_min_voltage	240 // 토크 0 따라서, 전압 0
#define MIN_GATE_max_voltage	60


#define MAX_CH 5
uint8_t bufZSU_use_not[ZSU_CH_MAX];


uint8_t rx_db_enableSet[MAX_CH];
uint16_t dutycycleCompared[MAX_CH];

uint16_t not_rx_temp_chkTimer; // 온도 보드 통신 두절 여부 체크 타이머 변수 09-07
bool b_temp_comm_not;
uint16_t not_rx_pannel_chkTimer; // 판넬 보드 통신 두절 여부 체크 타이머 변수 09-07
bool b_pannel_comm_not;
bool bef_b_pannel_comm_not;
bool b_pannel_comm_break_flag; // 판넬 통신이 딱 두절 되는 순간 !! => 전원을 off한거와 같은 효과
bool b_pannel_comm_break_flag_ch[5] = {0,};

uint8_t nRunStep;

enum {
	STEP_CHKING,
	STEP_ERROR,
	STEP_GOOD
}


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

enum {
	S_MINUS,
	S_PLUS,
	S_GOOD,
	S_ERROR
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


Heater heater[1];
Heater scr;
uint16_t zsu_ch0_ch7_analog[8]; // zsu 아날로그 채널 8개 저장 값



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
    for (i=0; i<8; i++) {
		// 서브로 부터 온도 아날로그 저장
        zsu_ch0_ch7_analog[i]
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


void scr_micom_setNowInVoltage_mV(uint8_t ch) {
    if (adc_bUpdated[ch]) {
		adc_bUpdated[ch] = FALSE;
		switch (ch) {
			case 0:
				// AN0 센서
				scr.nowMicomMainAdSensor = adc_updated_analog_mV[ch]; // 현재 아날로그 값
				scr.bNowMicomAdSensor_updted = TRUE;
				break;
			case 1:
				// AN1 수동 볼륨 값
				scr.nowMicomAdVolume = adc_updated_analog_mV[ch]; // 현재 아날로그 값
				scr.bNowMicomAdVolume_updted = TRUE;
				break;
			case 2:
				// AN2 전류
				scr.nowMicomAdCurrent = adc_updated_analog_mV[ch]; // 현재 아날로그 값
				scr.bNowMicomAdCurrent_updted = TRUE;
				break;
			case 3:
				// AN3 전압
				scr.nowMicomAdVoltage = adc_updated_analog_mV[ch]; // 현재 아날로그 값
				scr.bNowMicomAdVoltage_updted = TRUE;
				break;
			default:
				break;
		}

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




uint16_t correctedFinalGoalAnalogVolt_mv(uint8_t ch) {
	return 0;
}

uint16_t correctedFinalGoalAnalogAmp_mv(uint8_t ch) {

	return 0;
}



uint16_t heater_getAnalogGoalSetVoltage_mv(uint8_t ch) {
    return 0;
}

uint16_t heater_getAnalogGoalAmp_mv(uint8_t ch) {

    return 0;
}


enum {
	CONT_VOLT,
	CONT_CURR
};

enum {
    ERR_NONE_X  = 0,
    ERR_BREAK = 1,
    ERR_SHORT = 2,
    ERR_NOT_HEATING = 3,
    ERR_ETC,
} ErrorCodeXXX;

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
    return 0;
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
}







void whenRxEnFormPan_write(uint8_t ch) {

	if (heater[ch].bChEnStateFromPannel != heater[ch].db_bFlashChEnable_state) {


	}

}





uint8_t useState[5];


void heater_setChEnableDisable(uint8_t ch) {



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
}

void oscillator_fail_loop(void) {

    if(OSCFIF) {
    	OSCFIF = 0;
    	asm("reset");
    }

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

/*/////////////////////////////////
/* #부식방지 시스템
/*//////////////////////////////////

#define GATE_H		1
#define	GATE_L	0
#define PIN_R_PH_HIGH	1
#define PIN_R_PH_LOW	0

uint16_t pwstartTiemr; // 최초 전원 켰을 때 순간 게이트 on 되는 문제 때문에 추가 된 시간 변수이다.

bool bRzeroTimerStart;
bool bSzeroTimerStart;
bool bTzeroTimerStart;

uint16_t gateRSTDo_time; // 수동에의해 최종 몇 도 조절 할지의 값이다.
// 이 값이 결국은 PWM 의 듀티값과 같다고 보면 된다.
// 이값을 조절함으로 써 목표 전압/전류/센서 값을 제어하면 된다.

uint16_t test_manualVol(void) {
	uint16_t volAnalog = scr.nowMicomAdVolume; // 6~4994
	return (180 - (volAnalog / 28) + 60);
}


uint16_t timerRGateHighChk;
uint16_t timerSGateHighChk;
uint16_t timerTGateHighChk;



uint16_t tiemrRzeroTimer;
uint16_t tiemrSzeroTimer;
uint16_t tiemrTzeroTimer;

uint16_t micom_sensor_0_8_mV[9];
void micom_saveTotal8sensorNowValue(void) {
	uint8_t ch;
	for (ch = 0; ch < 8; ch++) {
		micom_sensor_0_8_mV[ch] = zsu_ch0_ch7_analog[ch];
	}
	micom_sensor_0_8_mV[ch] = scr.nowMicomMainAdSensor;
}



void whenRZeroEdgeUp(void) {
	if (bRzeroTimerStart) {
		// 제로 타이머 증가
		tiemrRzeroTimer++;
		// 제로 타이머가 설정 시간 되면, 게이트 하이 !
		if (tiemrRzeroTimer >= gateRSTDo_time) {
			tiemrRzeroTimer = 0;
			pin_GATE_R_PH = GATE_H;
			bRzeroTimerStart = 0;
			timerRGateHighChk = 0;
		}
	}
}
void whenSZeroEdgeUp(void) {
	if (bSzeroTimerStart) {
		// 제로 타이머 증가
		tiemrSzeroTimer++;
		// 제로 타이머가 설정 시간 되면, 게이트 하이 !
		if (tiemrSzeroTimer >= gateRSTDo_time) {
			tiemrSzeroTimer = 0;
			pin_GATE_S_PH = GATE_H;
			bSzeroTimerStart = 0;
			timerSGateHighChk = 0;
		}
	}
}
void whenTZeroEdgeUp(void) {
	if (bTzeroTimerStart) {
		// 제로 타이머 증가
		tiemrTzeroTimer++;
		// 제로 타이머가 설정 시간 되면, 게이트 하이 !
		if (tiemrTzeroTimer >= gateRSTDo_time) {
			tiemrTzeroTimer = 0;
			pin_GATE_T_PH = GATE_H;
			bTzeroTimerStart = 0;
			timerTGateHighChk = 0;
		}
	}
}


void lowRGate(void) {
	timerRGateHighChk++;
	if (timerRGateHighChk > 1) { // 2
		timerRGateHighChk = 0;
		pin_GATE_R_PH = GATE_L;
	}
}
void lowSGate(void) {
	timerSGateHighChk++;
	if (timerSGateHighChk > 1) {
		timerSGateHighChk = 0;
		pin_GATE_S_PH = GATE_L;
	}
}
void lowTGate(void) {
	timerTGateHighChk++;
	if (timerTGateHighChk > 1) {
		timerTGateHighChk = 0;
		pin_GATE_T_PH = GATE_L;
	}
}


void onRSTGATEWhenManual(void) {
	whenRZeroEdgeUp(); // gate on(high) !!

	whenSZeroEdgeUp();

	whenTZeroEdgeUp();

}





void offRSTGATEWhenOn(void) {
	if (pin_GATE_R_PH == GATE_H) {
		lowRGate();
	}
	if (pin_GATE_S_PH == GATE_H) {
		lowSGate();
	}
	if (pin_GATE_T_PH == GATE_H) {
		lowTGate();
	}
}

// 정/역
uint16_t finalSensor_mV_old[ZSU_CH_MAX+1][2];
void reverseZSU8ch_old(uint16_t ch) {

	if (zsu_ch0_ch7_analog[ch] >= 2500) {
		// +500
		finalSensor_mV_old[ch][0] = S_PLUS; // +
		finalSensor_mV_old[ch][1] = (zsu_ch0_ch7_analog[ch] - 2500); // 500
		return;
	}

	// -500
	finalSensor_mV_old[ch][0] = S_MINUS; // -
	finalSensor_mV_old[ch][1] = (2500 - zsu_ch0_ch7_analog[ch]); // 500
}
void reverseMainSensor_old(uint16_t sensor) {

	if (sensor >= 2500) {
		// +500
		finalSensor_mV_old[8][0] = S_PLUS; // +
		finalSensor_mV_old[8][1] = (sensor - 2500); // 500
		return;
	}

	// -500
	finalSensor_mV_old[8][0] = S_MINUS; // -
	finalSensor_mV_old[8][1] = (2500 - sensor); // 500
}

bool getSensorTypeByCh(uint16_t i) {
	// 9개의 정/역 비교를 위한 로더 설정 값
	switch (i) {
		case 0:
			return cF_sensorType_0;
		case 1:
			return cF_sensorType_1;
		case 2:
			return cF_sensorType_2;
		case 3:
			return cF_sensorType_3;
		case 4:
			return cF_sensorType_4;
		case 5:
			return cF_sensorType_5;
		case 6:
			return cF_sensorType_6;
		case 7:
			return cF_sensorType_7;
		case 8:
			return cF_sensorType_8;
	}


	return 1; // 가정, 정(+)
}

void filterGoodValue_old(void) { // #1023 알람 테스트 함수때 필요하다.
// 메뉴 설정값과 현재 입력 값의 기호가 같은지 체크 한다.
// 이것은 알람 테스트 기능에서 필요한 함수이다.
	uint16_t i;
	for (i = 0; i < (ZSU_CH_MAX+1); i++) {
		if (finalSensor_mV_old[i][0] == getSensorTypeByCh(i)) {
			// 메뉴 설정 값과 입력 값이 같다면,
			finalSensor_mV_old[i][0] = S_GOOD;
		} else {
			finalSensor_mV_old[i][0] = S_ERROR;
		}
	}
}
uint16_t getMaxValue_old(void) {
	uint16_t max = 0;
	uint16_t i;
	for (i = 0; i < (ZSU_CH_MAX+1); i++) {
		if (finalSensor_mV_old[i][0] == S_GOOD) {
			if (finalSensor_mV_old[i][1] > max) {
				max = finalSensor_mV_old[i][1];
			}
		}
	}
	return max;
}




// #1023 추후 센서 알람 테스트 시에 참고 함수 @중요
volatile uint16_t sensorMax;
uint16_t getMicomFinalMaxSensor_old(void) {
	uint16_t ch;
	// 모든(총 9개) 센서 입력값을 버퍼에 저장한다.
	for (ch = 0; ch < ZSU_CH_MAX; ch++) {
		reverseZSU8ch_old(ch); // 2500 기준으로 각 채널 값에 +/- 기호 값을 저장하였다.
	}
	reverseMainSensor_old(scr.nowMicomMainAdSensor); // 2500 기준으로 메인 채널 값에 +/- 기호 값을 저장하였다.

	// 이 중에 최대 값을 리턴한다.
	filterGoodValue_old();
	sensorMax = getMaxValue_old();
	return sensorMax;
}
// #1022 그냥 현재 센서 입력 값 중에 가장 큰 값을 마이컴단 값으로 가져오면 된다.
uint16_t getMicomFinalMaxSensor(uint16_t now_main) {
	uint16_t max = now_main;
	uint16_t i;
	// 총 9개 센서 현재 입력값 중에 최대 값을 얻기 위해서
	// 하지만 기본적으로 메인의 값을 기본 값으로 가져가므로
	// zsu 값 8개에 대해서 만 루프 돌리면 된다.
	for (i = 0; i < ZSU_CH_MAX; i++) {
		// 가장 큰 값을 뽑아야 한다.
		if (zsu_ch0_ch7_analog[i] > max) {
			max = zsu_ch0_ch7_analog[i];
		}
	}
	return max;
}




void decreadeDosu(uint16_t * pGateRSTDoValue) {
	if (*pGateRSTDoValue > 0) {
		*pGateRSTDoValue -= 1;
	}
}


void increaseDosu(uint16_t * pGateRSTDoValue) {
	if (*pGateRSTDoValue < 180) {
		*pGateRSTDoValue += 1;
	}
}


bool isOverVoltage(void) {
	uint16_t goal = (scr.goalSetVoltage_V * 4); // micom 목표 전압 100v -> 4000mV
	uint16_t now = scr.nowMicomAdVoltage; // AN3, micom 현재 전압

    if (now > goal) {
        return 1;
    }   return 0;
}
bool isOverCurrent(void) {
	uint16_t goal = (scr.goalSetAmp * 4); // micom 목표 전압 (50A : 2500mV 기준)
	uint16_t now = scr.nowMicomAdCurrent; // AN3, micom 현재 전압

    if (now > goal) {
        return 1;
    }   return 0;
}

// 전압 제어
void compareGoalNowVoltage(void) {
	if (scr.bNowMicomAdVoltage_updted) {
		scr.bNowMicomAdVoltage_updted = FALSE;

		if (gateRSTDo_time < MAX_GATE_min_voltage) {
			gateRSTDo_time += 1;
		}
	}
}
// 전류 제어
void compareGoalNowCurrent(void) {
	if (scr.bNowMicomAdCurrent_updted) {
		scr.bNowMicomAdCurrent_updted = FALSE;

		if (gateRSTDo_time < MAX_GATE_min_voltage) {
			gateRSTDo_time += 1;
		}
	}
}


uint16_t getMicomGoalSensorVal(uint16_t val) {
	uint32_t signalNumber[2];
	uint16_t micomVal;

	getSignalUserNumXXX(signalNumber, (uint32_t)val);

	switch (signalNumber[0]) {
		case SIGN_PLUS:
			micomVal = signalNumber[1] + 2500;
			break;
		case SIGN_MINUS:
			micomVal = 2500 - signalNumber[1];
			break;
	}

	return micomVal;
}


void compareGoalNowSensor(void) {
// 센서 제어
	// 마이컴 단에서는 그냥 2500mv 라는 기준치 없다.
	uint16_t goal; // micom단, 목표 센서 mV
		// 로더에 설정된 목표 값은 +/- 값 이므로, 마이컴 단 값으로 변환 시켜줘야 한다.
	uint16_t now; // AN3, micom 현재 전압
	// 목표 값과 현재 입력 전압값을 가져왔으니 둘을 비교해서
	// RSTGATE ON 지점의 도수를 증가하거나 감소한다. (제어하기 !)
	if (scr.bNowMicomAdSensor_updted) {
		scr.bNowMicomAdSensor_updted = FALSE;

		goal = getMicomGoalSensorVal(scr.goalSetSensor); // ★
		now = getMicomFinalMaxSensor(scr.nowMicomMainAdSensor);

		// #1017 에어컨 온도 제어 처럼 위 아래로 +50/-50 을 두었다.
		// 이것을 추후 어떻게 할지 테스트 장비 오면 수정하자.
		if (now < (goal-0)) {
			if (gateRSTDo_time < MAX_GATE_min_voltage) {
				gateRSTDo_time += 1;
			}
		} else if (now > (goal+0)) {
			// 전류를 많이 보내 줘야 한다.
			if (gateRSTDo_time > MIN_GATE_max_voltage) {
				gateRSTDo_time -= 1;
			}
		}
	}
}

void controlGateTotal(void) {
// 전압, 전류, 센서 제어
	if (isOverVoltage()) {
		compareGoalNowVoltage();
		return;
	} else if (isOverCurrent()) {
		compareGoalNowCurrent();
		return;
	}

	// ★ 센서 제어
	compareGoalNowSensor();
}


// 전압 제어 코딩 성공 !
// 목표 전압치 설정하기
// 필요한 것
//		- 목표 전압치
//		- 현재 전압값
//		- 이를 비교하여 게이트 ON 기준점 변화 주기
void autoRGateOnOff(void) {
	if (bRzeroTimerStart) { // 제로
		// 제로 타이머 증가
		tiemrRzeroTimer++;
		// 제로 타이머가 설정 시간 되면, 게이트 하이 !
		if (tiemrRzeroTimer >= gateRSTDo_time) {
			tiemrRzeroTimer = 0;
			pin_GATE_R_PH = GATE_H;
			bRzeroTimerStart = 0;
			timerRGateHighChk = 0;
		}
	}
}
void autoSGateOnOff(void) {
	if (bSzeroTimerStart) { // 제로
		// 제로 타이머 증가
		tiemrSzeroTimer++;
		// 제로 타이머가 설정 시간 되면, 게이트 하이 !
		if (tiemrSzeroTimer >= gateRSTDo_time) {
			tiemrSzeroTimer = 0;
			pin_GATE_S_PH = GATE_H;
			bSzeroTimerStart = 0;
			timerSGateHighChk = 0;
		}
	}
}
void autoTGateOnOff(void) {
	if (bTzeroTimerStart) { // 제로
		// 제로 타이머 증가
		tiemrTzeroTimer++;
		// 제로 타이머가 설정 시간 되면, 게이트 하이 !
		if (tiemrTzeroTimer >= gateRSTDo_time) {
			tiemrTzeroTimer = 0;
			pin_GATE_T_PH = GATE_H;
			bTzeroTimerStart = 0;
			timerTGateHighChk = 0;
		}
	}
}

void onRSTGATEWhenAuto(void) {
	autoRGateOnOff();
	autoSGateOnOff();
	autoTGateOnOff();
}

// #1024 4단계 테스트 알람
bool isJustNowPowerOn(void) {
	static bool bBefOffState = 1;

	if (pin_KEY_POWER == KEY_POWER_OFF) {
	// off
		bBefOffState = 1;
	} else {
	// on
		if (bBefOffState) {
			bBefOffState = 0;
			return 1;
		}
	}

	return 0;
}




void controlSensorSuWi(void) {
	// 46us = 1도
	if (!pin_AUTO) {
		// 수동 볼륨에 의해서 gateRSTDo_time 값을 획득한다.
		if (scr.bNowMicomAdVolume_updted) {
			scr.bNowMicomAdVolume_updted = FALSE;
			gateRSTDo_time = test_manualVol();
		}
	} else {
		// PWM 처럼 게이트 제어하는 함수이다.
		controlGateTotal();
	}
}

enum {
	TYPE_ZINC,
	TYPE_CUCUSO4
};

uint16_t micom_getSensorNowSuwi(uint8_t ch) {

	return micom_sensor_0_8_mV[ch];

}


uint16_t micom_getLdrSetSRP_max(uint16_t setvalue) {

	return 1000;
}

uint16_t micom_getLdrSetSRP_min(uint16_t setvalue) {
	return 4000;
}

volatile uint16_t a, b;
uint8_t isSRPError(void) {
/* 1단계 알람 테스트 기능 구현하기
	* SRP MAX
	* SRP LOW : - 방향에 대한 것이므로 Zinc일때 이 값을 보고 판단한다.
	* SRP Time : check time
*/
	uint8_t ch;
	uint8_t senseorType[9]; // sensor 별로 total 9개  // zinc / cucs
	uint16_t micom_nowSensorSuwi[9]; // sensor 별로 total 9 개  // 현재 수위 입력 값 (사용자 읽기용)
	//---------------------------------------------
	uint16_t micom_SRP_max = micom_getLdrSetSRP_max(iF_SRP_max);		// 설정값 메뉴 (전체)
	uint16_t micom_SRP_min = micom_getLdrSetSRP_min(iF_SRP_min); 		// 설정값 메뉴 (전체)
//	uint16_t setChkTime_SRP = iF_SRP_time; // 설정값 메뉴
	uint16_t setChkTime_SRP = 10000; // 설정값 메뉴

	if (chkTimer_SRP_msec > setChkTime_SRP) {
		return STEP_GOOD;
	}

	// 실제 에러 체크 (총 9개 센서에 대해서)
//	for (ch = 0; ch < 9; ch++) {

ch = 0;
		senseorType[ch] = getSensorTypeByCh(ch); // zinc = 0
		micom_nowSensorSuwi[ch] = micom_getSensorNowSuwi(ch);
a = micom_SRP_min; // 4000
b = micom_nowSensorSuwi[ch]; // 1775
		switch (senseorType[ch]) {
			case TYPE_ZINC:
				// 1775 < 4000
				if (micom_nowSensorSuwi[ch] < micom_SRP_min) {
					return STEP_ERROR;
				}
				break;
			case TYPE_CUCUSO4:
				if (micom_nowSensorSuwi[ch] > micom_SRP_max) {
					return STEP_ERROR;
				}
				break;
		}
//	}




	return STEP_CHKING;
}
bool isSOPError(void) {
	return 0;
}


void processErrorCode(uint8_t errorcode) {

}

enum {
	ERR_NONE,
	ERR_SRP,	// 센서 역전
	ERR_SOP,	// 단선
	ERR_AOP,	//
	ERR_ARP,
	ERR_UPR,
	ERR_OPR,
	ERR_FOP // RST 상
};


void loop_allStepRun(uint8_t step) {
	static uint8_t errorCode;
	uint8_t chk;

	switch (step) {
		case 1:
			// 설정 시간 동안
			// sensor별 = type | 현재 수위값 |
			chk = isSRPError(); // 0 ~ 7 , 8(main)
			// 하나라도 에러 발생하면 !
			if (chk == STEP_ERROR) {
				errorCode = ERR_SRP;
				break;
			} else if (chk == STEP_GOOD) {
				errorCode = ERR_NONE;
				step = 2;
				break;
			}
			break;
		case 2:
			if (isSOPError()) {
				errorCode = ERR_SOP;
				break;
			}
			break;
		case 3:
			//checkAOP();
			break;
		case 4:
			//checkARP();
			break;
		case 5:
			controlSensorSuWi();
			break;
		default:
			break;
	}

	processErrorCode(errorCode);

}


void initSystem(void) {

	gateRSTDo_time = MAX_GATE_min_voltage;
	pin_RY_RUN = RY_OFF;
	pin_RY_ALARM = RY_OFF;
}

// -------------------------------------
// - main loop ------------------------
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

	// 온도/판넬   통신 두절 체크 용 변수 초기화
	not_rx_temp_chkTimer = 0;
	b_temp_comm_not = 0;
	not_rx_pannel_chkTimer = 0;
	b_pannel_comm_not = 0;
	bef_b_pannel_comm_not = 0;
	b_pannel_comm_break_flag = 0;

	// 초기화
	gateRSTDo_time = MAX_GATE_min_voltage;
	bRzeroTimerStart = 0;
	bSzeroTimerStart = 0;
	bTzeroTimerStart = 0;
	tiemrRzeroTimer = 0;
	tiemrSzeroTimer = 0;
	tiemrTzeroTimer = 0;

	pwstartTiemr = 0;

	mysucessTimer = 0xffff;


    while (1) {
        unsigned int i;
        uint8_t ch;

        CLRWDT();
		oscillator_fail_loop();

// ad conversion
// * 10개 채널의 입력 mV 값 읽어 오는게 목적이다.
        adc_loop();

// 통신
        can_tx_loop();
	    rxtx_loop();

// pwm 출력
        //outputPwm_loop();

//////////
		b_temp_comm_not = getCheckNotCommTmep();

		b_pannel_comm_break_flag = getCheckCommBreakPnnel();
		if (b_pannel_comm_break_flag) {
			b_pannel_comm_break_flag = 0;

			for (ch = 0; ch < MAX_CH; ch++) {
				b_pannel_comm_break_flag_ch[ch] = 1;
			}
		}

// #부식방지 시스템 -----------------------------------------

		// 전압/전류/센서   로더 목표 설정값 가져오기       ---Goal Set Volt/Amp/Sensor
		scr.goalSetVoltage_V = iF_scr_goalVoltage;
		scr.goalSetAmp = iF_scr_goalCurrent;
		scr.goalSetSensor = iF_scr_goalSensor;

		// 메인 보드의 총 4개(전압/전류/센서/볼륨)의 아날로그 채널 값 획득하기 (저장하기)
		for (ch = 0; ch < ADC_CH_MAX; ch++) {
			scr_micom_setNowInVoltage_mV(ch);
		}

		// ZSU use/not_use 셋팅 값 저장하기
		bufZSU_use_not[0] = cF_ch0_use;
		bufZSU_use_not[1] = cF_ch1_use;
		bufZSU_use_not[2] = cF_ch2_use;
		bufZSU_use_not[3] = cF_ch3_use;
		bufZSU_use_not[4] = cF_ch4_use;
		bufZSU_use_not[5] = cF_ch5_use;
		bufZSU_use_not[6] = cF_ch6_use;
		bufZSU_use_not[7] = cF_ch7_use;

		// #1024 4단계 테스트 알람 기능 구현하기
		micom_saveTotal8sensorNowValue();

		if (isJustNowPowerOn()) {
			// 테스트 시작 !!!!
			mysucessTimer = 0;
			// setp 및 변수들 초기화
			nRunStep = 1;
			chkTimer_SRP_msec = 0;
		}
		if (pin_KEY_POWER == KEY_POWER_ON) {
			// 전체 제어 순서
			loop_allStepRun(nRunStep);
		} else {
			initSystem();
		}
    }
}

void interrupt isr(void) {
    static unsigned int timer_msec = 0;
	uint8_t ch;

	// ZERO RST 입력 인터럽트 (상승 엣지) 체크
	if(INT0IF && INT0IE){
  		INT0IF = 0;
		bRzeroTimerStart = 1; // 제로 타이머 시작을 위한 제로 RST 시작 지점
		tiemrRzeroTimer = 0;
	}
	if(INT1IF && INT1IE){
  		INT1IF = 0;
		bSzeroTimerStart = 1;
		tiemrSzeroTimer = 0;
	}
	if(INT2IF && INT2IE){
  		INT2IF = 0;
		bTzeroTimerStart = 1;
		tiemrTzeroTimer = 0;
	}

	// RST 게이트 ON 용 타이머1
	if (TMR1IF) {
		TMR1IF = 0;
	    TMR1L = MSEC_L_1;
	    TMR1H = MSEC_H_1;

		if (pwstartTiemr > 10) {
			if (!pin_AUTO) {
				onRSTGATEWhenManual();
			} else {
				onRSTGATEWhenAuto();
			}
		}
		// 수동/자동 on되면 무조건 off 해야 한다.
		offRSTGATEWhenOn();
	}

    if (TMR0IF) {
        TMR0IF = 0;
        TMR0L = MSEC_L;
        TMR0H = MSEC_H;
        loader_msecTimer();
        timer_msec++;
		if (pwstartTiemr < 0xffff) pwstartTiemr++;
        if ((timer_msec >= 1000) && (mysucessTimer < 5000)) {
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

		if (mysucessTimer < 0xffff) mysucessTimer++;
		if (chkTimer_SRP_msec < 0xffff) chkTimer_SRP_msec++;
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


