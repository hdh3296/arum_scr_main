
#include "loader_45k80_main.h"
#include "..\pwm.h"
#include "..\ad_conversion.h"
#include "..\main.h"

uint8_t UserSystemStatus;
uint8_t UserRam_8[VARIABLE_8_END];
uint16_t UserRam_16[VARIABLE_16_END];
uint32_t UserRam_32[VARIABLE_32_END];
Menu_Status MenuStatus[MAX_MENU];
uint16_t maxAmpMenuNum;


/////////////////////////////////////////////////////
// 설정 디포트 초기값
#define DFL_SETUP 0x55						//default val
#define DFL_VERSION 0						//version
#define DFL_SCR_GOAL_VOLTAGE 500 // 50.0V
#define DFL_SCR_GOAL_CURRENT 50 // 10.0A
#define DFL_SCR_GOAL_SENSOR  10250 // 1000mV 전위
// 센서 정/역 디폴트
#define DFL_REVERSE_0 0 // 0 : zinc
#define DFL_REVERSE_1 0
#define DFL_REVERSE_2 0
#define DFL_REVERSE_3 0
#define DFL_REVERSE_4 0
#define DFL_REVERSE_5 0
#define DFL_REVERSE_6 0
#define DFL_REVERSE_7 0
#define DFL_REVERSE_8 0

#define DFL_FOP_EN	1
#define DFL_1SRP_EN 0
#define DFL_2SOP_EN 0
#define DFL_3AOP_EN 0
#define DFL_4ARP_EN 0
#define DFL_UPR_EN  1
#define DFL_OPR_EN  0

// 1st
#define DFL_AMP_TYPE 0	 // 0 = 5A (테스트 용)
#define DFL_SRP_MAX 10100 // 7A
#define DFL_SRP_MIN 10101 // V
#define DFL_SRP_TIME 2000 // time
// 2st
#define DFL_SOP_MAX 10200 // V
#define DFL_SOP_MIN 9800 // 7A
#define DFL_SOP_TIME 2000 // V
// 3st
#define DFL_AOP_DUTY 20 // 7A
#define DFL_AOP_TIME 5000 // 7A
// 4st
#define DFL_ARP_DUTY 30 // 7A
#define DFL_ARP_TIME 5000 // 7A

#define DFL_UPR_SET 11000 // V
#define DFL_OPR_SET 9000 // 7A




// 보정
#define DFL_SCR_CORRECT_V	10000
#define DFL_SCR_CORRECT_A	10000
#define DFL_SCR_CORRECT_ch0	10000
#define DFL_SCR_CORRECT_ch1	10000
#define DFL_SCR_CORRECT_ch2	10000
#define DFL_SCR_CORRECT_ch3	10000
#define DFL_SCR_CORRECT_ch4	10000
#define DFL_SCR_CORRECT_ch5	10000
#define DFL_SCR_CORRECT_ch6	10000
#define DFL_SCR_CORRECT_ch7	10000
#define DFL_SCR_CORRECT_ch8	10000


#define DFL_CH3_CORRENT_A	200
#define DFL_CH4_CORRENT_T	20
#define DFL_CH4_CORRENT_V	200
#define DFL_CH4_CORRENT_A	200

bool positiveCorrT(uint8_t ch) {
/*
	보정 값이 + / - 여부 리턴
*/
	uint16_t corrT = heater[ch].corrTempSet;

	if (corrT >= SIGN_POSIT_T) {
		return 1;
	}
	return 0;
}

uint16_t getCorrentSetT(uint8_t ch) {
/*
	보정 값 리턴
*/
	uint16_t corrT = heater[ch].corrTempSet;
	return (corrT % 10);
}



uint16_t ByteType_DIGIT_STRING_EDIT_Set(uint16_t main_gr, uint16_t sub_gr, uint16_t divide) {
    MenuStatus[UserMenuSerialNm].M_EditGroupNm = main_gr;
    MenuStatus[UserMenuSerialNm].M_EditSubNm = sub_gr;
    MenuStatus[UserMenuSerialNm].M_EditCursor = 0;
    MenuStatus[UserMenuSerialNm].M_EditStatus = DIGIT_STRING_EDIT;
    MenuStatus[UserMenuSerialNm].M_EditStart = 3;
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDivide = divide;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 9;
    MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditDataType = BYTE_TYPE;

    if (MenuStatus[UserMenuSerialNm].M_EditDivide > 0) {
        MenuStatus[UserMenuSerialNm].M_EditShiftCnt = (MenuStatus[UserMenuSerialNm].M_EditShiftCnt + 1);
    }

    return (0);
}


