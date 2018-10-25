
#include <pic18.h>
#include	"main.h"
#include	"mcu_pic18f65k80.h"
#include	"initial.h"
#include	"ad_conversion.h"
#include	"system\system.h"
#include	"loader_45k80\loader_45k80_main.h"
#include    "pwm.h"
#include    "can\my_can.h"

#define MAX_GATE_min_voltage	240 // ��ũ 0 ����, ���� 0
#define MIN_GATE_max_voltage	60

enum {
	LED_ON	= 0,
	LED_OFF = 1
};

#define MAX_CH 5
uint8_t bufZSU_use_not[ZSU_CH_MAX];

volatile uint16_t db_ldrSetSRPMAX;
volatile uint16_t db_ldrSetSRPMIN;


uint8_t rx_db_enableSet[MAX_CH];
uint16_t dutycycleCompared[MAX_CH];

uint16_t not_rx_temp_chkTimer; // �µ� ���� ��� ���� ���� üũ Ÿ�̸� ���� 09-07
bool b_temp_comm_not;
uint16_t not_rx_pannel_chkTimer; // �ǳ� ���� ��� ���� ���� üũ Ÿ�̸� ���� 09-07
bool b_pannel_comm_not;
bool bef_b_pannel_comm_not;
bool b_pannel_comm_break_flag; // �ǳ� ����� �� ���� �Ǵ� ���� !! => ������ off�Ѱſ� ���� ȿ��
bool b_pannel_comm_break_flag_ch[5] = {0,};

uint8_t nRunStep;

enum {
	STEP_CHKING,
	STEP_ERROR,
	STEP_GOOD
};



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
uint16_t zsu_ch0_ch7_analog[8]; // zsu �Ƴ��α� ä�� 8�� ���� ��



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
	�Ƴ��α� �Է°� ��� �µ� ���� �������ش�.
	�Է� 		: ���� �Ƴ��α� �Է� ��
	return  : ���� ���� �µ� �� ex) 30 ��
*/

	return 59;
}


