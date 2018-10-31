
#include <pic18.h>
#include	"main.h"
#include	"mcu_pic18f65k80.h"
#include	"initial.h"
#include	"ad_conversion.h"
#include	"system\system.h"
#include	"loader_45k80\loader_45k80_main.h"
#include    "pwm.h"
#include    "can\my_can.h"

#define MAX_GATE_zero_voltage	240 // ��ũ 0 ����, ���� 0
#define MIN_GATE_max_voltage	60

uint16_t db_corrected_final_sensor_0_8_micomMV[9];
uint16_t db_sunsu_sensor_0_8_micomMV[9];


extern uint16_t getCorrectedNowIn_micomMV(uint16_t nowIn_mV, uint16_t correct_mV);
extern void micom_saveTotal9SensorNowIn_mV(void);

uint8_t db_finalErrorCode;
bool bAlarmRyOn_Off;
extern bool isSystemOffError(uint8_t errcode);
enum {
	OFF = 0,
	ON = 1
}

enum {
	LED_ON	= 0,
	LED_OFF = 1
};


// MAX Voltage : �ִ� �Ѱ� ���� (���� ����)
uint16_t getVoltage_micomMV_FromUserData(uint16_t src) {
	return (src * 4);
}
uint16_t getVoltage_RealData_FromMicomMV(uint16_t src) {
	return (src / 4);
}

// MAX Amp : �ִ� �Ѱ� ���� (���� ����)
uint16_t getAmp_micomMV_FromUserData(uint16_t goalAmp_user,
										uint8_t ampType){
/*
	user ldr set ���� �ִ� �Ѱ� ���� => micom �� mV ���� ������ ��ȯ�Ͽ� ��ȯ !
*/
	uint8_t ampTypeSet = ampType;
	uint16_t user_ampMaxSet = goalAmp_user; // 5A : 50 * 100 = 5000
	switch (ampTypeSet) {
		case 0: // 5A : 2.5��
			return (user_ampMaxSet * 40);
		case 1: // 10A : 5��
			return (user_ampMaxSet * 20);
		case 2: // 15A : 7.5��
			return (user_ampMaxSet * 40) / 3;
		case 3: // 30A : 15��
			return (user_ampMaxSet * 20) / 3;
		case 4: // 50A : 25��
			return (user_ampMaxSet * 4);
		case 5: // 75A : 37.5��
			return (user_ampMaxSet * 8) / 3;
		case 6: // 100A : 50��
			return (user_ampMaxSet * 2);
		case 7: // 150A : 75��
			return (user_ampMaxSet * 4) / 3;
		case 8: // 200A : 100��
			return (user_ampMaxSet * 1);
		case 9: // 250A : 125��
			return (user_ampMaxSet * 4) / 5;
		case 10: // 300A : 150��
			return (user_ampMaxSet * 2) / 3;

	}
    return 0;
}
uint16_t getAmp_RealData_FromMicomMV(uint16_t src,
										uint8_t ampType){
/*
	user ldr set ���� �ִ� �Ѱ� ���� => micom �� mV ���� ������ ��ȯ�Ͽ� ��ȯ !
*/
	uint8_t ampTypeSet = ampType;
	uint16_t micmo_mV = src; // 5A : 50 * 100 = 5000
	switch (ampTypeSet) {
		case 0: // 5A : 2.5��
			return (micmo_mV / 40); // => 50 (��, 5.0)
		case 1: // 10A : 5��
			return (micmo_mV / 20); // => 100 (��, 10.0)
		case 2: // 15A : 7.5��
			return (micmo_mV * 3) / 40;
		case 3: // 30A : 15��
			return (micmo_mV * 3) / 20;
		case 4: // 50A : 25��
			return (micmo_mV / 4);
		case 5: // 75A : 37.5��
			return (micmo_mV * 3) / 8;
		case 6: // 100A : 50��
			return (micmo_mV / 2);
		case 7: // 150A : 75��
			return (micmo_mV * 3) / 4;
		case 8: // 200A : 100��
			return (micmo_mV / 1);
		case 9: // 250A : 125��
			return (micmo_mV * 5) / 4;
		case 10: // 300A : 150��
			return (micmo_mV * 3) / 2;
	}
    return 0;
}



enum {
	ET_DISABLE = 0,
	ET_ENABLE = 1,
} ErrorTestEnableDisable;


#define MAX_CH 5
uint8_t bufZSU_use_not[ZSU_CH_MAX];

volatile uint16_t db_ldrSetSRPMAX;
volatile uint16_t db_ldrSetSRPMIN;
volatile uint16_t db_ldrSetSOPMAX;
volatile uint16_t db_ldrSetSOPMIN;



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
	STEP_DONE
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

enum {
	ERR_NONE = 0,
	ERR_FOP, // RST ��
	ERR_SRP, // 1step : ���� ����
	ERR_SOP, // 2step : �ܼ�
	ERR_AOP, // 3step
	ERR_ARP, // 4step
	ERR_UPR,
	ERR_OPR,
};



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





// -------------------------------------------
// --- sensor use/nouse ---------------------------
bool isSensorUseNo(uint8_t ch) {

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
        case 5:
            return cF_ch5_use;
        case 6:
            return cF_ch6_use;
        case 7:
            return cF_ch7_use;
        case 8:
            return cF_ch8_use;
    }
    return 0;
}