uint16_t ByteType_DIGIT_EDIT_Set(uint16_t main_gr, uint16_t sub_gr, uint16_t divide) {
    MenuStatus[UserMenuSerialNm].M_EditGroupNm = main_gr;
    MenuStatus[UserMenuSerialNm].M_EditSubNm = sub_gr;
    MenuStatus[UserMenuSerialNm].M_EditCursor = 0;
    MenuStatus[UserMenuSerialNm].M_EditStatus = DIGIT_EDIT;
    MenuStatus[UserMenuSerialNm].M_EditStart = 3;
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDivide = divide;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 255;
    MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditDataType = BYTE_TYPE;

    if (MenuStatus[UserMenuSerialNm].M_EditDivide > 0) {
        MenuStatus[UserMenuSerialNm].M_EditShiftCnt = (MenuStatus[UserMenuSerialNm].M_EditShiftCnt + 1);
    }

    return (0);
}


uint16_t IntType_DIGIT_EDIT_Set(uint16_t main_gr, uint16_t sub_gr, uint16_t divide) {
    MenuStatus[UserMenuSerialNm].M_EditGroupNm = main_gr;
    MenuStatus[UserMenuSerialNm].M_EditSubNm = sub_gr;
    MenuStatus[UserMenuSerialNm].M_EditCursor = 0;
    MenuStatus[UserMenuSerialNm].M_EditStatus = DIGIT_EDIT;
    MenuStatus[UserMenuSerialNm].M_EditStart = 3;
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDivide = divide;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 65535;
    MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditDataType = INT_TYPE;

    if (MenuStatus[UserMenuSerialNm].M_EditDivide > 0) {
        MenuStatus[UserMenuSerialNm].M_EditShiftCnt = (MenuStatus[UserMenuSerialNm].M_EditShiftCnt + 1);
    }

    return (0);
}


// 그룹1 : 유저그룹
uint16_t G1_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group1
    /////////////////////////////////////////////////////////////////////
    UserMenuSerialNm = 0;
    main_gr = MAIN_GROUP01;
    sub_gr = SUB_GROUP01;
    /////////////////////////////////////////////////////////////////////
    //0 : 현재 상태 값 보기 설정
    /////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_DEFAULT_DSP;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
		GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *)
		DefaultMsgSel;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;
    /////////////////////////////////////////////////////////////////////
    //1 : 펌웨어 버전 정보
    /////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_VERSION;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;


    return (0);
}

// 그룹2 : 최대 한계 전압/전류 그리고 목표 전위(센서)
uint16_t G2_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;
    //////////////////////////////////////////////////////////////////////
    // main group2
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP02;
    sub_gr = SUB_GROUP01;
    /////////////////////////////////////////////////////////////////////
    // 목표 한계 전압 (정격 전압)
    /////////////////////////////////////////////////////////////////////
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 4;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 999; // 99.9V
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_GOAL_VOLTAGE;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
    /////////////////////////////////////////////////////////////////////
    // 목표 한계 전류 (정격 전류)
    /////////////////////////////////////////////////////////////////////
    maxAmpMenuNum = UserMenuSerialNm;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 3000; // 3000 => 300A
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_GOAL_CURRENT;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //sub group7 - ch0 목표 센서(부식) 예시) 1000 mV ★
    /////////////////////////////////////////////////////////////////////
    // ThisSelMenuNm => 4
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 12500; // +2500
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  7500; // -2500
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_GOAL_SENSOR;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

    return (0);
}

// 보정 설정
uint16_t G3_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group3
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP03;
    sub_gr = SUB_GROUP01;

    /////////////////////////////////////////////////////////////////////
    //scr : 전압  보정
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_V;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

    /////////////////////////////////////////////////////////////////////
    // 전류 보정
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_A;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;


	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널0
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널1
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch1;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널2
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch2;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널3
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch3;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널4
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch4;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널5
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch5;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널6
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch6;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널7
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch7;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //센서  보정 채널8
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch8;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;




    return (0);
}

