
#ifndef __loader_UserDesign_h__
#define __loader_UserDesign_h__

#include    "..\main.h"

// #1020
#define MAX_FLASH_BLOCK_NM 3  // (64 * 10 = 640 byte)	or ( (64/4) * 10 = 160 long)
#define MAX_MENU 57 // 57개가 amx 네


extern uint16_t DefaultDisplay(void);

/////////////////////////////////////////////////////////////////////
////////////////////user message  set//////////////////////////////
enum {
	M_NONE = 0,
	M_ERR_FOP, // RST 상
	M_ERR_SRP, // 1step : 센서 역전
	M_ERR_SOP, // 2step : 단선
	M_ERR_AOP, // 3step
	M_ERR_ARP, // 4step
	M_ERR_UPR,
	M_ERR_OPR,
	M_08,
	M_09,
	M_POWER_OFF,
	M_RUNNING,
	M_1ST_SRP_CHK,
	M_2ST_SOP_CHK,
	M_3ST_AOP_CHK,
	M_4ST_ARP_CHK,
};

const uint8_t StatusMessage[][17] = {
	"00.---------    ", // 0
    "01.ERR_FOP !    ", // 1
    "02.1st.ERR_SRP !", // 2
    "03.2st.ERR_SOP !", // 3
    "04.3st.ERR_AOP !", // 4
    "05.4st.ERR_ARP !", // 5
    "06.ERR_UPR !    ", // 6
    "07.ERR_OPR !    ", // 7
    "08.             ", // 8
    "09.             ", // 9
    "10.Power Off    ", // 10
    "11.running      ", // 11
    "1st.SRP Chking  ", // 12
    "2st.SOP Chking  ", // 13
    "3st.AOP Chking  ", // 14
    "4st.ARP Chking  ", // 15
};
// home, ready, system running

const uint8_t GroupLineMessage[][17] = {
    "USER:Display Sel", //1
    "USER:Version    ", //0  // group0
    // 그룹 : 목표치 설정
    "limit: Volt(V)  ", //14
    "limit: Amp(A)   ", //15
    "goal:Set Sensor ", //16
    "correct: Voltage", //16
    "correct: Amp    ", //16
    "correct: CH0    ", //16
    "correct: CH1    ", //9  // group1
    "correct: CH2    ", //10
    "correct: CH3    ", //14
    "correct: CH4    ", //15
    "correct: CH5    ", //16
    "correct: CH6    ", //16
    "correct: CH7    ", //16
    "correct: CH8    ", //16

    "zsu :ch0 e/d    ", //16
    "zsu :ch1 e/d    ", //16
    "zsu :ch2 e/d    ", //9  // group1
    "zsu :ch3 e/d    ", //10
    "zsu :ch4 e/d    ", //14
    "zsu :ch5 e/d    ", //15
    "zsu :ch6 e/d    ", //16
    "zsu :ch7 e/d    ", //16
    "Main:ch8 e/d    ", //16
	// 센서 정/역
	// 정방향 센서 : 징크 Zinc
	//				- 양전하   수위 검출 용도
	// 역방향 센서 : CUCU/SO4
	//				- 음전하 수위 검출 용도
	"zsu:ch0 REF TYPE", //16
	"zsu:ch1 REF TYPE", //16
	"zsu:ch2 REF TYPE", //16
	"zsu:ch3 REF TYPE", //16
	"zsu:ch4 REF TYPE", //16
	"zsu:ch5 REF TYPE", //16
	"zsu:ch6 REF TYPE", //16
	"zsu:ch7 REF TYPE", //16
	"main:   REF TYPE", //16

    "etc :currentType", //16
	"SRP : MAX       ", //16
    "SRP : MIN       ", //16
    "SRP : TIME      ", //9  // group1
	"SOP : MAX       ", //16
    "SOP : MIN       ", //16
    "SOP : TIME      ", //9  // group1
    "AOP : DUTY      ", //16
    "AOP : TIME      ", //16
    "ARP : DUTY      ", //16
    "ARP : TIME      ", //16
    "UPR : opr Limit ", //16
    "OPR : upr Limit ", //16

    "FOP : ON / OFF  ",
    "1SRP: ON / OFF  ",
    "2SOP: ON / OFF  ",
    "3AOP: ON / OFF  ",
    "4ARP: ON / OFF  ",
    "UPR : ON / OFF  ",
    "OPR : ON / OFF  ",
};


