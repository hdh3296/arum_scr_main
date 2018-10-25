

#ifndef __MAIN_H__
#define __MAIN_H__

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned char bool;


uint16_t chkTimer_commomError_msec;
uint16_t powerOnReadyDelayTimer;


#define ZSU_CH_MAX	8

#define RX_GOOD_SUCESS_T 20


#define __TEMP_1000 1
#define TRUE 1
#define FALSE 0
#define ON_LED 0
#define OFF_LED 1
#define ON_RELAY 1
#define OFF_RELAY 0

enum {
	KEY_POWER_ON = 0,
	KEY_POWER_OFF = 1
};
enum {
	KEY_ALARM_ON = 0,
	KEY_ALARM_OFF = 1
};
enum {
	KEY_50HZ_ON = 0,
	KEY_50HZ_OFF = 1
};



enum {
	SIGN_MINUS,
	SIGN_PLUS
};

enum {
	TERR_NONE = 0,
	TERR_BREAK = 1,
	TERR_SHORT = 2,
};

// �µ�ǥ
typedef enum {
	T00 = 1881,	T01 = 1916, T02 = 1952, T03 = 1987, T04 = 2022,
	T05 = 2058,	T06 = 2093, T07 = 2128, T08 = 2164, T09 = 2199,
	T10 = 2234, T11 = 2269, T12 = 2304, T13 = 2339, T14 = 2374,
	T15 = 2409, T16 = 2444, T17 = 2479, T18 = 2514, T19 = 2549,
	T20 = 2584, T21 = 2619, T22 = 2654, T23 = 2689, T24 = 2723,
	T25 = 2758, T26 = 2793, T27 = 2827, T28 = 2862, T29 = 2897,
	T30 = 2931, T31 = 2966, T32 = 3000, T33 = 3035, T34 = 3069,
	T35 = 3104, T36 = 3138, T37 = 3173, T38 = 3207, T39 = 3241,
	T40 = 3276, T41 = 3310, T42 = 3344, T43 = 3378, T44 = 3413,
	T45 = 3447, T46 = 3481, T47 = 3515, T48 = 3549, T49 = 3583,
	T50 = 3617,	T51 = 3651, T52 = 3685, T53 = 3719, T54 = 3753,
	T55 = 3787,	T56 = 3821, T57 = 3854, T58 = 3888, T59 = 3922
} tag_TempValueDef;

uint16_t analogTempList[] = {
	T00
};


extern uint16_t getCurTempTo(uint16_t);

#define MAX_CH 5
typedef struct {
	bool 	db_heatingOnState; // pwm on/off ���� (heater on/off ����)

    uint8_t db_tempShortBreak; // ��Ʈ, �ܼ�
    uint8_t db_errorCode;		// ���� �ڵ� ��ȣ

	// ����, ����, ���� �Է� ����
    uint16_t anlog_nowAmp_mV; 			// ����, �Է� �Ƴ��α� (������)
    uint16_t anlog_correctedVoltage_mV; // ����, �Է� ���� �Ƴ��α� (���� ����)
    uint16_t anlog_correctedAmp_mV; 	// ����, �Է� ���� �Ƴ��α� (���� ����)
	uint16_t userNowInVoltage_V;				// ����, �Է� ���� (���� ���̱�)
	uint16_t userNowInAmp_100mA;				// ����, �Է� ���� (���� ���̱� )
    uint16_t analog_goalSetVoltage_mV;	// Goal ���� ���� �Ƴ��α�
    uint16_t analog_goalSetCurrent_mV;	// Goal ���� ���� �Ƴ��α�



	uint16_t userCorrVoltSet;			// ���� - ���� ex) +10 V
	uint16_t userCorrAmpSet;			// ���� - ���� ex) +1.0 A

    uint16_t db_finaldutycycle;			// ƩƼ����Ŭ


    bool 	db_heaterCmd_on_off_none; // ��� - on / off / none
	bool 	bChEnStateFromPannel;	// enable / disable ���� (from �ǳ�)
	uint8_t db_bFlashChEnable_state;	// �÷����� ����� enable / disable ����

    bool 	db_bChUse; // use / notuse

	// ��Ÿ
    bool adc_bUdted_mA;
    bool db_bCurrentOver;

	// �µ� ���� ������ ���̽�
	// 1. ����� ���� ���� ���� �Ƴ��α�
	// 2. ���� �Ƴ��α� on / off ��
	uint16_t db_nowTemp_mV;
	uint16_t db_setOnTemp_mV;
	uint16_t db_setOffTemp_mV;
	uint16_t corrTempSet;


	// #�νĹ���
    uint16_t goalSetVoltage_V;	// Goal ���� ���� ���� ����) 1v => �����Ŀ��� 50mV
    uint16_t goalSetAmp;	// Goal ���� ���� ����(����) ����) 50A =>
	uint16_t goalSetSensor;

	uint16_t nowMicomAdVoltage;
	uint16_t nowMicomAdCurrent;
	uint16_t nowMicomMainAdSensor;
	uint16_t nowMicomAdVolume;
	bool bNowMicomAdVoltage_updted;
	bool bNowMicomAdCurrent_updted;
	bool bNowMicomAdSensor_updted;
	bool bNowMicomAdVolume_updted;


    uint16_t adc_nowAnalog_mV; 		// ����, �Է� �Ƴ��α� (������) => ����/����/����/����
    //  �̰��� ä�� 0 ~ 3 ���� ���� ����/����/����/���� �����̴�.
} Heater;


extern Heater heater[];

extern void can_txReady(void);

extern unsigned    char    can_txBuffer[];

extern unsigned    int     Can1TxDataTotalCnt;
extern unsigned    int     CanTxDataTxPointer;

extern uint16_t rxGoodSucessTimer;

extern uint8_t bufZSU_use_not[];

extern uint16_t micom_getSensorNowSuwi(uint8_t);

#endif