// ZSU 서브 채널 8개 인에이블/디세이블 (use/not)
uint16_t G4_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group1
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP04;
    sub_gr = SUB_GROUP01;

	/////////////////////////////////////////////////////////////////////
	//서브 보드 - ch0 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH0_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//서브 보드 - ch1 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH1_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//서브 보드 - ch2 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH2_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//서브 보드 - ch3 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH3_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//서브 보드 - ch4 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH4_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//서브 보드 - ch5 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH5_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//서브 보드 - ch6 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH6_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//서브 보드 - ch7 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH7_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//메인 센서 ch8
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH8_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

    return (0);
}


// 센서 Zinc or CUCUSO4 선택
uint16_t G5_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group1
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP05;
    sub_gr = SUB_GROUP01;

	/////////////////////////////////////////////////////////////////////
	// 0 채널
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_0; // 데이어/플래쉬
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 1 채널
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_1;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 2 채널
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_2;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 3 채널
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_3;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 4 채널
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_4;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 5 채널
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_5;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 6 채널
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_6;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 7 채널
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_7;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 메인 채널 (8)
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_8;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;



    return (0);
}


//
uint16_t G6_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    main_gr = MAIN_GROUP06;
    sub_gr = SUB_GROUP01;
	/////////////////////////////////////////////////////////////////////
	// 총 11가지 전류 타입 선택 (5A ~ 300A)
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_AMP_TYPE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ampTypeList;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 11;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //SRP MAX
    /////////////////////////////////////////////////////////////////////
    // ThisSelMenuNm => ??
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 11999; // +1999
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  8001; // -1999
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SRP_MAX;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //SRP MIN
    /////////////////////////////////////////////////////////////////////
    // ThisSelMenuNm => ??
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 11999; // 10000 +
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  8001; //  0000 -
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SRP_MIN;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //SRP TIME
    /////////////////////////////////////////////////////////////////////
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 50000;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SRP_TIME;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;


	/////////////////////////////////////////////////////////////////////
    //SOP MAX
    /////////////////////////////////////////////////////////////////////
    // ThisSelMenuNm => ??
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 11999; // +1999
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  8001; // -1999
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SOP_MAX;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //SOP MIN
    /////////////////////////////////////////////////////////////////////
    // ThisSelMenuNm => ??
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 11999; // 10000 +
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  8001; //  0000 -
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SOP_MIN;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //SOP TIME
    /////////////////////////////////////////////////////////////////////
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 50000;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SOP_TIME;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //AOP DUTY
    /////////////////////////////////////////////////////////////////////
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 2;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 99;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_AOP_DUTY;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //AOP TIME
    /////////////////////////////////////////////////////////////////////
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 50000;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_AOP_TIME;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;


	/////////////////////////////////////////////////////////////////////
    //ARP DUTY
    /////////////////////////////////////////////////////////////////////
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 2;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 99;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_ARP_DUTY;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //ARP TIME
    /////////////////////////////////////////////////////////////////////
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 50000;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 0;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_ARP_TIME;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;


	/////////////////////////////////////////////////////////////////////
    //UPR set
    /////////////////////////////////////////////////////////////////////
    // ThisSelMenuNm => ??
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 11999; // +1999
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  8001; // -1999
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_UPR_SET;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //OPR set
    /////////////////////////////////////////////////////////////////////
    // ThisSelMenuNm => ??
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 11999; // +1999
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  8001; // -1999
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_OPR_SET;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

    return (0);
}




uint16_t G7_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    main_gr = MAIN_GROUP07;
    sub_gr = SUB_GROUP01;