const uint8_t DefaultMsg[][17] = {
    "Running         ", //0
    "Ready...        "
};



const uint8_t ch_enable_sel_list[][17] = {
    "Off             ", //0
    "On              ", //1
};

const uint8_t ch_use_sel_list[][17] = {
    "Not use         ", //0
    "Use             ", //1
};

const uint8_t ch_reverse_sel_list[][17] = {
    "Zinc            ", //0
    "CUCU/SO4        ", //1
};

const uint8_t ldr_errorTestSetText[][17] = {
    "OFF             ",
    "ON              ",
};



const uint8_t ampTypeList[][17] = {
    "5A              ", //0
    "10A             ", //1
    "15A             ", //2
    "30A             ", //3
    "50A             ", //4
    "75A             ", //5
    "100A            ", //6
    "150A            ", //7
    "200A            ", //8
    "250A            ", //9
    "300A            ", //10
};


const uint8_t DefaultMsgSel[][17] = {
/*
    로더 초기화면에 표시할 메뉴 선택
*/
    "display state   ", //0
    "display data    ", //1
};


const uint8_t FlashMsgSel[][17] = {
    ".             ", //0
    "..            ", //1
    "...           ", //2
};


/////////////////////////////////////////////////////////////////////
////////////////////memory address set//////////////////////////////

// ByteData001 ~ 064 (64바트 = 1블럭) 까지

#define F_VERSION			ByteData001						//1
#define F_DEFAULT_DSP       ByteData002						//1
#define F_FLASH_COPY        ByteData003					//1
#define F_INITIAL           ByteData004						//1
// not use 					ByteData005

// ch0~4 ----------------------------------------------------
#define F_CH0_USE           ByteData006
#define F_CH1_USE			ByteData007
#define F_CH2_USE			ByteData008
#define F_CH3_USE			ByteData009
#define F_CH4_USE			ByteData010
#define F_CH5_USE	    	ByteData011
#define F_CH6_USE	    	ByteData012
#define F_CH7_USE   	 	ByteData013
#define F_CH8_USE    		ByteData014

// none ㅜㅜ
#define F_CH4_ENABLE    	ByteData015

// 전류 타입 11가지 선택
#define F_AMP_TYPE      	ByteData016
// #부식방지 : 센서 Zinc / CUCUSO4
#define F_REVERSE_0     	ByteData017
#define F_REVERSE_1     	ByteData018
#define F_REVERSE_2     	ByteData019
#define F_REVERSE_3     	ByteData020
#define F_REVERSE_4    	 	ByteData021
#define F_REVERSE_5     	ByteData022
#define F_REVERSE_6     	ByteData023
#define F_REVERSE_7     	ByteData024
#define F_REVERSE_8     	ByteData025

#define F_FOP_EN    	ByteData041
#define F_1SRP_EN    	ByteData042
#define F_2SOP_EN    	ByteData043
#define F_3AOP_EN    	ByteData044
#define F_4ARP_EN    	ByteData045
#define F_UPR_EN    	ByteData046
#define F_OPR_EN    	ByteData047




#define F_CH3_CORRECT_A     ByteData037
#define F_CH4_CORRECT_T     ByteData038
#define F_CH4_CORRECT_V     ByteData039
#define F_CH4_CORRECT_A     ByteData040






// ByteData065 ~  (2번째 블럭 부터) : 인테져
// 64 바이트 부터 integer 값을 사용하였다.
// ※ 두번째 블락 (인테져 타입은 두번째 블럭에서 처리 하고자 한다.)
#define F_SCR_GOAL_VOLTAGE     		IntzData033
#define F_SCR_GOAL_CURRENT     		IntzData034
#define F_SCR_GOAL_SENSOR       	IntzData035
#define F_SRP_MAX     				IntzData036
#define F_SRP_MIN			       	IntzData037
#define F_SRP_TIME		     		IntzData038

