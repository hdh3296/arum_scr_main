
#ifndef __loader_UserDesign_h__
#define __loader_UserDesign_h__

#include    "..\main.h"


#define MAX_FLASH_BLOCK_NM 3  // (64 * 10 = 640 byte)	or ( (64/4) * 10 = 160 long)
#define MAX_MENU 50


extern uint16_t DefaultDisplay(void);

/////////////////////////////////////////////////////////////////////
////////////////////user message  set//////////////////////////////
const uint8_t StatusMessage[][17] = {
    "Wait_Ready      ", //000
    "System Home.... ", //000
    "Basket1 Home    ", //000
    "Basket  Ready   ", //000
};


const uint8_t GroupLineMessage[][17] = {
    "USER:Display Sel", //1
    "USER:Version    ", //0  // group0
    // 그룹 : 목표치 설정
    "goal:Set Volt(V)", //14
    "goal:Set Amp(A) ", //15
    "goal:Set Sensor ", //16
    "CH-1:ch Use/Not ", //16
    "CH-1:ch correctT", //16
    "CH-1:ch correctV", //16
    "CH-1:ch correctA", //16

    "CH-2:On Temp    ", //9  // group1
    "CH-2:Off Temp   ", //10
    "CH-2:Set Volt(V)", //14
    "CH-2:Set Amp(A) ", //15
    "CH-2:ch On/Off  ", //16
    "CH-2:ch Use/Not ", //16
    "CH-2:ch correctT", //16
    "CH-2:ch correctV", //16
    "CH-2:ch correctA", //16

    "CH-3:On Temp    ", //9  // group1
    "CH-3:Off Temp   ", //10
    "CH-3:Set Volt(V)", //14
    "CH-3:Set Amp(A) ", //15
    "CH-3:ch On/Off  ", //16
    "CH-3:ch Use/Not ", //16
    "CH-3:ch correctT", //16
    "CH-3:ch correctV", //16
    "CH-3:ch correctA", //16

    "CH-4:On Temp    ", //9  // group1
    "CH-4:Off Temp   ", //10
    "CH-4:Set Volt(V)", //14
    "CH-4:Set Amp(A) ", //15
    "CH-4:ch On/Off  ", //16
    "CH-4:ch Use/Not ", //16
    "CH-4:ch correctT", //16
    "CH-4:ch correctV", //16
    "CH-4:ch correctA", //16

    "CH-5:On Temp    ", //9  // group1
    "CH-5:Off Temp   ", //10
    "CH-5:Set Volt(V)", //14
    "CH-5:Set Amp(A) ", //15
    "CH-5:ch On/Off  ", //16
    "CH-5:ch Use/Not ", //16
    "CH-5:ch correctT", //16
    "CH-5:ch correctV", //16
    "CH-5:ch correctA", //16

};


const uint8_t DefaultMsg[][17] = {
    "Running         ", //0
    "Ready...        "
};



const uint8_t ch_enable_sel_list[][17] = {
    "Off            ", //0
    "On           ", //1
};

const uint8_t ch_use_sel_list[][17] = {
    "Not use         ", //0
    "Use           ", //1
};



const uint8_t DefaultMsgSel[][17] = {
/*
    로더 초기화면에 표시할 메뉴 선택
*/
    "display CH    ", //0
    "display pwm   ", //1
};


const uint8_t FlashMsgSel[][17] = {
    ".             ", //0
    "..            ", //1
    "...           ", //2
};


/////////////////////////////////////////////////////////////////////
////////////////////memory address set//////////////////////////////
#define F_VERSION			ByteData001						//1
#define F_DEFAULT_DSP       ByteData002						//1
#define F_FLASH_COPY        ByteData003					//1
#define F_INITIAL           ByteData004						//1
// not use 					ByteData005

// ch0~4 ----------------------------------------------------
#define F_CH0_ENABLE    	ByteData006
#define F_CH0_USE           ByteData007
#define F_CH1_ENABLE    	ByteData008
#define F_CH1_USE			ByteData009
#define F_CH2_ENABLE    	ByteData010
#define F_CH2_USE			ByteData011
#define F_CH3_ENABLE    	ByteData012
#define F_CH3_USE			ByteData013
#define F_CH4_ENABLE    	ByteData014
#define F_CH4_USE			ByteData015

#define F_CH0_TEMP_LOW      ByteData016
#define F_CH0_TEMP_HIGH     ByteData017
#define F_CH1_TEMP_LOW     	ByteData018
#define F_CH1_TEMP_HIGH     ByteData019
#define F_CH2_TEMP_LOW     	ByteData020
#define F_CH2_TEMP_HIGH     ByteData021
#define F_CH3_TEMP_LOW     	ByteData022
#define F_CH3_TEMP_HIGH     ByteData023
#define F_CH4_TEMP_LOW     	ByteData024
#define F_CH4_TEMP_HIGH     ByteData025

// 보정 기능 추가
#define F_CH0_CORRECT_T     ByteData026
#define F_CH0_CORRECT_V     ByteData027
#define F_CH0_CORRECT_A     ByteData028

#define F_CH1_CORRECT_T     ByteData029
#define F_CH1_CORRECT_V     ByteData030
#define F_CH1_CORRECT_A     ByteData031

#define F_CH2_CORRECT_T     ByteData032
#define F_CH2_CORRECT_V     ByteData033
#define F_CH2_CORRECT_A     ByteData034

#define F_CH3_CORRECT_T     ByteData035
#define F_CH3_CORRECT_V     ByteData036
#define F_CH3_CORRECT_A     ByteData037

#define F_CH4_CORRECT_T     ByteData038
#define F_CH4_CORRECT_V     ByteData039
#define F_CH4_CORRECT_A     ByteData040