/////////////////
// ---- FOP ~ OPR 총 7개 각 알람 ON/OFF 설정(세팅)
	/////////////////////////////////////////////////////////////////////
	// FOP
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_FOP_EN;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ldr_errorTestSetText;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;
	/////////////////////////////////////////////////////////////////////
	// 1SRP
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_1SRP_EN;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ldr_errorTestSetText;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;
	/////////////////////////////////////////////////////////////////////
	// 2SOP
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_2SOP_EN;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ldr_errorTestSetText;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;
	/////////////////////////////////////////////////////////////////////
	// 3AOP
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_3AOP_EN;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ldr_errorTestSetText;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;
	/////////////////////////////////////////////////////////////////////
	// 4ARP
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_4ARP_EN;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ldr_errorTestSetText;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;
	/////////////////////////////////////////////////////////////////////
	// UPR
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_UPR_EN;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ldr_errorTestSetText;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;
	/////////////////////////////////////////////////////////////////////
	// OPR
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_OPR_EN;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ldr_errorTestSetText;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	// end ----------------
	MenuStatus[UserMenuSerialNm].M_EditGroupNm = 0xff;
    return (0);
}


uint16_t G8_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    main_gr = MAIN_GROUP08;
    sub_gr = SUB_GROUP01;
    return (0);
}


uint16_t G9_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    main_gr = MAIN_GROUP09;
    sub_gr = SUB_GROUP01;
    return (0);
}


uint16_t G10_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    main_gr = MAIN_GROUP10;
    sub_gr = SUB_GROUP01;
    return (0);
}

// 로더 값 디폴트
uint16_t DefaultValueSet(void) {
    if (cF_INITIAL == DFL_SETUP) {
        return (0);
    } else {
        cSR_ByteData(F_VERSION) = DFL_VERSION;
        cSR_ByteData(F_INITIAL) = DFL_SETUP;
		// 센서 정/역 디폴트
        cSR_ByteData(F_REVERSE_0) = DFL_REVERSE_0;
        cSR_ByteData(F_REVERSE_1) = DFL_REVERSE_1;
        cSR_ByteData(F_REVERSE_2) = DFL_REVERSE_2;
        cSR_ByteData(F_REVERSE_3) = DFL_REVERSE_3;
        cSR_ByteData(F_REVERSE_4) = DFL_REVERSE_4;
        cSR_ByteData(F_REVERSE_5) = DFL_REVERSE_5;
        cSR_ByteData(F_REVERSE_6) = DFL_REVERSE_6;
        cSR_ByteData(F_REVERSE_7) = DFL_REVERSE_7;
        cSR_ByteData(F_REVERSE_8) = DFL_REVERSE_8;

        cSR_ByteData(F_FOP_EN)  = DFL_FOP_EN;
        cSR_ByteData(F_1SRP_EN) = DFL_1SRP_EN;
		cSR_ByteData(F_2SOP_EN) = DFL_2SOP_EN;
		cSR_ByteData(F_3AOP_EN) = DFL_3AOP_EN;
		cSR_ByteData(F_4ARP_EN) = DFL_4ARP_EN;
		cSR_ByteData(F_UPR_EN)  = DFL_UPR_EN;
		cSR_ByteData(F_OPR_EN)  = DFL_OPR_EN;

		// ch0
        cSR_ByteData(F_AMP_TYPE) = DFL_AMP_TYPE;
        cSR_ByteData(F_CH5_USE) = 1;
        cSR_ByteData(F_CH0_USE) = 1;
		// ch1
        cSR_ByteData(F_CH6_USE) = 1;
        cSR_ByteData(F_CH1_USE) = 1;
		// ch2
		cSR_ByteData(F_CH7_USE) = 1;
        cSR_ByteData(F_CH2_USE) = 1;
		// ch3
		cSR_ByteData(F_CH8_USE) = 1;
        cSR_ByteData(F_CH3_USE) = 1;
		// ch4
		cSR_ByteData(F_CH4_ENABLE) = 1;
        cSR_ByteData(F_CH4_USE) = 1;
		// 보정

        cSR_ByteData(F_CH3_CORRECT_A) = DFL_CH3_CORRENT_A;
        cSR_ByteData(F_CH4_CORRECT_T) = DFL_CH4_CORRENT_T;
        cSR_ByteData(F_CH4_CORRECT_V) = DFL_CH4_CORRENT_V;
        cSR_ByteData(F_CH4_CORRECT_A) = DFL_CH4_CORRENT_A;

        FlashBlockWr(0);

		// 두번째 블럭
		iSR_IntData(F_SCR_GOAL_VOLTAGE) = DFL_SCR_GOAL_VOLTAGE;
		iSR_IntData(F_SCR_GOAL_CURRENT) = DFL_SCR_GOAL_CURRENT;
        iSR_IntData(F_SCR_GOAL_SENSOR) 	= DFL_SCR_GOAL_SENSOR;

        iSR_IntData(F_SRP_MAX) = DFL_SRP_MAX;
        iSR_IntData(F_SRP_MIN) = DFL_SRP_MIN;
        iSR_IntData(F_SRP_TIME) = DFL_SRP_TIME;

        iSR_IntData(F_SOP_MAX) = DFL_SOP_MAX;
        iSR_IntData(F_SOP_MIN) = DFL_SOP_MIN;
        iSR_IntData(F_SOP_TIME) = DFL_SOP_TIME;

        iSR_IntData(F_AOP_DUTY) = DFL_AOP_DUTY;
        iSR_IntData(F_AOP_TIME) = DFL_AOP_TIME;
        iSR_IntData(F_ARP_DUTY) = DFL_ARP_DUTY;
        iSR_IntData(F_ARP_TIME) = DFL_ARP_TIME;

        iSR_IntData(F_UPR_SET) = DFL_UPR_SET;
        iSR_IntData(F_OPR_SET) = DFL_OPR_SET;

		iSR_IntData(F_SCR_CORRECT_V) = DFL_SCR_CORRECT_V;
		iSR_IntData(F_SCR_CORRECT_A) = DFL_SCR_CORRECT_A;
		iSR_IntData(F_SCR_CORRECT_ch0) = DFL_SCR_CORRECT_ch0;
		iSR_IntData(F_SCR_CORRECT_ch1) = DFL_SCR_CORRECT_ch1;
		iSR_IntData(F_SCR_CORRECT_ch2) = DFL_SCR_CORRECT_ch2;
		iSR_IntData(F_SCR_CORRECT_ch3) = DFL_SCR_CORRECT_ch3;
		iSR_IntData(F_SCR_CORRECT_ch4) = DFL_SCR_CORRECT_ch4;
		iSR_IntData(F_SCR_CORRECT_ch5) = DFL_SCR_CORRECT_ch5;
		iSR_IntData(F_SCR_CORRECT_ch6) = DFL_SCR_CORRECT_ch6;
		iSR_IntData(F_SCR_CORRECT_ch7) = DFL_SCR_CORRECT_ch7;
		iSR_IntData(F_SCR_CORRECT_ch8) = DFL_SCR_CORRECT_ch8;

		FlashBlockWr(1);
    }

    return (0);
}