#define F_SOP_MAX       	IntzData039
#define F_SOP_MIN     		IntzData040
#define F_SOP_TIME       	IntzData041

#define F_AOP_DUTY     		IntzData042
#define F_AOP_TIME     		IntzData043
#define F_ARP_DUTY     		IntzData044
#define F_ARP_TIME     		IntzData045

#define F_UPR_SET       	IntzData046
#define F_OPR_SET     		IntzData047


// 보정 기능 추가 (추후 보정 설정 기능 할 때) #1020
#define F_SCR_CORRECT_V     	IntzData048
#define F_SCR_CORRECT_A     	IntzData049
#define F_SCR_CORRECT_ch0    	IntzData050
#define F_SCR_CORRECT_ch1    	IntzData051
#define F_SCR_CORRECT_ch2    	IntzData052
#define F_SCR_CORRECT_ch3    	IntzData053
#define F_SCR_CORRECT_ch4    	IntzData054
#define F_SCR_CORRECT_ch5    	IntzData055
#define F_SCR_CORRECT_ch6    	IntzData056
#define F_SCR_CORRECT_ch7    	IntzData057
#define F_SCR_CORRECT_ch8    	IntzData058 // main sensor

// 128 바이트
////////////////////////////////////////////////////////////////////////

// 데이터 베이스
#define cF_VERSION 				cF_ByteData(F_VERSION)
#define cF_DEFAULT_DSP 			cF_ByteData(F_DEFAULT_DSP)
#define cF_FLASH_COPY 			cF_ByteData(F_FLASH_COPY)
#define cF_INITIAL 				cF_ByteData(F_INITIAL)

#define cF_ch0_use 			cF_ByteData(F_CH0_USE)
#define cF_ch1_use 			cF_ByteData(F_CH1_USE)
#define cF_ch2_use 			cF_ByteData(F_CH2_USE)
#define cF_ch3_use 			cF_ByteData(F_CH3_USE)
#define cF_ch4_use 			cF_ByteData(F_CH4_USE)
#define cF_ch5_use	 		cF_ByteData(F_CH5_USE) //
#define cF_ch6_use 			cF_ByteData(F_CH6_USE)
#define cF_ch7_use 			cF_ByteData(F_CH7_USE)
#define cF_ch8_use 			cF_ByteData(F_CH8_USE)

#define cF_ch4_enable 		cF_ByteData(F_CH4_ENABLE) // ※ iF -> cF
#define cF_amp_type   		cF_ByteData(F_AMP_TYPE)

// 센서 zinc, cocuso4 type 설정 값
#define cF_sensorType_0      		cF_ByteData(F_REVERSE_0)
#define cF_sensorType_1 			cF_ByteData(F_REVERSE_1)
#define cF_sensorType_2 			cF_ByteData(F_REVERSE_2)
#define cF_sensorType_3 			cF_ByteData(F_REVERSE_3)
#define cF_sensorType_4 			cF_ByteData(F_REVERSE_4)
#define cF_sensorType_5 			cF_ByteData(F_REVERSE_5)
#define cF_sensorType_6 			cF_ByteData(F_REVERSE_6)
#define cF_sensorType_7 			cF_ByteData(F_REVERSE_7)
#define cF_sensorType_8 			cF_ByteData(F_REVERSE_8)

#define cF_FOP_en	 			cF_ByteData(F_FOP_EN)
#define cF_1SRP_en	 			cF_ByteData(F_1SRP_EN)
#define cF_2SOP_en	 			cF_ByteData(F_2SOP_EN)
#define cF_3AOP_en	 			cF_ByteData(F_3AOP_EN)
#define cF_4ARP_en	 			cF_ByteData(F_4ARP_EN)
#define cF_UPR_en	 			cF_ByteData(F_UPR_EN)
#define cF_OPR_en	 			cF_ByteData(F_OPR_EN)