void initPwm(void) {

//    InitPwmx(1);
//    InitPwmx(2);
//    InitPwmx(3);
//    InitPwmx(4);
//    InitPwmx(5);
}

void offPwmAll(void) {
//    DutyCyclex[0]	= 0;
//    DutyCyclex[1]	= 0;
//    DutyCyclex[2]	= 0;
//    DutyCyclex[3]	= 0;
//    DutyCyclex[4]	= 0;
//
//    OutPWMx(1);
//    OutPWMx(2);
//    OutPWMx(3);
//    OutPWMx(4);
//    OutPWMx(5);
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
				scr.nowMainAdSensor_micom_mV = adc_updated_analog_mV[ch]; // ���� �Ƴ��α� ��
				scr.bNowAdSensor_micom_updted = TRUE;
				micom_saveTotal9SensorNowIn_mV();
				break;
			case 1:
				// AN1 ���� ���� ��
				scr.nowAdVolume_micom_mV = adc_updated_analog_mV[ch]; // ���� �Ƴ��α� ��
				scr.bNowAdVolume_micom_updted = TRUE;
				break;
			case 2:
				// AN2 ����
				scr.nowAdAmp_micom_mV = adc_updated_analog_mV[ch]; // ���� �Ƴ��α� ��
				scr.bNowAdCurrent_micom_updted = TRUE;
				break;
			case 3:
				// AN3 ����
				scr.nowVoltage_micom_mV = adc_updated_analog_mV[ch]; // ���� �Ƴ��α� ��
				scr.bNowAdVoltage_micom_updted = TRUE;
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




void writeFlash(uint16_t addressNum, uint16_t ldrdata) {
    uint16_t BlockPt;

    BlockPt = (addressNum / FLASH_ONE_BLOCK_SIZE);
    FlashBlockRd(BlockPt);
    cSR_ByteData(addressNum) = ldrdata;

    FlashBlockWr(BlockPt);
}







void whenRxEnFormPan_write(uint8_t ch) {

	if (heater[ch].bChEnStateFromPannel != heater[ch].db_bFlashChEnable_state) {


	}

}





uint8_t useState[5];


void heater_setChEnableDisable(uint8_t ch) {



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
	uint16_t volAnalog = scr.nowAdVolume_micom_mV; // 6~4994
	return (180 - (volAnalog / 28) + 60);
}


uint16_t timerRGateHighChk;
uint16_t timerSGateHighChk;
uint16_t timerTGateHighChk;



uint16_t tiemrRzeroTimer;
uint16_t tiemrSzeroTimer;
uint16_t tiemrTzeroTimer;

uint16_t RZeroXChekTimer;
uint16_t SZeroXChekTimer;
uint16_t TZeroXChekTimer;


uint16_t getCorrectedLdrSet_ch0_ch8(uint8_t ch) {

	switch (ch) {
		case 0:
			return iF_correct_ch0_mV;
		case 1:
			return iF_correct_ch1_mV;
		case 2:
			return iF_correct_ch2_mV;
		case 3:
			return iF_correct_ch3_mV;
		case 4:
			return iF_correct_ch4_mV;
		case 5:
			return iF_correct_ch5_mV;
		case 6:
			return iF_correct_ch6_mV;
		case 7:
			return iF_correct_ch7_mV;
		case 8:
			return iF_correct_ch8_mV;
	}
	return 0;
}


void micom_saveTotal9SensorNowIn_mV(void) {
	uint8_t ch;
	for (ch = 0; ch < 8; ch++) {
		db_corrected_final_sensor_0_8_micomMV[ch] =
			getCorrectedNowIn_micomMV(zsu_ch0_ch7_analog[ch], getCorrectedLdrSet_ch0_ch8(ch));
		db_sunsu_sensor_0_8_micomMV[ch] = zsu_ch0_ch7_analog[ch];
	}
	db_corrected_final_sensor_0_8_micomMV[ch] =
		getCorrectedNowIn_micomMV(scr.nowMainAdSensor_micom_mV, getCorrectedLdrSet_ch0_ch8(ch));
	db_sunsu_sensor_0_8_micomMV[ch] = scr.nowMainAdSensor_micom_mV;
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




uint16_t getFinalOneTopMaxSensor_micom_mV(void) {
// ������ ���̴�.
	uint16_t max;
	uint8_t ch;

	max = 0;
	for (ch = 0; ch < 9; ch++) {
		if (!isSensorUseNo(ch)) continue;
		if (db_corrected_final_sensor_0_8_micomMV[ch] > max) {
			max = db_corrected_final_sensor_0_8_micomMV[ch];
		}
	}
	return max;
}







uint16_t getFinalOneLowMinSensor_micom_mV(void) {
// ������ ���̴�.
	uint16_t min;
	uint8_t ch;

	min = 0xffff;
	for (ch = 0; ch < 9; ch++) {
		if (!isSensorUseNo(ch)) continue;
		if (db_corrected_final_sensor_0_8_micomMV[ch] < min) {
			min = db_corrected_final_sensor_0_8_micomMV[ch];
		}
	}
	return min;

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

// ���� ���� Voltage User
uint16_t getCorrectedNowIn_micomMV_voltage(uint16_t nowIn_mV,
										uint16_t correct_user) {
	uint32_t signalNumber[2];
	uint16_t result, correct_mV;
	getSignNumberByLdrDigit(signalNumber, correct_user);
	correct_mV = getVoltage_micomMV_FromUserData(signalNumber[1]); // ���� ����, ���� ������

	switch (signalNumber[0]) {
		case SIGN_PLUS: // +
			// ����) +1 �̸�,
			// 		���� �Է� �����ٰ� + 1�� �ϸ� ����
			result = nowIn_mV + correct_mV;
			if (result > 5000) result = 5000;
			return result;
		case SIGN_MINUS:
			if (correct_mV <= nowIn_mV) {
				result = nowIn_mV - correct_mV;
			} else {
				result = 0;
			}
			return result;
	}
	return 5000;
}
uint16_t getCorrectedNowIn_micomMV(uint16_t nowIn_mV, uint16_t correct_user) {
	uint32_t signalNumber[2];
	uint16_t result, correct_mV;
	getSignNumberByLdrDigit(signalNumber, correct_user);
	correct_mV = signalNumber[1]; // ���� �϶��� �̷��� �Ѵ�.

	switch (signalNumber[0]) {
		case SIGN_PLUS: // +
			// ����) +1 �̸�,
			//		���� �Է� �����ٰ� + 1�� �ϸ� ����
			result = nowIn_mV + correct_mV;
			if (result > 5000) result = 5000;
			return result;
		case SIGN_MINUS:
			if (correct_mV <= nowIn_mV) {
				result = nowIn_mV - correct_mV;
			} else {
				result = 0;
			}
			return result;
	}
	return 5000;
}
// ���� ���� Amp User
uint16_t getCorrectedNowIn_micomMV_Amp(uint16_t nowIn_mV,
										uint16_t correct_user,
										uint8_t ampType) {
	uint32_t signalNumber[2];
	uint16_t result, correct_mV;
	getSignNumberByLdrDigit(signalNumber, correct_user);
	correct_mV = getAmp_micomMV_FromUserData(signalNumber[1], ampType);

	switch (signalNumber[0]) {
		case SIGN_PLUS: // +
			// ����) +1 �̸�,
			// 		���� �Է� �����ٰ� + 1�� �ϸ� ����
			result = nowIn_mV + correct_mV;
			if (result > 5000) result = 5000;
			return result;
		case SIGN_MINUS:
			if (correct_mV <= nowIn_mV) {
				result = nowIn_mV - correct_mV;
			} else {
				result = 0;
			}
			return result;
	}
	return 5000;
}




bool isOverVoltage_micomMV(void) {
	uint16_t limit = getVoltage_micomMV_FromUserData(iF_scr_goalVoltage); // micom ��ǥ ���� 100v -> 4000mV
	uint16_t now = getCorrectedNowIn_micomMV_voltage(scr.nowVoltage_micom_mV,
													iF_correct_V_user); // AN3, micom ���� ����

    if (now > limit) {
        return 1;
    }   return 0;
}
bool isOverCurrent(void) {
	uint16_t limit = getAmp_micomMV_FromUserData(iF_scr_goalAmp, cF_amp_type); // micom ��ǥ ���� (50A : 2500mV ����)
	uint16_t now = getCorrectedNowIn_micomMV_Amp(scr.nowAdAmp_micom_mV, iF_correct_A_user,
												cF_amp_type); // AN3, micom ���� ����

    if (now > limit) {
        return 1;
    }   return 0;
}

// ���� ����
void compareGoalNowVoltage(void) {
	if (scr.bNowAdVoltage_micom_updted) {
		scr.bNowAdVoltage_micom_updted = FALSE;

		if (gateRSTDo_time < MAX_GATE_zero_voltage) {
			gateRSTDo_time += 1;
		}
	}
}
// ���� ����
void compareGoalNowCurrent(void) {
	if (scr.bNowAdCurrent_micom_updted) {
		scr.bNowAdCurrent_micom_updted = FALSE;

		if (gateRSTDo_time < MAX_GATE_zero_voltage) {
			gateRSTDo_time += 1;
		}
	}
}


uint16_t getGoalSensorSetVal_micom_mV(uint16_t ldrValue) { // #1025
// src : �δ��� => ��ȯ dest : �����Ĵ� ��
	uint32_t signalNumber[2];
	uint16_t micomVal;
						// +/- ��
	getSignNumberByLdrDigit(signalNumber, (uint32_t)ldrValue);

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
// ���� ���� �Լ�
	// ������ �ܿ����� �׳� 2500mv ��� ����ġ ����.
	uint16_t goal; // micom��, ��ǥ ���� mV
		// �δ��� ������ ��ǥ ���� +/- �� �̹Ƿ�, ������ �� ������ ��ȯ ������� �Ѵ�.
	uint16_t now_max; // AN3, micom ���� ����
	// ��ǥ ���� ���� �Է� ���а��� ���������� ���� ���ؼ�
	// RSTGATE ON ������ ������ �����ϰų� �����Ѵ�. (�����ϱ� !)
	if (scr.bNowAdSensor_micom_updted) {
		scr.bNowAdSensor_micom_updted = FALSE;
									// �δ�����
		goal = getGoalSensorSetVal_micom_mV(iF_scr_goalSensor); // ��
		now_max = getFinalOneTopMaxSensor_micom_mV();

		// #1017 ������ �µ� ���� ó�� �� �Ʒ��� +50/-50 �� �ξ���.
		// �̰��� ���� ��� ���� �׽�Ʈ ��� ���� ��������.
		if (now_max < (goal-0)) {
			if (gateRSTDo_time < MAX_GATE_zero_voltage) {
				gateRSTDo_time += 1;
			}
		} else if (now_max > (goal+0)) {
			// ������ ���� ���� ��� �Ѵ�.
			if (gateRSTDo_time > MIN_GATE_max_voltage) {
				gateRSTDo_time -= 1;
			}
		}
	}
}

void controlGateTotal(void) {
// ����, ����, ���� ����
	if (isOverVoltage_micomMV()) {
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





void controlSensorJuWi(void) {
	// 46us = 1��
	if (!pin_AUTO) {
		// ���� ������ ���ؼ� gateRSTDo_time ���� ȹ���Ѵ�.
		if (scr.bNowAdVolume_micom_updted) {
			scr.bNowAdVolume_micom_updted = FALSE;
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

uint16_t get_micom_SRP_max() {
	return db_ldrSetSRPMAX;
}
uint16_t get_micom_SRP_min() {
	return db_ldrSetSRPMIN;
}


uint16_t get_micom_SOP_max() {
	return db_ldrSetSOPMAX;
}
uint16_t get_micom_SOP_min() {
	return db_ldrSetSOPMIN;
}


uint8_t isSRPError(void) {
/* 1�ܰ� �˶� �׽�Ʈ ��� �����ϱ�
	* SRP MAX
	* SRP LOW : - ���⿡ ���� ���̹Ƿ� Zinc�϶� �� ���� ���� �Ǵ��Ѵ�.
	* SRP Time : check time
*/
    uint8_t ch; // #1020
	uint8_t senseorType[9]; // sensor ���� total 9��  // zinc / cucs
	uint16_t micom_nowIn_sensorJunwi[9]; // sensor ���� total 9 ��  // ���� ���� �Է� �� (����� �б��)
	//---------------------------------------------
	uint16_t micom_SRP_max = get_micom_SRP_max();		// ������ �޴� (��ü)
	uint16_t micom_SRP_min = get_micom_SRP_min(); 		// ������ �޴� (��ü)
	uint16_t setChkTime_SRP = iF_SRP_time; // ������ �޴�
	if (cF_1SRP_en == ET_DISABLE) return STEP_DONE;
	//---------------------------------------------
	UserSystemStatus = M_1ST_SRP_CHK;

	if (chkTimer_commomError_msec > setChkTime_SRP) {
		return STEP_DONE;
	}

	for (ch = 0; ch < 9; ch++) {
		if (!isSensorUseNo(ch)) continue;
		micom_nowIn_sensorJunwi[ch] = db_corrected_final_sensor_0_8_micomMV[ch]; // ���� ���� ���� �����Ĵ�

		switch (getSensorTypeByCh(ch)) {
			case TYPE_ZINC:
				if (micom_nowIn_sensorJunwi[ch] < micom_SRP_min) {
					return STEP_ERROR;
				}
				break;
			case TYPE_CUCUSO4:
				if (micom_nowIn_sensorJunwi[ch] > micom_SRP_max) {
					return STEP_ERROR;
				}
				break;
		}
	}

	return STEP_CHKING;
}


uint8_t isSOPError(void) {
/* 2�ܰ� �˶� �׽�Ʈ ��� �����ϱ�
	* SOP MAX
	* SOP LOW : - ���⿡ ���� ���̹Ƿ� Zinc�϶� �� ���� ���� �Ǵ��Ѵ�.
	* SOP Time : check time
*/
    uint8_t ch; // #1020
	//----------------------------
	uint16_t micom_nowIn_sensorJunwi[9]; // ���� ���� ��
	//---------------------------------------------
	uint16_t micom_SOP_max = get_micom_SOP_max();		// ������ �޴� (��ü)
	uint16_t micom_SOP_min = get_micom_SOP_min(); 		// ������ �޴� (��ü)
	uint16_t setChkTime_SOP = iF_SOP_time; // ������ �޴�
	if (cF_2SOP_en == ET_DISABLE) return STEP_DONE;
	// ------------------------------------------------------
	UserSystemStatus = M_2ST_SOP_CHK;
	if (chkTimer_commomError_msec > setChkTime_SOP) {
		return STEP_DONE;
	}

	for (ch = 0; ch < 9; ch++) {
		if (!isSensorUseNo(ch)) continue;
		micom_nowIn_sensorJunwi[ch] = db_corrected_final_sensor_0_8_micomMV[ch]; // ���� ���� ���� (�����Ĵ�)

		// ä�� 0���� ���ؼ� (���꺸���� ù��°) #1025
		// 2300 <  ���� ���� < 2700 ���� => ���� �ܼ�
		if ( (micom_nowIn_sensorJunwi[ch] < micom_SOP_max)
			&& (micom_nowIn_sensorJunwi[ch] > micom_SOP_min) ){
			// �ܼ� ���� !
			return STEP_ERROR;
		}
	}

	return STEP_CHKING;
}

uint16_t getGateRstDoTimeByDuty(uint16_t duty) {
	uint16_t basu, i;
    basu =	(MAX_GATE_zero_voltage - MIN_GATE_max_voltage) * 10 / 99;
	i = (duty * basu / 10);
	return MAX_GATE_zero_voltage - i;
}


uint8_t isAOPError(void) {
/* 3�ܰ� AOP
	* AOP Duty
	* AOP Time

	SCR�� ����� �����ð� ��ŭ �Ѵ�.
		* ���� AOP Duty ��ŭ�� SCR ����� ��������.
			+ ����) 50 Duty�̸�, 90��/180�� �ش� SCR ����� ��������.
			+ ���� ������ ���� ��ġ�̴�.
		* ���а��� ������ 50%�̻�
		* ������ ������ 10%���Ϸ� ����Ǹ�
		* Anode �ܼ����� �����ϰ� �溸�� �߻��Ѵ�.

		�� ���� ����/����    : �δ��� ������ �ִ� �Ѱ� ���� ��
*/
	uint16_t duty = iF_AOP_duty;		// ������ �޴� (��ü)
	uint16_t time = iF_AOP_time; // ������ �޴�
	// ���� ������ 50%
	uint16_t rated50_Voltage_mV = (getVoltage_micomMV_FromUserData(iF_scr_goalVoltage)) / 2; // micom�� mV������ �����ؾ� �Ѵ�. <<
	// ���� ������ 10%
	uint16_t reted10_Amp_mV = (getAmp_micomMV_FromUserData(iF_scr_goalAmp, cF_amp_type) / 10); // micom�� mV������ �����ؾ� �Ѵ�.
	// ���� ���� ��
	uint16_t nowVoltage_mV = scr.nowVoltage_micom_mV; // AN3, micom ���� ����
	// ���� ���� ��
	uint16_t nowAmp_mV = scr.nowAdAmp_micom_mV; // AN2, micom ���� ����
	if (cF_3AOP_en == ET_DISABLE) return STEP_DONE;
	// ------------------------------------------------------

	UserSystemStatus = M_3ST_AOP_CHK;
	// duty ���
	// duty // 0 ~ 99 %
	gateRSTDo_time = getGateRstDoTimeByDuty(duty);


	if (chkTimer_commomError_msec > time) {
		gateRSTDo_time = MAX_GATE_zero_voltage;
		return STEP_DONE;
	}

	// ���а��� ���� 50% �̻�
	if (nowVoltage_mV > rated50_Voltage_mV) {
		// ������ 10% ����
		if (nowAmp_mV < reted10_Amp_mV) {
			// ���� !
			return STEP_ERROR;
		}
	}

	return STEP_CHKING;
}

uint8_t isARPError(void) {
/* 4�ܰ� ARP
	* AOP Duty
	* AOP Time

	SCR�� ����� �����ð� ��ŭ �Ѵ�.
	* �̶�, ���� ������ �����ؾ� ����
	* �׷��� �ʰ� ���� ������ �ö󰡸� ���� !
	* ó�� ���� ������ ���� ���� �� ��� + (����) 150mV �ö� ������ �ϳ��� �߰ߵǸ� ���� !
*/
	uint16_t ch; // #1020
	uint16_t now_mV[9];
	uint16_t duty = iF_ARP_duty;		// ������ �޴� (��ü)
	uint16_t time = iF_ARP_time; // ������ �޴�

	static uint16_t start_mV[9]; // ���������� ���� �ȵ��� ������ static
	// ���� ���� �����Ĵ� mV ��
	if (cF_4ARP_en == ET_DISABLE) return STEP_DONE;
	// -------------------------------------------------------------------
	UserSystemStatus = M_4ST_ARP_CHK;

	if (chkTimer_commomError_msec > time) {
		gateRSTDo_time = MAX_GATE_zero_voltage;
		return STEP_DONE;
	}

	for (ch = 0; ch < 9; ch++) {
		if (!isSensorUseNo(ch)) continue;
		now_mV[ch] = db_corrected_final_sensor_0_8_micomMV[ch]; // ���� ���� ���� �����Ĵ�
		// scr ��� !
		gateRSTDo_time = getGateRstDoTimeByDuty(duty);

		if (chkTimer_commomError_msec <= 2) {
			start_mV[ch] = now_mV[ch];
		} else {
			// ���� + 150mV ������ �ö󰬴��� ���� üũ
			if (now_mV[ch] > (start_mV[ch] + 150)) {
				return STEP_ERROR;
			}
		}
	}

	return STEP_CHKING;
}

uint16_t tiemr_30UjiChkUpper_msec;
bool bUPRstate;
uint8_t isUPRWarning(void) {
/* 5�ܰ� UPR set
	* 30�� ���� üũ�� Ÿ�̸� ����
		+ ���ذ��� �Ǹ� �ʱ�ȭ
	* ���� ���� ���� �� mV
	* ���� ��ǥ ���� ��
*/
	uint16_t set = getGoalSensorSetVal_micom_mV(iF_UPR_set); // ��
	uint16_t now_max = getFinalOneTopMaxSensor_micom_mV();
	if (cF_UPR_en == ET_DISABLE) return STEP_DONE;
	// -------------------------------------------------------------------

	if (now_max > set) {
		bUPRstate = 1;
	} else {
		bUPRstate = 0;
		tiemr_30UjiChkUpper_msec = 0;
	}

	if (tiemr_30UjiChkUpper_msec > 30000) {
		return STEP_ERROR;
	}

	return STEP_CHKING;
}

uint8_t isOPRError(void) {
/* 5�ܰ� OPR set
	* ���� ���� ���� ��
	* ���� ���� ���� �� mV
*/
	uint16_t set = getGoalSensorSetVal_micom_mV(iF_OPR_set); // ��
	uint16_t now = getFinalOneLowMinSensor_micom_mV();
	if (cF_OPR_en == ET_DISABLE) return STEP_DONE;
	// -------------------------------------------------------------------

	if (now < set) {
		return STEP_ERROR;
	}
	return STEP_CHKING;
}


uint8_t isFopErrorChk(void) {
/*
	�⺻���� R/S/T �Է� ������ ��� ������ üũ �ؾ� �Ѵ�.
		���� �ð� ���� �ȵ�� ������ üũ�ؾ� �Ѵ�.
		����, �� R/S/T�� ���� üũ�� Ÿ�̸Ӱ� �־�� �Ѵ�.
	�ΰ� �̻� �� ������ ��, 1���� ������ �����̴�.

*/
	uint16_t cnt;

	if (cF_FOP_en == ET_DISABLE) return 0;

	cnt = 0;
	if (RZeroXChekTimer > 100) {
		cnt++;
	}
	if (SZeroXChekTimer > 100) {
		cnt++;
	}
	if (TZeroXChekTimer > 100) {
		cnt++;
	}

	if (cnt >= 2) {
		return 1; // ERROR !
	}
	return 0;
}


uint8_t allStepRun_5step() {
	uint8_t bState;

	// FOP ������ ��� �ý����� �⺻ �����̹Ƿ�
	// ��� �˻��ϴ� ������ ����. !
	if (isFopErrorChk()) return ERR_FOP;

	switch (nRunStep) {
		case 1: // SRP
			bState = isSRPError();
			if (bState == STEP_ERROR) {
				return ERR_SRP; // ���, ��ȯ
			} else if (bState == STEP_CHKING) {
				return ERR_NONE;
			} else if (bState == STEP_DONE) {
				nRunStep = 2; // <<< next
				chkTimer_commomError_msec = 0;
				return ERR_NONE;
			}
			break;

		case 2: // SOP
			bState = isSOPError();
			if (bState == STEP_ERROR) {
				return ERR_SOP; // ���, ��ȯ
			} else if (bState == STEP_CHKING) {
				return ERR_NONE;
			} else if (bState == STEP_DONE) {
				nRunStep = 3; // <<< next
				chkTimer_commomError_msec = 0;
				return ERR_NONE;
			}
			break;

		case 3:
			bState = isAOPError();
			if (bState == STEP_ERROR) {
				return ERR_AOP; // ���, ��ȯ
			} else if (bState == STEP_CHKING) {
				return ERR_NONE;
			} else if (bState == STEP_DONE) {
				nRunStep = 4; // <<< next
				chkTimer_commomError_msec = 0;
				return ERR_NONE;
			}
			break;

		case 4:
			bState = isARPError();
			if (bState == STEP_ERROR) {
				return ERR_ARP; // ���, ��ȯ
			} else if (bState == STEP_CHKING) {
				return ERR_NONE;
			} else if (bState == STEP_DONE) {
				nRunStep = 5; // <<< next
				chkTimer_commomError_msec = 0;
				return ERR_NONE;
			}
			break;

		case 5:
			UserSystemStatus = M_RUNNING;
			pin_RY_RUN = RY_ON;
			controlSensorJuWi();

			bState = isOPRError();
			if (bState == STEP_ERROR) {
				return ERR_OPR;
			}

			bState = isUPRWarning(); // ��� ����
			if (bState == STEP_ERROR) {
				return ERR_UPR;
			}

			return ERR_NONE;

		default:
			break;
	}
	return ERR_NONE;
}




// #1025 coding ����

void database_SRPSOPARLRM() {
	uint16_t micom_SRP_max = getGoalSensorSetVal_micom_mV(iF_SRP_max);
	uint16_t micom_SRP_min = getGoalSensorSetVal_micom_mV(iF_SRP_min);		// ������ �޴� (��ü)
	uint16_t micom_SOP_max = getGoalSensorSetVal_micom_mV(iF_SOP_max);
	uint16_t micom_SOP_min = getGoalSensorSetVal_micom_mV(iF_SOP_min);		// ������ �޴� (��ü)

	// 1. ������ +1000 (-1000)
	// 2. �δ����� 11000 (1000 -1000 = 9000)
	// 3. �����Ĵ�_���� 2500 +1000 = 3500 (2500 -1000 = 1500)

	// #1025
	db_ldrSetSRPMAX = micom_SRP_max;
	db_ldrSetSRPMIN = micom_SRP_min;
	db_ldrSetSOPMAX = micom_SOP_max;
	db_ldrSetSOPMIN = micom_SOP_min;

}

bool isCoditionContorl() {
	return ( (pin_KEY_POWER == KEY_POWER_ON)
		&& (powerOnReadyDelayTimer > 1000) );
}



void mainStartInit(void) {
	// �µ�/�ǳ�   ��� ���� üũ �� ���� �ʱ�ȭ
	not_rx_temp_chkTimer = 0;
	b_temp_comm_not = 0;
	not_rx_pannel_chkTimer = 0;
	b_pannel_comm_not = 0;
	bef_b_pannel_comm_not = 0;
	b_pannel_comm_break_flag = 0;

	// �ʱ�ȭ
	gateRSTDo_time = MAX_GATE_zero_voltage;
	bRzeroTimerStart = 0;
	bSzeroTimerStart = 0;
	bTzeroTimerStart = 0;
	tiemrRzeroTimer = 0;
	tiemrSzeroTimer = 0;
	tiemrTzeroTimer = 0;

	pwstartTiemr = 0;

	pin_RUN_LED = LED_OFF; // 1 = off
	powerOnReadyDelayTimer = 0;

	// �ʱ�ȭ
	RZeroXChekTimer = 0;
	SZeroXChekTimer = 0;
	TZeroXChekTimer = 0;

	UserSystemStatus = M_NONE;
}

uint16_t getMaxByAmpType(uint8_t type) {
	uint16_t max;
	switch (type) {
		case 0:
			max = 5;
			return max * 10;
		case 1:
			max = 10;
			return max * 10;
		case 2:
			max = 15;
			return max * 10;
		case 3:
			max = 30;
			return max * 10;
		case 4:
			max = 50;
			return max * 10;
		case 5:
			max = 75;
			return max * 10;
		case 6:
			max = 100;
			return max * 10;
		case 7:
			max = 150;
			return max * 10;
		case 8:
			max = 200;
			return max * 10;
		case 9:
			max = 250;
			return max * 10;
		case 10:
			max = 300;
			return max * 10;
	}
	return 0;
}
uint16_t getMaxByAmpType_correct(uint8_t type) {
	uint16_t max;
	switch (type) {
		case 0: // ù��° Ÿ��
			max = 5;
			return (max * 2) + 10000; // 20%
		case 1:
			max = 10;
			return (max * 2)  + 10000;
		case 2:
			max = 15;
			return (max * 2)  + 10000;
		case 3:
			max = 30;
			return (max * 2)  + 10000;
		case 4:
			max = 50;
			return (max * 2)  + 10000;
		case 5:
			max = 75;
			return (max * 2)  + 10000;
		case 6:
			max = 100;
			return (max * 2)  + 10000;
		case 7:
			max = 150;
			return (max * 2)  + 10000;
		case 8:
			max = 200;
			return (max * 2)  + 10000;
		case 9:
			max = 250;
			return (max * 2)  + 10000;
		case 10:
			max = 300;
			return (max * 2)  + 10000;
	}
	return 0;
}
uint16_t getMinByAmpType_correct(uint8_t type) {
	uint16_t min;
	switch (type) {
		case 0: // ù��° Ÿ��
			min = 5;
			return 10000 - (min * 2); // 20%
		case 1:
			min = 10;
			return 10000 - (min * 2);
		case 2:
			min = 15;
			return 10000 - (min * 2);
		case 3:
			min = 30;
			return 10000 - (min * 2);
		case 4:
			min = 50;
			return 10000 - (min * 2);
		case 5:
			min = 75;
			return 10000 - (min * 2);
		case 6:
			min = 100;
			return 10000 - (min * 2);
		case 7:
			min = 150;
			return 10000 - (min * 2);
		case 8:
			min = 200;
			return 10000 - (min * 2);
		case 9:
			min = 250;
			return 10000 - (min * 2);
		case 10:
			min = 300;
			return 10000 - (min * 2);
	}
	return 0;
}

void ldr_maxValue_maxAmp() {
	uint16_t max, mn;
	mn = maxAmpMenuNum;
	max = MenuStatus[mn].M_EditDigitMaxValue = getMaxByAmpType(cF_amp_type);

    if (max >= 1000) MenuStatus[mn].M_EditShiftCnt = 5;
    else if (max >= 100) MenuStatus[mn].M_EditShiftCnt = 4;
    else MenuStatus[mn].M_EditShiftCnt = 3;
}
void ldr_maxminValue_correctAmpMenuMaxMin() {
// ���� Ÿ�Կ� ���� �δ� ������ �ִ�/�ּ� ���� ���� ����
	uint16_t max, min, mn;
	mn = maxCorrectAmpMenuNum;
	max = MenuStatus[mn].M_EditDigitMaxValue = getMaxByAmpType_correct(cF_amp_type);
	min = MenuStatus[mn].M_EditDigitMinValue = getMinByAmpType_correct(cF_amp_type);

	// �ڸ����� max �������ε� ��������.
	if (max >= 10000) MenuStatus[mn].M_EditShiftCnt = 6;
    else if (max >= 1000) MenuStatus[mn].M_EditShiftCnt = 5;
    else if (max >= 100) MenuStatus[mn].M_EditShiftCnt = 4;
	else if (max >= 10) MenuStatus[mn].M_EditShiftCnt = 3;
	else if (max >= 1) MenuStatus[mn].M_EditShiftCnt = 2;
    else MenuStatus[mn].M_EditShiftCnt = 1;
}


bool isSystemOffError(uint8_t errcode) {
// ���� ���� �޽��� ����
// ġ���� ���� ���� ��ȯ
	switch (errcode) {
		case ERR_FOP:	// RST
			UserSystemStatus = M_ERR_FOP;
			return 1;
		case ERR_SRP:	// 1. ���� ����
			UserSystemStatus = M_ERR_SRP;
			return 1;
		case ERR_SOP:	// 2. �ܼ�
			UserSystemStatus = M_ERR_SOP;
			return 1;
		case ERR_AOP:	// 3.
			UserSystemStatus = M_ERR_AOP;
			return 1;
		case ERR_ARP:	// 4. ���� ������ �ݴ�� ��� �� !
			UserSystemStatus = M_ERR_ARP;
			return 1;
		case ERR_OPR:
			UserSystemStatus = M_ERR_OPR;
			return 1;
		// --- ��� �� ���� ---------
		case ERR_UPR:	// ���
			UserSystemStatus = M_ERR_UPR;
			return 0;
		case ERR_NONE:
			return 0;
		default:
			break;
	}
	return 0;

}


void outputAlarmRyOnOff() {

	if (pin_KEY_ALARM == KEY_ALARM_OFF) {
		// �˶� ������ ������ off
		pin_RY_ALARM = RY_OFF;
		return;
	}

	if (bAlarmRyOn_Off == ON) {
		pin_RY_ALARM = RY_ON;
	} else {
		pin_RY_ALARM = RY_OFF;
	}
}

void loadSensorUseNouseTo(uint8_t buf[]) {
	buf[0] = cF_ch0_use;
	buf[1] = cF_ch1_use;
	buf[2] = cF_ch2_use;
	buf[3] = cF_ch3_use;
	buf[4] = cF_ch4_use;
	buf[5] = cF_ch5_use;
	buf[6] = cF_ch6_use;
	buf[7] = cF_ch7_use;
}


// -------------------------------------
// - main loop ------------------------
// ���� ����
void main(void) {
	uint8_t ch;
	uint8_t errorCode = ERR_NONE;
	static uint8_t ampType, bef_ampType;

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

	mainStartInit(); // ���� �����ϱ� ���� �ʱ�ȭ !

	ampType = bef_ampType = cF_amp_type;

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

		// ���� ������ �� 4��(����/����/����/����)�� �Ƴ��α� ä�� �� ȹ���ϱ� (�����ϱ�)
		for (ch = 0; ch < ADC_CH_MAX; ch++) {
			scr_micom_setNowInVoltage_mV(ch);
		}


		// #1025 SRP MAX���� �����Ĵ� ������ ��ȯ�Ͽ� �����ϱ� (�����ͺ��̽�ȭ)
		database_SRPSOPARLRM();

		if (isJustNowPowerOn()) { // powerOff -> On �ϴ� ���� !!!
			powerOnReadyDelayTimer = 0;
		}

		if (isCoditionContorl()) {

			if(isSystemOffError(db_finalErrorCode)) {
			// ġ���� ���� ���� �� !
				pin_RY_RUN = RY_OFF;
				gateRSTDo_time = MAX_GATE_zero_voltage; // off
			} else {
				// �Ϲ� ���� ���� �� ~ ~ ~
				db_finalErrorCode = allStepRun_5step(nRunStep);
			}

			// �ܼ��� �˶� ������ ON, OFF �ϱ� ���� ����
			if (db_finalErrorCode) {
				bAlarmRyOn_Off = ON;
			} else bAlarmRyOn_Off = OFF;
		} else {
		// ���� ����ġ off ���� �� !
			// system off !!!
			db_finalErrorCode = 0;
			UserSystemStatus = M_POWER_OFF;
			nRunStep = 1;
			bAlarmRyOn_Off = OFF;

			chkTimer_commomError_msec = 0;
			gateRSTDo_time = MAX_GATE_zero_voltage; // off
			pin_RY_RUN = RY_OFF;
			pin_RY_ALARM = RY_OFF;
			tiemr_30UjiChkUpper_msec = 0;
		}
		outputAlarmRyOnOff();



// ------ �� Ÿ --------------------------------------------------

		// ZSU use/not_use ���� �� �����ϱ�
		loadSensorUseNouseTo(bufZSU_use_not);

		// ���� Ÿ�Ժ� �δ� ����ġ ���� ���� �ϱ� ���� �Լ��̴�.
		ldr_maxValue_maxAmp();
		ldr_maxminValue_correctAmpMenuMaxMin();

		// ���� Ÿ�� ������ ���� �Ǹ� ������ �ʱ�ȭ �ؾ� �Ѵ�. (������ ���� ���� ���� ����)
		ampType = cF_amp_type;
		if (ampType != bef_ampType) {
			bef_ampType = ampType;
			writeFlash(F_SCR_CORRECT_A, 10000);
		}
    }
}

void interrupt isr(void) {
    static unsigned int timer_msec = 0;
	uint8_t ch;

	// ZERO RST �Է� ���ͷ�Ʈ (��� ����) üũ
	// * R
	if(INT0IF && INT0IE){
  		INT0IF = 0;
		bRzeroTimerStart = 1; // ���� Ÿ�̸� ������ ���� ���� RST ���� ����
		tiemrRzeroTimer = 0;
		RZeroXChekTimer = 0;
	}
	// * S
	if(INT1IF && INT1IE){
  		INT1IF = 0;
		bSzeroTimerStart = 1;
		tiemrSzeroTimer = 0;
		SZeroXChekTimer = 0;
	}
	// * T
	if(INT2IF && INT2IE){
  		INT2IF = 0;
		bTzeroTimerStart = 1;
		tiemrTzeroTimer = 0;
		TZeroXChekTimer = 0;
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

		if (chkTimer_commomError_msec < 0xffff) chkTimer_commomError_msec++;
		if (powerOnReadyDelayTimer < 0xffff) powerOnReadyDelayTimer++;

		if (bUPRstate) {
			if (tiemr_30UjiChkUpper_msec < 0xffff) tiemr_30UjiChkUpper_msec++;
		}

		if (RZeroXChekTimer < 0xffff) RZeroXChekTimer++;
		if (SZeroXChekTimer < 0xffff) SZeroXChekTimer++;
		if (TZeroXChekTimer < 0xffff) TZeroXChekTimer++;
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