uint16_t Menu_Status_Set(void) {
    G1_Menu_Status_Set();
    G2_Menu_Status_Set();
    G3_Menu_Status_Set();
    G4_Menu_Status_Set();
    G5_Menu_Status_Set();
    G6_Menu_Status_Set();
    G7_Menu_Status_Set();
    G8_Menu_Status_Set();
    G9_Menu_Status_Set();
    G10_Menu_Status_Set();
    DefaultValueSet();
    return (0);
}











uint8_t getAscii(uint8_t value) {
    uint8_t ascii;

    if (value < 0x0a) {
        ascii = value + '0';
    } else {
        ascii = value + '7';
    }

    return ascii;
}



void ldr_loadStatViewAsciiBuffer_number(uint8_t ch) {
/*
	현재 온도 표시 ex. 30 도
*/
    uint16_t nowTemp, correct_value;
    uint8_t ascii_1000, ascii_100, ascii_10, ascii_1;

	nowTemp = getCurTempTo(heater[ch].db_nowTemp_mV);
	// 현재 아날로그 입력 대비 현재 온도 값을 얻어온다.
	// ex. 30 도 <= 1000 아날로그 입력
	correct_value = getCorrentSetT(ch);
	// 현재 온도 + 보정 값
	// ex. 30 + 2 => 32 도 (보정된 값)
	if (positiveCorrT(ch)) {
    	nowTemp = nowTemp + correct_value;
	} else {
		nowTemp = nowTemp - correct_value;
	}


    ascii_1000 = nowTemp / 1000;
    nowTemp = nowTemp % 1000;
    ascii_100   = nowTemp / 100;
    nowTemp = nowTemp % 100;
    ascii_10    =   nowTemp / 10;
    nowTemp = nowTemp % 10;
    ascii_1     = nowTemp;

    ascii_1000 = getAscii(ascii_1000);
    ascii_100 = getAscii(ascii_100);
    ascii_10 = getAscii(ascii_10);
    ascii_1 = getAscii(ascii_1);

    new485Ladder[SECONDLINE_BASE + 11] = ascii_10;
    new485Ladder[SECONDLINE_BASE + 12] = ascii_1;
	new485Ladder[SECONDLINE_BASE + 13] = 0x60;
    new485Ladder[SECONDLINE_BASE + 14] = 'C';
    new485Ladder[SECONDLINE_BASE + 15] = ' ';
}