#define cF_ch3_correctA 			cF_ByteData(F_CH3_CORRECT_A)
#define cF_ch4_correctT 			cF_ByteData(F_CH4_CORRECT_T)
#define cF_ch4_correctV 			cF_ByteData(F_CH4_CORRECT_V)
#define cF_ch4_correctA 			cF_ByteData(F_CH4_CORRECT_A)

// integer ---------------------------------------------------------
// ------------------------------------------------------------------
#define iF_scr_goalVoltage 			iF_IntData(F_SCR_GOAL_VOLTAGE)
#define iF_scr_goalAmp 				iF_IntData(F_SCR_GOAL_CURRENT)
#define iF_scr_goalSensor 			iF_IntData(F_SCR_GOAL_SENSOR)
#define iF_SRP_max 					iF_IntData(F_SRP_MAX)
#define iF_SRP_min 					iF_IntData(F_SRP_MIN)
#define iF_SRP_time		 			iF_IntData(F_SRP_TIME)
#define iF_SOP_max 					iF_IntData(F_SOP_MAX)
#define iF_SOP_min 					iF_IntData(F_SOP_MIN)
#define iF_SOP_time 				iF_IntData(F_SOP_TIME)

#define iF_AOP_duty 			iF_IntData(F_AOP_DUTY)
#define iF_AOP_time 			iF_IntData(F_AOP_TIME)
#define iF_ARP_duty 			iF_IntData(F_ARP_DUTY)
#define iF_ARP_time 			iF_IntData(F_ARP_TIME)

#define iF_UPR_set 					iF_IntData(F_UPR_SET)
#define iF_OPR_set 					iF_IntData(F_OPR_SET)

// 보정
#define iF_correct_V_user 			iF_IntData(F_SCR_CORRECT_V)
#define iF_correct_A_user 			iF_IntData(F_SCR_CORRECT_A)
#define iF_correct_ch0_mV 			iF_IntData(F_SCR_CORRECT_ch0)
#define iF_correct_ch1_mV 			iF_IntData(F_SCR_CORRECT_ch1)
#define iF_correct_ch2_mV 			iF_IntData(F_SCR_CORRECT_ch2)
#define iF_correct_ch3_mV 			iF_IntData(F_SCR_CORRECT_ch3)
#define iF_correct_ch4_mV 			iF_IntData(F_SCR_CORRECT_ch4)
#define iF_correct_ch5_mV 			iF_IntData(F_SCR_CORRECT_ch5)
#define iF_correct_ch6_mV 			iF_IntData(F_SCR_CORRECT_ch6)
#define iF_correct_ch7_mV 			iF_IntData(F_SCR_CORRECT_ch7)
#define iF_correct_ch8_mV 			iF_IntData(F_SCR_CORRECT_ch8)

// ==============================================
////////////////////////////////////////////////////////
/////////////sram memory set- char type/////////////////
////////////////////////////////////////////////////////
#define UpButtonTime_8 0
#define msec100_8 1
#define sec1_8 2
#define MotorSeq_8 3
#define BasketNm_8 4
#define RepeateCounter_8 5
#define LeftRightCurpt_8 6
#define UpDownCurpt_8 7
#define VARIABLE_8_END 8
/////////////////////////////////////////////////////////////////
/////////////sram memory set- int type/////////////////////////
/////////////////////////////////////////////////////////////////
#define StartKeyChartTime_16 0
#define VibTime_16 1
#define LeftRightMovePulse_16 2
#define UpDownMovePulse_16 3
#define ReverseTime_16 4
#define VARIABLE_16_END 5
#define VARIABLE_32_END 1
extern uint16_t LoaderKey;

extern uint16_t getCorrectedVoltage_mV(uint8_t ch);
extern uint16_t getCorrectedCurrent_mV(uint8_t ch);

extern uint16_t getCorrentSetT(uint8_t ch);
extern bool positiveCorrT(uint8_t ch);
extern uint16_t maxAmpMenuNum;
extern uint16_t maxCorrectAmpMenuNum;
#endif