// ※ 두번째 블락 (인테져 타입은 두번째 블럭에서 처리 하고자 한다.)
#define F_SCR_GOAL_VOLTAGE     		IntzData033
#define F_SCR_GOAL_CURRENT     		IntzData034
#define F_SCR_GOAL_SENSOR       	IntzData035

#define F_CH1_GOAL_CURRENT     		IntzData036

#define F_CH2_GOAL_VOLTAGE       	IntzData037
#define F_CH2_GOAL_CURRENT     		IntzData038

#define F_CH3_GOAL_VOLTAGE       	IntzData039
#define F_CH3_GOAL_CURRENT     		IntzData040

#define F_CH4_GOAL_VOLTAGE       	IntzData041
#define F_CH4_GOAL_CURRENT     		IntzData042


// -----------------------------------------------------------
#define F_TEST_LONG         		longData033 					//1
////////////////////////////////////////////////////////////////////////

// 데이터 베이스
#define cF_VERSION 				cF_ByteData(F_VERSION)
#define cF_DEFAULT_DSP 			cF_ByteData(F_DEFAULT_DSP)
#define cF_FLASH_COPY 			cF_ByteData(F_FLASH_COPY)
#define cF_INITIAL 				cF_ByteData(F_INITIAL)

#define cF_ch0_enable 		cF_ByteData(F_CH0_ENABLE) // ※ iF -> cF
#define cF_ch0_use 			cF_ByteData(F_CH0_USE)
#define cF_ch1_enable 		cF_ByteData(F_CH1_ENABLE)
#define cF_ch1_use 			cF_ByteData(F_CH1_USE)
#define cF_ch2_enable 		cF_ByteData(F_CH2_ENABLE) // ※ iF -> cF
#define cF_ch2_use 			cF_ByteData(F_CH2_USE)
#define cF_ch3_enable 		cF_ByteData(F_CH3_ENABLE)
#define cF_ch3_use 			cF_ByteData(F_CH3_USE)
#define cF_ch4_enable 		cF_ByteData(F_CH4_ENABLE) // ※ iF -> cF
#define cF_ch4_use 			cF_ByteData(F_CH4_USE)
// ch0
#define cF_ch0_temp_low     		cF_ByteData(F_CH0_TEMP_LOW)
#define cF_ch0_temp_high    		cF_ByteData(F_CH0_TEMP_HIGH)
// ch1
#define cF_ch1_temp_low 			cF_ByteData(F_CH1_TEMP_LOW)
#define cF_ch1_temp_high 			cF_ByteData(F_CH1_TEMP_HIGH)
// ch2
#define cF_ch2_temp_low 			cF_ByteData(F_CH2_TEMP_LOW)
#define cF_ch2_temp_high 			cF_ByteData(F_CH2_TEMP_HIGH)
// ch3
#define cF_ch3_temp_low 			cF_ByteData(F_CH3_TEMP_LOW)
#define cF_ch3_temp_high 			cF_ByteData(F_CH3_TEMP_HIGH)
// ch4
#define cF_ch4_temp_low 			cF_ByteData(F_CH4_TEMP_LOW)
#define cF_ch4_temp_high 			cF_ByteData(F_CH4_TEMP_HIGH)
// 보정
#define cF_ch0_correctT 			cF_ByteData(F_CH0_CORRECT_T)
#define cF_ch0_correctV 			cF_ByteData(F_CH0_CORRECT_V)
#define cF_ch0_correctA 			cF_ByteData(F_CH0_CORRECT_A)

#define cF_ch1_correctT 			cF_ByteData(F_CH1_CORRECT_T)
#define cF_ch1_correctV 			cF_ByteData(F_CH1_CORRECT_V)
#define cF_ch1_correctA 			cF_ByteData(F_CH1_CORRECT_A)

#define cF_ch2_correctT 			cF_ByteData(F_CH2_CORRECT_T)
#define cF_ch2_correctV 			cF_ByteData(F_CH2_CORRECT_V)
#define cF_ch2_correctA 			cF_ByteData(F_CH2_CORRECT_A)

#define cF_ch3_correctT 			cF_ByteData(F_CH3_CORRECT_T)
#define cF_ch3_correctV 			cF_ByteData(F_CH3_CORRECT_V)
#define cF_ch3_correctA 			cF_ByteData(F_CH3_CORRECT_A)

#define cF_ch4_correctT 			cF_ByteData(F_CH4_CORRECT_T)
#define cF_ch4_correctV 			cF_ByteData(F_CH4_CORRECT_V)
#define cF_ch4_correctA 			cF_ByteData(F_CH4_CORRECT_A)

// ------------------------------------------------------------------
#define iF_scr_goalVoltage 			iF_IntData(F_SCR_GOAL_VOLTAGE)
#define iF_scr_goalCurrent 			iF_IntData(F_SCR_GOAL_CURRENT)
#define iF_scr_goalSensor 			iF_IntData(F_SCR_GOAL_SENSOR)

#define iF_ch1_goalCurrent 			iF_IntData(F_CH1_GOAL_CURRENT)

#define iF_ch2_goalVoltage 			iF_IntData(F_CH2_GOAL_VOLTAGE)
#define iF_ch2_goalCurrent 			iF_IntData(F_CH2_GOAL_CURRENT)

#define iF_ch3_goalVoltage 			iF_IntData(F_CH3_GOAL_VOLTAGE)
#define iF_ch3_goalCurrent 			iF_IntData(F_CH3_GOAL_CURRENT)

#define iF_ch4_goalVoltage 			iF_IntData(F_CH4_GOAL_VOLTAGE)
#define iF_ch4_goalCurrent 			iF_IntData(F_CH4_GOAL_CURRENT)


// ==============================================
#define lF_TEST_LONG lF_LongData(longData033)
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

#endif