void ldr_loadStatViewAsciiBuffer_shortErr(uint8_t ch) {

    new485Ladder[SECONDLINE_BASE + 11] = 'E';
    new485Ladder[SECONDLINE_BASE + 12] = 'r';
	new485Ladder[SECONDLINE_BASE + 13] = 'r';
    new485Ladder[SECONDLINE_BASE + 14] = ' ';
    new485Ladder[SECONDLINE_BASE + 15] = ' ';


}

void ldr_loadStatViewAsciiBuffer_breakErr(uint8_t ch) {
    new485Ladder[SECONDLINE_BASE + 11] = 'E';
    new485Ladder[SECONDLINE_BASE + 12] = 'r';
	new485Ladder[SECONDLINE_BASE + 13] = 'r';
    new485Ladder[SECONDLINE_BASE + 14] = ' ';
    new485Ladder[SECONDLINE_BASE + 15] = ' ';



}


void ldr_loadStatViewAsciiBuffer(uint8_t ch) {
	uint8_t state = heater[ch].db_tempShortBreak;
	switch (state) {
		case TERR_BREAK:
			ldr_loadStatViewAsciiBuffer_breakErr(ch);
			return;
		case TERR_SHORT:
			ldr_loadStatViewAsciiBuffer_shortErr(ch);
			return;
		case TERR_NONE:
			// pass
		default:
			ldr_loadStatViewAsciiBuffer_number(ch);
			break;
	}
}

void initLdrLineBuf(void) {
	uint16_t i;
	for (i = 0; i < 16; i++) {
		 new485Ladder[SECONDLINE_BASE + i] = ' ';
	 }
}

void ldr_setFirstLine(uint8_t ch) {
	uint8_t ascii_chNumber;

	ascii_chNumber = getAscii(ch+1);
    new485Ladder[FIRSTLINE_BASE + 0] = 'C';
    new485Ladder[FIRSTLINE_BASE + 1] = 'H';
    new485Ladder[FIRSTLINE_BASE + 2] = '-';
    new485Ladder[FIRSTLINE_BASE + 3] = getAscii(ch+1);
    new485Ladder[FIRSTLINE_BASE + 4] = ':';
    new485Ladder[FIRSTLINE_BASE + 5] = 'S';
    new485Ladder[FIRSTLINE_BASE + 6] = 'T';
    new485Ladder[FIRSTLINE_BASE + 7] = 'A';
    new485Ladder[FIRSTLINE_BASE + 8] = 'T';
    new485Ladder[FIRSTLINE_BASE + 9] = 'U';
    new485Ladder[FIRSTLINE_BASE + 10] = 'S';
    new485Ladder[FIRSTLINE_BASE + 11] = ' ';
    new485Ladder[FIRSTLINE_BASE + 12] = ' ';
	new485Ladder[FIRSTLINE_BASE + 13] = ' ';
	new485Ladder[FIRSTLINE_BASE + 14] = ' ';
	new485Ladder[FIRSTLINE_BASE + 15] = ' ';
}