uint16_t getAnalogTo(uint16_t temp) {
/*
	������ ������ �µ� ���� ���ؼ� �����Ŀ� �Ƴ��α� ������ �����ش�.

	return : �Ƴ��α� (�µ�)
*/

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
		// ����� ���� �µ� �Ƴ��α� ����
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
	�µ� ���� ����
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

	// frist enable !! ������
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

// pwm enable/disable ������ ���??
uint8_t whatChEnDisNone_byPannel(uint8_t ch) {
	if (b_pannel_comm_break_flag_ch[ch]) {
		b_pannel_comm_break_flag_ch[ch] = 0;
		return CH_SET_DIS;
	}
	// �̰�, ����� �ɶ��� �ǹ̰� �ִ�.
	if (isPnannel_comm_good()) {
		// ����� �� �� ��
	    if (rx_db_enableSet[ch] & 0x80) { // <-- @�� �ӽ÷� �ǳ� pwm enable switch
	        return CH_SET_EN;
	    } else if (rx_db_enableSet[ch] & 0x40) {
			return CH_SET_DIS;
	    }
	}

    return CH_SET_NONE;
}


uint8_t getTempShortBreakError(uint16_t temp_mv) {
    if ((temp_mv >= 4900) || b_temp_comm_not) // 99��
        return TERR_BREAK;
    if (temp_mv <= 100)
        return TERR_SHORT;

	return TERR_NONE;
}

void heater_setTempError(uint8_t ch) {
/*
    �µ� ���� �ܼ� �Ǵ� ��Ʈ
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
    rx �����Ͱ� ���� ���������� Ȯ�� ��
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


    // �µ�
    if ((rxBuf[1] == 'T') && (rxBuf[2] == 'S')) {
         return RX_TEMP_INFO;
    }

    // �ǳ�
    if ((rxBuf[1] == 'P') && (rxBuf[2] == 'S')) {
        return RX_PANNEL_INFO;
    }


    // �� ��
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
				// AN0 ����
				scr.nowMicomMainAdSensor = adc_updated_analog_mV[ch]; // ���� �Ƴ��α� ��
				scr.bNowMicomAdSensor_updted = TRUE;
				break;
			case 1:
				// AN1 ���� ���� ��
				scr.nowMicomAdVolume = adc_updated_analog_mV[ch]; // ���� �Ƴ��α� ��
				scr.bNowMicomAdVolume_updted = TRUE;
				break;
			case 2:
				// AN2 ����
				scr.nowMicomAdCurrent = adc_updated_analog_mV[ch]; // ���� �Ƴ��α� ��
				scr.bNowMicomAdCurrent_updted = TRUE;
				break;
			case 3:
				// AN3 ����
				scr.nowMicomAdVoltage = adc_updated_analog_mV[ch]; // ���� �Ƴ��α� ��
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
    // ���� ����
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
    // ���� ����
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
// pwm on ���� ������,
// �Է� ���� �ƹ��͵� �ȵ�� �� ���
	if (isPwmOnState(ch)) {
		if (isInputEmpty(ch)) {
			return 1;
		}
	}
	return 0;
}

uint8_t heater_chkEtcError(uint8_t ch) {
// ���� ���� ����
    // ���� üũ
	if (heater[ch].db_tempShortBreak == 1) { // �ܼ�
        return ERR_BREAK;
    } else if (heater[ch].db_tempShortBreak == 2) { // ��Ʈ
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



// @�� �ٽ� �ڵ� �ؾ� �Ѵ�.
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
    ���� dutycycle, pwm ���� ��� �Լ�
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
// ���� �Է� �Ƴ��α� ���� ���� ������ ��ȯ�Ͽ� ��ȯ
// ex) �Ƴ��α� : ���� = 1000 mv : 100 V
// @@ : �߰��� ���� �Լ��� �־�� �Ѵ�.
// ex) ���� 1V ���, �Ƴ��α� : ���� = 1000 mv : 101V
	uint16_t in_mV = (heater[ch].anlog_correctedVoltage_mV / 10);
	uint16_t correct = heater[ch].userCorrVoltSet;
	uint16_t corr_value;
	corr_value = correct % 100;
	if (correct >= SIGN_POSIT_V_OR_A) {
		// + ����
		return in_mV + corr_value;
	} else {	// - ����
		if (in_mV > corr_value) {
			return in_mV - corr_value;
		}
	}
	return in_mV;
}
uint16_t heater_getRealAmp(uint8_t ch) {
// ���� �Է� �Ƴ��α� ���� ���� ������ ��ȯ�Ͽ� ��ȯ
// ex) �Ƴ��α� : ���� = 1000 mv : 100 V
// @@ : �߰��� ���� �Լ��� �־�� �Ѵ�.
// ex) ���� 1V ���, �Ƴ��α� : ���� = 1000 mv : 101V
	uint16_t in_mV = (heater[ch].anlog_correctedAmp_mV / 10);
	uint16_t correct = heater[ch].userCorrAmpSet;
	uint16_t corr_value;
	corr_value = correct % 100;
	if (correct >= SIGN_POSIT_V_OR_A) {
		// + ����
		return in_mV + corr_value;
	} else {	// - ����
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
		// ��� <-> ���x
		if (b_pannel_comm_not) {
			// ��� ���� �� ����
			// �ǳ� ���� off ������ ���� ����
			// ��� ch�� off �ض�.
			return 1;
		}
	}

	return 0;
}

/*/////////////////////////////////
/* #�νĹ��� �ý���
/*//////////////////////////////////

#define GATE_H		1
#define	GATE_L	0
#define PIN_R_PH_HIGH	1
#define PIN_R_PH_LOW	0

uint16_t pwstartTiemr; // ���� ���� ���� �� ���� ����Ʈ on �Ǵ� ���� ������ �߰� �� �ð� �����̴�.

bool bRzeroTimerStart;
bool bSzeroTimerStart;
bool bTzeroTimerStart;

uint16_t gateRSTDo_time; // ���������� ���� �� �� ���� ������ ���̴�.
// �� ���� �ᱹ�� PWM �� ��Ƽ���� ���ٰ� ���� �ȴ�.
// �̰��� ���������� �� ��ǥ ����/����/���� ���� �����ϸ� �ȴ�.

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
		// ���� Ÿ�̸� ����
		tiemrRzeroTimer++;
		// ���� Ÿ�̸Ӱ� ���� �ð� �Ǹ�, ����Ʈ ���� !
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
		// ���� Ÿ�̸� ����
		tiemrSzeroTimer++;
		// ���� Ÿ�̸Ӱ� ���� �ð� �Ǹ�, ����Ʈ ���� !
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
		// ���� Ÿ�̸� ����
		tiemrTzeroTimer++;
		// ���� Ÿ�̸Ӱ� ���� �ð� �Ǹ�, ����Ʈ ���� !
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


bool getSensorTypeByCh(uint16_t i) {
	// 9���� ��/�� �񱳸� ���� �δ� ���� ��
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


	return 1; // ����, ��(+)
}




// #1022 �׳� ���� ���� �Է� �� �߿� ���� ū ���� �����Ĵ� ������ �������� �ȴ�.
uint16_t getMicomFinalMaxSensor(uint16_t now_main) {
	uint16_t max = now_main;
	uint16_t i;
	// �� 9�� ���� ���� �Է°� �߿� �ִ� ���� ��� ���ؼ�
	// ������ �⺻������ ������ ���� �⺻ ������ �������Ƿ�
	// zsu �� 8���� ���ؼ� �� ���� ������ �ȴ�.
	for (i = 0; i < ZSU_CH_MAX; i++) {
		// ���� ū ���� �̾ƾ� �Ѵ�.
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
	uint16_t goal = (scr.goalSetVoltage_V * 4); // micom ��ǥ ���� 100v -> 4000mV
	uint16_t now = scr.nowMicomAdVoltage; // AN3, micom ���� ����

    if (now > goal) {
        return 1;
    }   return 0;
}
bool isOverCurrent(void) {
	uint16_t goal = (scr.goalSetAmp * 4); // micom ��ǥ ���� (50A : 2500mV ����)
	uint16_t now = scr.nowMicomAdCurrent; // AN3, micom ���� ����

    if (now > goal) {
        return 1;
    }   return 0;
}

// ���� ����
void compareGoalNowVoltage(void) {
	if (scr.bNowMicomAdVoltage_updted) {
		scr.bNowMicomAdVoltage_updted = FALSE;

		if (gateRSTDo_time < MAX_GATE_min_voltage) {
			gateRSTDo_time += 1;
		}
	}
}
// ���� ����
void compareGoalNowCurrent(void) {
	if (scr.bNowMicomAdCurrent_updted) {
		scr.bNowMicomAdCurrent_updted = FALSE;

		if (gateRSTDo_time < MAX_GATE_min_voltage) {
			gateRSTDo_time += 1;
		}
	}
}


uint16_t getMicomGoalSensorVal(uint16_t ldrValue) { // #1025
// src : �δ��� => ��ȯ dest : �����Ĵ� ��
	uint32_t signalNumber[2];
	uint16_t micomVal;
						// +/- ��
	getSignalUserNumXXX(signalNumber, (uint32_t)ldrValue);

	switch (signalNumber[0]) {
		case SIGN_PLUS:
			micomVal = signalNumber[1] + 2500;
			break;
		case SIGN_MINUS:
			micomVal = 2500 - signalNumber[1];
			break;
	}

	return micomVal; // �����Ĵ� mV (���� �� ���´�.)
}


void compareGoalNowSensor(void) {
// ���� ����
	// ������ �ܿ����� �׳� 2500mv ��� ����ġ ����.
	uint16_t goal; // micom��, ��ǥ ���� mV
		// �δ��� ������ ��ǥ ���� +/- �� �̹Ƿ�, ������ �� ������ ��ȯ ������� �Ѵ�.
	uint16_t now; // AN3, micom ���� ����
	// ��ǥ ���� ���� �Է� ���а��� ���������� ���� ���ؼ�
	// RSTGATE ON ������ ������ �����ϰų� �����Ѵ�. (�����ϱ� !)
	if (scr.bNowMicomAdSensor_updted) {
		scr.bNowMicomAdSensor_updted = FALSE;
									// �δ�����
		goal = getMicomGoalSensorVal(scr.goalSetSensor); // ��
		now = getMicomFinalMaxSensor(scr.nowMicomMainAdSensor);

		// #1017 ������ �µ� ���� ó�� �� �Ʒ��� +50/-50 �� �ξ���.
		// �̰��� ���� ��� ���� �׽�Ʈ ��� ���� ��������.
		if (now < (goal-0)) {
			if (gateRSTDo_time < MAX_GATE_min_voltage) {
				gateRSTDo_time += 1;
			}
		} else if (now > (goal+0)) {
			// ������ ���� ���� ��� �Ѵ�.
			if (gateRSTDo_time > MIN_GATE_max_voltage) {
				gateRSTDo_time -= 1;
			}
		}
	}
}

void controlGateTotal(void) {
// ����, ����, ���� ����
	if (isOverVoltage()) {
		compareGoalNowVoltage();
		return;
	} else if (isOverCurrent()) {
		compareGoalNowCurrent();
		return;
	}

	// �� ���� ����
	compareGoalNowSensor();
}


// ���� ���� �ڵ� ���� !
// ��ǥ ����ġ �����ϱ�
// �ʿ��� ��
//		- ��ǥ ����ġ
//		- ���� ���а�
//		- �̸� ���Ͽ� ����Ʈ ON ������ ��ȭ �ֱ�
void autoRGateOnOff(void) {
	if (bRzeroTimerStart) { // ����
		// ���� Ÿ�̸� ����
		tiemrRzeroTimer++;
		// ���� Ÿ�̸Ӱ� ���� �ð� �Ǹ�, ����Ʈ ���� !
		if (tiemrRzeroTimer >= gateRSTDo_time) {
			tiemrRzeroTimer = 0;
			pin_GATE_R_PH = GATE_H;
			bRzeroTimerStart = 0;
			timerRGateHighChk = 0;
		}
	}
}
void autoSGateOnOff(void) {
	if (bSzeroTimerStart) { // ����
		// ���� Ÿ�̸� ����
		tiemrSzeroTimer++;
		// ���� Ÿ�̸Ӱ� ���� �ð� �Ǹ�, ����Ʈ ���� !
		if (tiemrSzeroTimer >= gateRSTDo_time) {
			tiemrSzeroTimer = 0;
			pin_GATE_S_PH = GATE_H;
			bSzeroTimerStart = 0;
			timerSGateHighChk = 0;
		}
	}
}
void autoTGateOnOff(void) {
	if (bTzeroTimerStart) { // ����
		// ���� Ÿ�̸� ����
		tiemrTzeroTimer++;
		// ���� Ÿ�̸Ӱ� ���� �ð� �Ǹ�, ����Ʈ ���� !
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

// #1024 4�ܰ� �׽�Ʈ �˶�
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
	// 46us = 1��
	if (!pin_AUTO) {
		// ���� ������ ���ؼ� gateRSTDo_time ���� ȹ���Ѵ�.
		if (scr.bNowMicomAdVolume_updted) {
			scr.bNowMicomAdVolume_updted = FALSE;
			gateRSTDo_time = test_manualVol();
		}
	} else {
		// PWM ó�� ����Ʈ �����ϴ� �Լ��̴�.
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


// #1025
uint16_t get_micom_SRP_max() {
	return db_ldrSetSRPMAX;
}
uint16_t get_micom_SRP_min() {
	return db_ldrSetSRPMIN;
}

volatile uint16_t d_xxx;
uint8_t isSRPError(void) {
/* 1�ܰ� �˶� �׽�Ʈ ��� �����ϱ�
	* SRP MAX
	* SRP LOW : - ���⿡ ���� ���̹Ƿ� Zinc�϶� �� ���� ���� �Ǵ��Ѵ�.
	* SRP Time : check time
*/
    uint8_t ch = 0;
	uint8_t senseorType[9]; // sensor ���� total 9��  // zinc / cucs
	uint16_t micom_nowIn_sensorJunwi[9]; // sensor ���� total 9 ��  // ���� ���� �Է� �� (����� �б��)
	//---------------------------------------------
	uint16_t micom_SRP_max = get_micom_SRP_max();		// ������ �޴� (��ü)
	uint16_t micom_SRP_min = get_micom_SRP_min(); 		// ������ �޴� (��ü)
//	uint16_t setChkTime_SRP = iF_SRP_time; // ������ �޴�
	uint16_t setChkTime_SRP = 1000; // ������ �޴�
	micom_nowIn_sensorJunwi[ch] = micom_getSensorNowSuwi(ch); // ���� ���� ���� �����Ĵ�

	if (chkTimer_SRP_msec > setChkTime_SRP) {
        d_xxx = 0;
		return STEP_GOOD;
	}


	// ä�� 0���� ���ؼ� (���꺸���� ù��°) #1025
	switch (getSensorTypeByCh(ch)) {
		case TYPE_ZINC:
			if (micom_nowIn_sensorJunwi[0] < micom_SRP_min) {
                d_xxx = 0;
				return STEP_ERROR;
			}
			break;
		case TYPE_CUCUSO4:
			if (micom_nowIn_sensorJunwi[ch] > micom_SRP_max) {
                d_xxx = 0;
				return STEP_ERROR;
			}
			break;
	}

	return STEP_CHKING;
}
bool isSOPError(void) {
	return 0;
}


void processErrorCode(uint8_t errorcode) {

}

enum {
	ERR_NONE,
	ERR_SRP,	// ���� ����
	ERR_SOP,	// �ܼ�
	ERR_AOP,	//
	ERR_ARP,
	ERR_UPR,
	ERR_OPR,
	ERR_FOP // RST ��
};

// #1025
volatile	uint8_t errchk;
void loop_allStepRun() {
	static uint8_t errorCode;

	switch (nRunStep) {
		case 1:
			// ���� �ð� ����
			// sensor�� = type | ���� ������ |
			errchk = isSRPError(); // 0 ~ 7 , 8(main)
			// �ϳ��� ���� �߻��ϸ� !
			if (errchk == STEP_ERROR) {
				// error !
				errorCode = ERR_SRP;

nRunStep = 2;
				break;
			} else if (errchk == STEP_GOOD) {
				// good !
				// ���� X
				errorCode = ERR_NONE;
				nRunStep = 2;
				break;
			} else if (errchk == STEP_CHKING){
				// üŷ ��� ���� ~~
				// ���� ���� X
				errorCode = ERR_NONE;
				break;
			}
			break;
		case 2:
			//if (isSOPError()) {
				errorCode = ERR_SOP;
				break;
			//}
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


// #1025 coding ����

void database_SRP_MAX() {
	uint16_t micom_SRP_max = getMicomGoalSensorVal(iF_SRP_max);
	uint16_t micom_SRP_min = getMicomGoalSensorVal(iF_SRP_min);		// ������ �޴� (��ü)

	// 1. ������ +1000 (-1000)
	// 2. �δ����� 11000 (1000 -1000 = 9000)
	// 3. �����Ĵ�_���� 2500 +1000 = 3500 (2500 -1000 = 1500)

	// #1025
	db_ldrSetSRPMAX = micom_SRP_max;
	db_ldrSetSRPMIN = micom_SRP_min;
}

bool isCoditionContorl() {
	return ( (pin_KEY_POWER == KEY_POWER_ON)
		&& (powerOnReadyDelayTimer > 1000) );
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
	// INT1, INT2 �Է� ����
    initAdc();
    initLoaderUart2();
    //initPwm();
	// can ����
	init_canPort();
    InitCAN();
    ei();
    setExtInterrupt();
    setWatchDockEnable();
    setAdcStartOrStop(1);

	// �µ�/�ǳ�   ��� ���� üũ �� ���� �ʱ�ȭ
	not_rx_temp_chkTimer = 0;
	b_temp_comm_not = 0;
	not_rx_pannel_chkTimer = 0;
	b_pannel_comm_not = 0;
	bef_b_pannel_comm_not = 0;
	b_pannel_comm_break_flag = 0;

	// �ʱ�ȭ
	gateRSTDo_time = MAX_GATE_min_voltage;
	bRzeroTimerStart = 0;
	bSzeroTimerStart = 0;
	bTzeroTimerStart = 0;
	tiemrRzeroTimer = 0;
	tiemrSzeroTimer = 0;
	tiemrTzeroTimer = 0;

	pwstartTiemr = 0;

	pin_RUN_LED = LED_OFF; // 1 = off
	powerOnReadyDelayTimer = 0;
    while (1) {
        unsigned int i;
        uint8_t ch;

        CLRWDT();
		oscillator_fail_loop();

// ad conversion
// * 10�� ä���� �Է� mV �� �о� ���°� �����̴�.
        adc_loop();

// ���
        can_tx_loop();
	    rxtx_loop();

// pwm ���
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

// #�νĹ��� �ý��� -----------------------------------------

		// ����/����/����   �δ� ��ǥ ������ ��������       ---Goal Set Volt/Amp/Sensor
		scr.goalSetVoltage_V = iF_scr_goalVoltage;
		scr.goalSetAmp = iF_scr_goalCurrent;
		scr.goalSetSensor = iF_scr_goalSensor;

		// ���� ������ �� 4��(����/����/����/����)�� �Ƴ��α� ä�� �� ȹ���ϱ� (�����ϱ�)
		for (ch = 0; ch < ADC_CH_MAX; ch++) {
			scr_micom_setNowInVoltage_mV(ch);
		}

		// ZSU use/not_use ���� �� �����ϱ�
		bufZSU_use_not[0] = cF_ch0_use;
		bufZSU_use_not[1] = cF_ch1_use;
		bufZSU_use_not[2] = cF_ch2_use;
		bufZSU_use_not[3] = cF_ch3_use;
		bufZSU_use_not[4] = cF_ch4_use;
		bufZSU_use_not[5] = cF_ch5_use;
		bufZSU_use_not[6] = cF_ch6_use;
		bufZSU_use_not[7] = cF_ch7_use;

		// #1024 4�ܰ� �׽�Ʈ �˶� ��� �����ϱ�
		micom_saveTotal8sensorNowValue();

		// #1025 SRP MAX���� �����Ĵ� ������ ��ȯ�Ͽ� �����ϱ� (�����ͺ��̽�ȭ)
		database_SRP_MAX();


		if (isJustNowPowerOn()) {
			powerOnReadyDelayTimer = 0;
		}

		if (isCoditionContorl()) {
			// ��ü ���� ����
			loop_allStepRun(nRunStep);
		} else {
			initSystem();
			nRunStep = 1;
			chkTimer_SRP_msec = 0;
		}
    }
}

void interrupt isr(void) {
    static unsigned int timer_msec = 0;
	uint8_t ch;

	// ZERO RST �Է� ���ͷ�Ʈ (��� ����) üũ
	if(INT0IF && INT0IE){
  		INT0IF = 0;
		bRzeroTimerStart = 1; // ���� Ÿ�̸� ������ ���� ���� RST ���� ����
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

	// RST ����Ʈ ON �� Ÿ�̸�1
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
		// ����/�ڵ� on�Ǹ� ������ off �ؾ� �Ѵ�.
		offRSTGATEWhenOn();
	}

    if (TMR0IF) {
        TMR0IF = 0;
        TMR0L = MSEC_L;
        TMR0H = MSEC_H;
        loader_msecTimer();
        timer_msec++;
		if (pwstartTiemr < 0xffff) pwstartTiemr++;
        if (timer_msec >= 1000) {
            timer_msec = 0;
			//pin_RUN_LED = ~pin_RUN_LED;
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

		if (chkTimer_SRP_msec < 0xffff) chkTimer_SRP_msec++;
		if (powerOnReadyDelayTimer < 0xffff) powerOnReadyDelayTimer++;
    }


    if (ADIF) {
        unsigned int ad;

        ad = ADRES;
        if (ad > 5500) ad = 0; // 07-17 ���̳ʽ� �� ���� ��츦 ����ؼ�
         set_input_ad(ad);
        ADIF = 0;
        b_conversion_ad = 0;
    }


    loader_uart_interrupt();


    if (PIR5 > 0) {
        can_rxInterrupt();
    }
}


