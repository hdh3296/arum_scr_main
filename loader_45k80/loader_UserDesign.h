
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
    "scr :correct V  ", //16
    "scr :correct A  ", //16
    "scr :correct S-M", //16
    "scr :correct S0 ", //16
    "scr :correct S1 ", //9  // group1
    "scr :correct S2 ", //10
    "scr :correct S3 ", //14
    "scr :correct S4 ", //15
    "scr :correct S5 ", //16
    "scr :correct S6 ", //16
    "scr :correct S7 ", //16

    "zsu :ch0 e/d    ", //16
    "zsu :ch1 e/d    ", //16
    "zsu :ch2 e/d    ", //9  // group1
    "zsu :ch3 e/d    ", //10
    "zsu :ch4 e/d    ", //14
    "zsu :ch5 e/d    ", //15
    "zsu :ch6 e/d    ", //16
    "zsu :ch7 e/d    ", //16
	// 센서 정/역
	"zsu :ch0 R/F    ", //16
	"zsu :ch1 R/F    ", //16
	"zsu :ch2 R/F    ", //16
	"zsu :ch3 R/F    ", //16
	"zsu :ch4 R/F    ", //16
	"zsu :ch5 R/F    ", //16
	"zsu :ch6 R/F    ", //16
	"zsu :ch7 R/F    ", //16
	"main:--- R/F    ", //16

    "etc :currentType", //16

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
    "Off             ", //0
    "On              ", //1
};

const uint8_t ch_use_sel_list[][17] = {
    "Not use         ", //0
    "Use             ", //1
};
const uint8_t ch_reverse_sel_list[][17] = {
    "Reverse         ", //0
    "Forward         ", //1
};


const uint8_t currentTypeList[][17] = {
    "A_type1         ", //0
    "A_type2         ", //1
    "A_type3         ", //1
    "A_type4         ", //1
    "A_type5         ", //1
    "A_type6         ", //1
    "A_type7         ", //1
    "A_type8         ", //1
    "A_type9         ", //1
    "A_type10        ", //1
};


const uint8_t DefaultMsgSel[][17] = {
/*
    로더 초기화면에 표시할 메뉴 선택
*/
    "display CH      ", //0
    "display pwm     ", //1
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
#define F_CH0_USE           ByteData006
#define F_CH1_USE			ByteData007
#define F_CH2_USE			ByteData008
#define F_CH3_USE			ByteData009
#define F_CH4_USE			ByteData010
#define F_CH5_USE	    	ByteData011
#define F_CH6_USE	    	ByteData012
#define F_CH7_USE   	 	ByteData013
// #부식방지 : 센서 정/역
#define F_REVERSE_0     	ByteData017
#define F_REVERSE_1     	ByteData018
#define F_REVERSE_2     	ByteData019
#define F_REVERSE_3     	ByteData020
#define F_REVERSE_4    	 	ByteData021
#define F_REVERSE_5     	ByteData022
#define F_REVERSE_6     	ByteData023
#define F_REVERSE_7     	ByteData024
#define F_REVERSE_8     	ByteData025



#define F_CH3_ENABLE    	ByteData014
#define F_CH4_ENABLE    	ByteData015

#define F_A_TYPE_NUM      	ByteData016


// 보정 기능 추가
#define F_SCR_CORRECT_V     	ByteData026
#define F_SCR_CORRECT_A     	ByteData027
#define F_SCR_CORRECT_S_MAIN    ByteData028
#define F_SCR_CORRECT_S0    	ByteData029
#define F_SCR_CORRECT_S1    	ByteData030
#define F_SCR_CORRECT_S2    	ByteData031
#define F_SCR_CORRECT_S3    	ByteData032
#define F_SCR_CORRECT_S4    	ByteData033
#define F_SCR_CORRECT_S5    	ByteData034
#define F_SCR_CORRECT_S6    	ByteData035
#define F_SCR_CORRECT_S7    	ByteData036


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

#define cF_ch0_use 			cF_ByteData(F_CH0_USE)
#define cF_ch1_use 			cF_ByteData(F_CH1_USE)
#define cF_ch2_use 			cF_ByteData(F_CH2_USE)
#define cF_ch3_use 			cF_ByteData(F_CH3_USE)
#define cF_ch4_use 			cF_ByteData(F_CH4_USE)
#define cF_ch5_use	 		cF_ByteData(F_CH5_USE) //
#define cF_ch6_use 			cF_ByteData(F_CH6_USE)
#define cF_ch7_use 			cF_ByteData(F_CH7_USE)

#define cF_ch3_enable 		cF_ByteData(F_CH3_ENABLE)
#define cF_ch4_enable 		cF_ByteData(F_CH4_ENABLE) // ※ iF -> cF

#define cF_A_type_num   	cF_ByteData(F_A_TYPE_NUM)

// 센서 정/역 설정 final 저장 값
#define cF_reverse_0    		cF_ByteData(F_REVERSE_0)
#define cF_reverse_1 			cF_ByteData(F_REVERSE_1)
#define cF_reverse_2 			cF_ByteData(F_REVERSE_2)
#define cF_reverse_3 			cF_ByteData(F_REVERSE_3)
#define cF_reverse_4 			cF_ByteData(F_REVERSE_4)
#define cF_reverse_5 			cF_ByteData(F_REVERSE_5)
#define cF_reverse_6 			cF_ByteData(F_REVERSE_6)
#define cF_reverse_7 			cF_ByteData(F_REVERSE_7)
#define cF_reverse_8 			cF_ByteData(F_REVERSE_8)



// 보정
#define cF_ch0_correctT 			cF_ByteData(F_SCR_CORRECT_S_MAIN)
#define cF_ch0_correctV 			cF_ByteData(F_SCR_CORRECT_V)
#define cF_ch0_correctA 			cF_ByteData(F_SCR_CORRECT_A)

#define cF_ch1_correctT 			cF_ByteData(F_SCR_CORRECT_S0)
#define cF_ch1_correctV 			cF_ByteData(F_SCR_CORRECT_S1)
#define cF_ch1_correctA 			cF_ByteData(F_SCR_CORRECT_S2)

#define cF_ch2_correctT 			cF_ByteData(F_SCR_CORRECT_S3)
#define cF_ch2_correctV 			cF_ByteData(F_SCR_CORRECT_S4)
#define cF_ch2_correctA 			cF_ByteData(F_SCR_CORRECT_S5)

#define cF_ch3_correctT 			cF_ByteData(F_SCR_CORRECT_S6)
#define cF_ch3_correctV 			cF_ByteData(F_SCR_CORRECT_S7)
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