// 현재 상태 센서 값 표시
void ldr_setSecondLine(uint8_t ch) {
    uint16_t num;
    uint8_t  ascii_1000, ascii_100, ascii_10, ascii_1;

    num = getFinalOneTopMaxSensor_micom_mV(); // <<< #1025
    ascii_1000   = num / 1000;
    num = num % 1000;
    ascii_100   = num / 100;
    num = num % 100;
    ascii_10    =   num / 10;
    num = num % 10;
    ascii_1     = num;

	ascii_1000 = getAscii(ascii_1000);
    ascii_100 = getAscii(ascii_100);
    ascii_10 = getAscii(ascii_10);
    ascii_1 = getAscii(ascii_1);

    new485Ladder[SECONDLINE_BASE + 0] = ascii_1000;
    new485Ladder[SECONDLINE_BASE + 1] = ascii_100;
    new485Ladder[SECONDLINE_BASE + 2] = ascii_10;
    new485Ladder[SECONDLINE_BASE + 3] = ascii_1;
    new485Ladder[SECONDLINE_BASE + 4] = '/';


    num = getFinalOneLowMinSensor_micom_mV();
    ascii_1000   = num / 1000;
    num = num % 1000;
    ascii_100   = num / 100;
    num = num % 100;
    ascii_10    =   num / 10;
    num = num % 10;
    ascii_1     = num;

	ascii_1000 = getAscii(ascii_1000);
    ascii_100 = getAscii(ascii_100);
    ascii_10 = getAscii(ascii_10);
    ascii_1 = getAscii(ascii_1);


	new485Ladder[SECONDLINE_BASE + 5] = ascii_1000;
	new485Ladder[SECONDLINE_BASE + 6] = ascii_100;
	new485Ladder[SECONDLINE_BASE + 7] = ascii_10;
	new485Ladder[SECONDLINE_BASE + 8] = ascii_1;
	new485Ladder[SECONDLINE_BASE + 9] = '/';
	new485Ladder[SECONDLINE_BASE + 10] = '/';


	// 온도

	ldr_loadStatViewAsciiBuffer(ch);

}

void ldr_viewState(uint8_t ch) {

	initLdrLineBuf();
	ldr_setFirstLine(ch);
	ldr_setSecondLine(ch);

}

//////////////////////////////////////////
//////////////////////////////////////////
uint16_t Default_Cur_State_Display(void) {
    uint16_t i, j;

    j = UserSystemStatus;

    for (i = 0; i < 16; i++) {
        new485Ladder[SECONDLINE_BASE + i] = StatusMessage[j][i];
    }


    return (0);
}


uint16_t Nm_Display(void) {
    new485Ladder[SECONDLINE_BASE + 0] = (CurKeyPoint / 10) + '0';
    new485Ladder[SECONDLINE_BASE + 1] = (CurKeyPoint % 10) + '0';
    CurMenuStatus.M_EditStart = 3;
    return (0);
}


uint16_t getShowIndex(void) {
	static uint16_t index;

		switch (LoaderKey) {
			case	UPKEY:
				if (index < 4) index++;
				break;
			case	DOWNKEY:
				if (index != 0) index--;
				break;
		}
		LoaderKey = '0';
		return index;
}



void loadTxLdrBuf_forDutycycle(uint8_t ch) {
    uint16_t i;
    uint16_t analog;
    uint8_t ascii_1000, ascii_100, ascii_10, ascii_1;
	uint8_t ascii_chNumber;

    for (i = 0; i < 16; i++) {
        new485Ladder[SECONDLINE_BASE + i] = ' ';
    }

    analog = DutyCyclex[ch];   // <<<---

    ascii_1000 = analog / 1000;
    analog = analog % 1000;
    ascii_100   = analog / 100;
    analog = analog % 100;
    ascii_10    =   analog / 10;
    analog = analog % 10;
    ascii_1     = analog;

    ascii_1000 = getAscii(ascii_1000);
    ascii_100 = getAscii(ascii_100);
    ascii_10 = getAscii(ascii_10);
    ascii_1 = getAscii(ascii_1);

	ascii_chNumber = getAscii(ch+1);
    new485Ladder[FIRSTLINE_BASE + 0] = 'S';
    new485Ladder[FIRSTLINE_BASE + 1] = '/';
    new485Ladder[FIRSTLINE_BASE + 2] = 'C';
    new485Ladder[FIRSTLINE_BASE + 3] = 'H';
    new485Ladder[FIRSTLINE_BASE + 4] = getAscii(ch+1);
    new485Ladder[FIRSTLINE_BASE + 5] = '-';
    new485Ladder[FIRSTLINE_BASE + 6] = 'D';
    new485Ladder[FIRSTLINE_BASE + 7] = 'u';
    new485Ladder[FIRSTLINE_BASE + 8] = 't';
    new485Ladder[FIRSTLINE_BASE + 9] = 'y';
    new485Ladder[FIRSTLINE_BASE + 10] = 'c';
    new485Ladder[FIRSTLINE_BASE + 11] = 'y';
    new485Ladder[FIRSTLINE_BASE + 12] = 'c';
    new485Ladder[FIRSTLINE_BASE + 13] = 'l';
    new485Ladder[FIRSTLINE_BASE + 14] = 'e';

    new485Ladder[SECONDLINE_BASE + 0] = ascii_1000;
    new485Ladder[SECONDLINE_BASE + 1] = ascii_100;
    new485Ladder[SECONDLINE_BASE + 2] = ascii_10;
    new485Ladder[SECONDLINE_BASE + 3] = ascii_1;
    new485Ladder[SECONDLINE_BASE + 4] = ' ';
    new485Ladder[SECONDLINE_BASE + 5] = ' ';
}





//////////================================
// ch0 현재 상태 로더에서 보기 위한 함수
void dsplayManyStateInfo_allCH(void) {
/*
    로더 초기화면에 현재 상태값 보여주기 위한 함수
*/
	uint16_t showIndex = getShowIndex();

	switch (showIndex) {
		case 0:
            ldr_viewState(0);
			break;
		case 1:
            ldr_viewState(1);
			break;
        case 2:
            ldr_viewState(2);
            break;
        case 3:
            ldr_viewState(3);
            break;
        case 4:
            ldr_viewState(4);
            break;
		default:
			break;
	}
}

void dsplayManyStateInfo_allCH__copy(void) {
/*
    로더 초기화면에 현재 상태값 보여주기 위한 함수
*/
	uint16_t showIndex = getShowIndex();

	switch (showIndex) {
		case 0:
            ldr_viewState(0);
			break;
		case 1:
            ldr_viewState(1);
			break;
        case 2:
            ldr_viewState(2);
            break;
        case 3:
            ldr_viewState(3);
            break;
        case 4:
            ldr_viewState(4);
            break;
		default:
			break;
	}
}



void dsplayManyStateInfo_pwm(void) {
/*
    로더 초기화면에 현재 상태값 보여주기 위한 함수
*/
	uint16_t showIndex = getShowIndex();

	switch (showIndex) {
		case 0:
            loadTxLdrBuf_forDutycycle(0);
			break;
		case 1:
            loadTxLdrBuf_forDutycycle(1);
			break;
        case 2:
            loadTxLdrBuf_forDutycycle(2);
            break;
        case 3:
            loadTxLdrBuf_forDutycycle(3);
            break;
        case 4:
            loadTxLdrBuf_forDutycycle(4);
            break;
		default:
			break;
	}
}



uint16_t DefaultDisplay(void) {
/*
	로더 초기화면 / 디폴트

	(ㅋㅋ 여기에 제목을 적음으로써 검색하기 쉽구나. !!! <== 검색 용도)
*/
    uint16_t i, j, k;

    k = cF_DEFAULT_DSP;

    if (BefDspMode != k) {
        BefDspMode = k;

        for (i = 0; i < 16; i++) {
            new485Ladder[SECONDLINE_BASE + i] = ' ';
        }
    }

    if (UserSystemStatus == M_NONE) { // #1027 ??
        for (i = 0; i < 16; i++) {
            new485Ladder[i + 2] = DefaultMsg[1][i];
        }
    } else {
        for (i = 0; i < 16; i++) {
            new485Ladder[i + 2] = DefaultMsg[0][i];
        }
    }

    switch (k) {
        case 0:
			Default_Cur_State_Display();
            break;
        case 1:
			dsplayManyStateInfo_allCH__copy();
            break;
        default:
			dsplayManyStateInfo_pwm();
            break;
    }

    return (0);
}


/////////////////////////////////////
