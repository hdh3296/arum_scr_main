
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
uint16_t maxCorrectAmpMenuNum;


/////////////////////////////////////////////////////
// �δ� ���� ����Ʈ �ʱ�ȭ �� ����
#define DFL_SETUP 0x55						//default val
#define DFL_VERSION 0						//version
#define DFL_SCR_GOAL_VOLTAGE 500 // 50.0V
#define DFL_SCR_GOAL_CURRENT 50 // 10.0A
#define DFL_SCR_GOAL_SENSOR  10250 // 1000mV ����
// sensor type
#define DFL_REVERSE_0 0 // 0 : zinc
#define DFL_REVERSE_1 0
#define DFL_REVERSE_2 0
#define DFL_REVERSE_3 0
#define DFL_REVERSE_4 0
#define DFL_REVERSE_5 0
#define DFL_REVERSE_6 0
#define DFL_REVERSE_7 0
#define DFL_REVERSE_8 0
// sensor use/nouse
#define DEF_CH0_USE 1
#define DEF_CH1_USE 0
#define DEF_CH2_USE 0
#define DEF_CH3_USE 0
#define DEF_CH4_USE 0
#define DEF_CH5_USE 0
#define DEF_CH6_USE 0
#define DEF_CH7_USE 0
#define DEF_CH8_USE 0

#define DFL_FOP_EN	1
#define DFL_1SRP_EN 1
#define DFL_2SOP_EN 1
#define DFL_3AOP_EN 1
#define DFL_4ARP_EN 1
#define DFL_UPR_EN  1
#define DFL_OPR_EN  0

// 1st SRP
#define DFL_AMP_TYPE 0	 // 0 = 5A (�׽�Ʈ ��)
#define DFL_SRP_MAX 10200 // 7A
#define DFL_SRP_MIN 9800 // V
#define DFL_SRP_TIME 2000 // time
// 2st sop (���� �ܼ�)
#define DFL_SOP_MAX 10100 // V
#define DFL_SOP_MIN 9900 // 7A
#define DFL_SOP_TIME 2000 // V
// 3st
#define DFL_AOP_DUTY 20 // 7A
#define DFL_AOP_TIME 5000 // 7A
// 4st
#define DFL_ARP_DUTY 30 // 7A
#define DFL_ARP_TIME 5000 // 7A

#define DFL_UPR_SET 10350 // V under protection : +350
#define DFL_OPR_SET 8600 // 7A over protectio : -1400

// ����
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
	���� ���� + / - ���� ����
*/
	uint16_t corrT = heater[ch].corrTempSet;

	if (corrT >= SIGN_POSIT_T) {
		return 1;
	}
	return 0;
}

uint16_t getCorrentSetT(uint8_t ch) {
/*
	���� �� ����
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


// �׷�1 : �����׷�
uint16_t G1_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group1
    /////////////////////////////////////////////////////////////////////
    UserMenuSerialNm = 0;
    main_gr = MAIN_GROUP01;
    sub_gr = SUB_GROUP01;
    /////////////////////////////////////////////////////////////////////
    //0 : ���� ���� �� ���� ����
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
    //1 : �߿��� ���� ����
    /////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_VERSION;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;


    return (0);
}

// �׷�2 : ����/����/��ǥ����
uint16_t G2_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;
    //////////////////////////////////////////////////////////////////////
    // main group2
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP02;
    sub_gr = SUB_GROUP01;
    /////////////////////////////////////////////////////////////////////
    // ��ǥ �Ѱ� ���� (���� ����)
    /////////////////////////////////////////////////////////////////////
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 4;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 999; // 99.9V
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_GOAL_VOLTAGE;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
    /////////////////////////////////////////////////////////////////////
    // ��ǥ �Ѱ� ���� (���� ����)
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
    // ��ǥ ����(�ν�) ����) 1000 mV ��
    /////////////////////////////////////////////////////////////////////
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

// ���� ����
uint16_t G3_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group3
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP03;
    sub_gr = SUB_GROUP01;

    /////////////////////////////////////////////////////////////////////
    //scr : ����  ����
    /////////////////////////////////////////////////////////////////////
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 6;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_V;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

    /////////////////////////////////////////////////////////////////////
    // ���� ����
    /////////////////////////////////////////////////////////////////////
    maxCorrectAmpMenuNum = UserMenuSerialNm;
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 6;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  9900; // -100
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_A;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
													GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// CH0 : ����
	/////////////////////////////////////////////////////////////////////
	bThisMenuSaver[UserMenuSerialNm] = 1;
	IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 10100; // +100
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =	9900; // -100
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_ch0;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
    // CH1
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
    // CH2
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
    // CH3
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
    // CH4
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
    // CH5
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
    // CH6
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
    // CH7
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
    // CH8
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

// ZSU ���� ä�� 8�� �ο��̺�/���̺� (use/not)
uint16_t G4_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group1
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP04;
    sub_gr = SUB_GROUP01;


	/////////////////////////////////////////////////////////////////////
	// use/nouse CH0 (main)
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH0_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// use/nouse CH1
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH1_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// use/nouse CH2
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH2_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// use/nouse CH3
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH3_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// use/nouse CH4
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH4_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// use/nouse CH5
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH5_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// use/nouse CH6
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH6_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// use/nouse CH7
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH7_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// use/nouse CH8
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


// Ÿ�� ���� : Zinc or CUCUSO4 ����
uint16_t G5_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group1
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP05;
    sub_gr = SUB_GROUP01;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH0
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_0; // ���̾�/�÷���
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH1
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_1;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH2
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_2;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH3
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_3;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH4
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_4;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH5
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_5;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH6
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_6;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH7
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_7;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// sensor type select : CH8
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_8;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// �� 11���� ���� Ÿ�� ���� (5A ~ 300A)
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_AMP_TYPE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ampTypeList;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 11;
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
    //SRP MAX
    /////////////////////////////////////////////////////////////////////
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
// ---- FOP ~ OPR �� 7�� �� �˶� ON/OFF ����(����)
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

// �δ� �� ����Ʈ
uint16_t DefaultValueSet(void) {
    if (cF_INITIAL == DFL_SETUP) {
        return (0);
    } else {
        cSR_ByteData(F_VERSION) = DFL_VERSION;
        cSR_ByteData(F_INITIAL) = DFL_SETUP;

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

        cSR_ByteData(F_AMP_TYPE) = DFL_AMP_TYPE;

        cSR_ByteData(F_CH0_USE) = DEF_CH0_USE;
        cSR_ByteData(F_CH1_USE) = DEF_CH1_USE;
        cSR_ByteData(F_CH2_USE) = DEF_CH2_USE;
        cSR_ByteData(F_CH3_USE) = DEF_CH3_USE;
        cSR_ByteData(F_CH4_USE) = DEF_CH4_USE;
        cSR_ByteData(F_CH5_USE) = DEF_CH5_USE;
        cSR_ByteData(F_CH6_USE) = DEF_CH6_USE;
		cSR_ByteData(F_CH7_USE) = DEF_CH7_USE;
		cSR_ByteData(F_CH8_USE) = DEF_CH8_USE;

		cSR_ByteData(F_CH4_ENABLE) = 1;
        cSR_ByteData(F_CH3_CORRECT_A) = DFL_CH3_CORRENT_A;
        cSR_ByteData(F_CH4_CORRECT_T) = DFL_CH4_CORRENT_T;
        cSR_ByteData(F_CH4_CORRECT_V) = DFL_CH4_CORRENT_V;
        cSR_ByteData(F_CH4_CORRECT_A) = DFL_CH4_CORRENT_A;

        FlashBlockWr(0);

		// �ι�° ��
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
	���� �µ� ǥ�� ex. 30 ��
*/
    uint16_t nowTemp, correct_value;
    uint8_t ascii_1000, ascii_100, ascii_10, ascii_1;

	nowTemp = getCurTempTo(heater[ch].db_nowTemp_mV);
	// ���� �Ƴ��α� �Է� ��� ���� �µ� ���� ���´�.
	// ex. 30 �� <= 1000 �Ƴ��α� �Է�
	correct_value = getCorrentSetT(ch);
	// ���� �µ� + ���� ��
	// ex. 30 + 2 => 32 �� (������ ��)
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


	// �µ�

	ldr_loadStatViewAsciiBuffer(ch);

}

void ldr_viewState(uint8_t ch) {

	initLdrLineBuf();
	ldr_setFirstLine(ch);
	ldr_setSecondLine(ch);

}

void loadTxLdrBuf_ldrdata_V(void) {
    uint16_t i;
    uint16_t num, nowIn_mV;
    uint8_t  ascii_1000, ascii_100, ascii_10, ascii_1;
	uint32_t signalNumber[2];
	uint8_t sign;


    for (i = 0; i < 16; i++) {
		new485Ladder[FIRSTLINE_BASE + i] = ' ';
        new485Ladder[SECONDLINE_BASE + i] = ' ';
    }
    new485Ladder[FIRSTLINE_BASE + 0] = 'V';
    new485Ladder[FIRSTLINE_BASE + 1] = ':';
    new485Ladder[FIRSTLINE_BASE + 2] = ' ';

	num = scr.nowVoltage_micom_mV;
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
    new485Ladder[FIRSTLINE_BASE + 3] = ascii_1000;
    new485Ladder[FIRSTLINE_BASE + 4] = ascii_100;
    new485Ladder[FIRSTLINE_BASE + 5] = ascii_10;
    new485Ladder[FIRSTLINE_BASE + 6] = ascii_1;

	// ���� : ù��° �� - ���� ���� �����Ĵ� (������) �� ǥ��
	nowIn_mV = num = getCorrectedNowIn_micomMV_voltage(scr.nowVoltage_micom_mV,
														iF_correct_V_user);
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

	// ���п� ���� ���� �δ� ���� ���� �� ǥ�� �ϱ�
	getSignNumberByLdrDigit(signalNumber, iF_correct_V_user);
	if (signalNumber[0] == SIGN_PLUS) {
		sign = '+';
	} else sign = '-';
	new485Ladder[SECONDLINE_BASE + 5] = sign;

    num = signalNumber[1];
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

	new485Ladder[SECONDLINE_BASE + 6] = ascii_100;
	new485Ladder[SECONDLINE_BASE + 7] = ascii_10;
	new485Ladder[SECONDLINE_BASE + 8] = '.';
	new485Ladder[SECONDLINE_BASE + 9] = ascii_1;
	new485Ladder[SECONDLINE_BASE + 10] = '/';

	// ���� : ���� ���� ǥ���ϱ� (������ ����)
	num = getVoltage_RealData_FromMicomMV(nowIn_mV);
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
	new485Ladder[SECONDLINE_BASE + 11] = ascii_1000;
	new485Ladder[SECONDLINE_BASE + 12] = ascii_100;
	new485Ladder[SECONDLINE_BASE + 13] = ascii_10;
	new485Ladder[SECONDLINE_BASE + 14] = '.';
	new485Ladder[SECONDLINE_BASE + 15] = ascii_1;


}


void loadTxLdrBuf_ldrdata_A(void) {
    uint16_t i;
    uint16_t num, nowIn_mV;
    uint8_t  ascii_1000, ascii_100, ascii_10, ascii_1;
	uint32_t signalNumber[2];
	uint8_t sign;


    for (i = 0; i < 16; i++) {
		new485Ladder[FIRSTLINE_BASE + i] = ' ';
        new485Ladder[SECONDLINE_BASE + i] = ' ';
    }
    new485Ladder[FIRSTLINE_BASE + 0] = 'A';
    new485Ladder[FIRSTLINE_BASE + 1] = ':';
    new485Ladder[FIRSTLINE_BASE + 2] = ' ';

	num = scr.nowAdAmp_micom_mV;
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
    new485Ladder[FIRSTLINE_BASE + 3] = ascii_1000;
    new485Ladder[FIRSTLINE_BASE + 4] = ascii_100;
    new485Ladder[FIRSTLINE_BASE + 5] = ascii_10;
    new485Ladder[FIRSTLINE_BASE + 6] = ascii_1;


	// ���� Amp  : ù��°  , �����Ĵ� ǥ��
	nowIn_mV = num = getCorrectedNowIn_micomMV_Amp(scr.nowAdAmp_micom_mV,
													iF_correct_A_user,
													cF_amp_type);
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


	// ���� ���� ���� �δ� ���� �� ǥ���ϱ�
	getSignNumberByLdrDigit(signalNumber, iF_correct_A_user);
	if (signalNumber[0] == SIGN_PLUS) {
		sign = '+';
	} else sign = '-';
	new485Ladder[SECONDLINE_BASE + 5] = sign;

    num = signalNumber[1];
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
	new485Ladder[SECONDLINE_BASE + 6] = ascii_100;
	new485Ladder[SECONDLINE_BASE + 7] = ascii_10;
	new485Ladder[SECONDLINE_BASE + 8] = '.';
	new485Ladder[SECONDLINE_BASE + 9] = ascii_1;
	new485Ladder[SECONDLINE_BASE + 10] = '/';

	// 3] ���� Amp : ���� ���� ǥ���ϱ�
	num = getAmp_RealData_FromMicomMV(nowIn_mV, cF_amp_type); // <<<
	// ���� : ���� ���� ǥ���ϱ� (������ ����)
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
	new485Ladder[SECONDLINE_BASE + 11] = ascii_1000;
	new485Ladder[SECONDLINE_BASE + 12] = ascii_100;
	new485Ladder[SECONDLINE_BASE + 13] = ascii_10;
	new485Ladder[SECONDLINE_BASE + 14] = '.';
	new485Ladder[SECONDLINE_BASE + 15] = ascii_1;

}

void loadTxLdrBuf_ldrdata_Volume(void) {
    uint16_t i;
    uint16_t num;
    uint8_t  ascii_1000, ascii_100, ascii_10, ascii_1;


    for (i = 0; i < 16; i++) {
		new485Ladder[FIRSTLINE_BASE + i] = ' ';
        new485Ladder[SECONDLINE_BASE + i] = ' ';
    }
    new485Ladder[FIRSTLINE_BASE + 0] = 'V';
    new485Ladder[FIRSTLINE_BASE + 1] = 'o';
    new485Ladder[FIRSTLINE_BASE + 2] = 'l';
    new485Ladder[FIRSTLINE_BASE + 3] = 'u';
    new485Ladder[FIRSTLINE_BASE + 4] = 'm';
    new485Ladder[FIRSTLINE_BASE + 5] = 'e';
    new485Ladder[FIRSTLINE_BASE + 6] = ':';
    new485Ladder[FIRSTLINE_BASE + 7] = ' ';
    new485Ladder[FIRSTLINE_BASE + 8] = ' ';
    new485Ladder[FIRSTLINE_BASE + 9] = ' ';
    new485Ladder[FIRSTLINE_BASE + 10] = ' ';
    new485Ladder[FIRSTLINE_BASE + 11] = ' ';
    new485Ladder[FIRSTLINE_BASE + 12] = ' ';
    new485Ladder[FIRSTLINE_BASE + 13] = ' ';
    new485Ladder[FIRSTLINE_BASE + 14] = ' ';

    num = scr.nowAdVolume_micom_mV;
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
    new485Ladder[SECONDLINE_BASE + 4] = ' ';
}

void loadTxLdrBuf_ldrdata_sensor(uint8_t ch) {
	uint8_t c;
    uint16_t i;
    uint16_t num;
    uint8_t  ascii_1000, ascii_100, ascii_10, ascii_1;
	uint32_t signalNumber[2];
	uint8_t sign;


    for (i = 0; i < 16; i++) {
		new485Ladder[FIRSTLINE_BASE + i] = ' ';
        new485Ladder[SECONDLINE_BASE + i] = ' ';
    }
    new485Ladder[FIRSTLINE_BASE + 0] = 'C';
    new485Ladder[FIRSTLINE_BASE + 1] = 'H';
    new485Ladder[FIRSTLINE_BASE + 2] = getAscii(ch);
	new485Ladder[FIRSTLINE_BASE + 3] = ':';

	// use/nouse ǥ�� �ϱ�
	if (isSensorUseNo(ch)) {
		c = 'Y';
	} else c = 'N';
	new485Ladder[FIRSTLINE_BASE + 4] = c;
	new485Ladder[FIRSTLINE_BASE + 5] = '/';

	num = db_sunsu_sensor_0_8_micomMV[ch];
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
    new485Ladder[FIRSTLINE_BASE + 6] = ascii_1000;
    new485Ladder[FIRSTLINE_BASE + 7] = ascii_100;
    new485Ladder[FIRSTLINE_BASE + 8] = ascii_10;
    new485Ladder[FIRSTLINE_BASE + 9] = ascii_1;



	// ���� �����Ĵ� ǥ��
    num = db_corrected_final_sensor_0_8_micomMV[ch];   // <<<---
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

	// ���� ���� �� ǥ��
	getSignNumberByLdrDigit(signalNumber, getCorrectedLdrSet_ch0_ch8(ch));
	if (signalNumber[0] == SIGN_PLUS) {
		sign = '+';
	} else sign = '-';
	new485Ladder[SECONDLINE_BASE + 5] = sign;
    num = signalNumber[1];
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
	new485Ladder[SECONDLINE_BASE + 6] = ascii_1000;
	new485Ladder[SECONDLINE_BASE + 7] = ascii_100;
	new485Ladder[SECONDLINE_BASE + 8] = ascii_10;
	new485Ladder[SECONDLINE_BASE + 9] = ascii_1;
	new485Ladder[SECONDLINE_BASE + 10] = '/';

	// ���� : ���� ���� ���� �� �����ֱ�
	getSignNumberByNowInSensor(signalNumber, db_corrected_final_sensor_0_8_micomMV[ch]);
	if (signalNumber[0] == SIGN_PLUS) {
		sign = '+';
	} else sign = '-';
	new485Ladder[SECONDLINE_BASE + 11] = sign;
    num = signalNumber[1];
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
	new485Ladder[SECONDLINE_BASE + 12] = ascii_1000;
	new485Ladder[SECONDLINE_BASE + 13] = ascii_100;
	new485Ladder[SECONDLINE_BASE + 14] = ascii_10;
	new485Ladder[SECONDLINE_BASE + 15] = ascii_1;

}



// ���� ���� ū��/������  ǥ���ϱ�
void loadTxLdrBuf_ldrdata_sensor_TopLow(void) {
    uint16_t i;
    uint16_t num;
    uint8_t  ascii_1000, ascii_100, ascii_10, ascii_1;
	uint32_t signalNumber[2];
	uint8_t sign;


    for (i = 0; i < 16; i++) {
		new485Ladder[FIRSTLINE_BASE + i] = ' ';
        new485Ladder[SECONDLINE_BASE + i] = ' ';
    }
    new485Ladder[FIRSTLINE_BASE + 0] = 'M';
    new485Ladder[FIRSTLINE_BASE + 1] = 'A';
    new485Ladder[FIRSTLINE_BASE + 2] = 'X';
	new485Ladder[FIRSTLINE_BASE + 3] = '/';
	new485Ladder[FIRSTLINE_BASE + 4] = 'M';
	new485Ladder[FIRSTLINE_BASE + 5] = 'I';
	new485Ladder[FIRSTLINE_BASE + 6] = 'N';

	// ������ �� �ι�° �ٿ� ǥ��
	num = getFinalOneTopMaxSensor_micom_mV();
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


uint16_t getShowIndex(uint8_t maxmenu) {
	static uint16_t index;

		switch (LoaderKey) {
			case	UPKEY:
				if (index < maxmenu) index++;
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

void dsplayInDataState(void) {
/*
	== ���� ���� ������ ���� �Լ� ==
    �δ� �ʱ�ȭ�鿡 ���� ���°� �����ֱ� ���� �Լ�
*/
	uint8_t maxMenuIndex = 12;
	uint16_t num = getShowIndex(maxMenuIndex);

	switch (num) {
		case 0:
            loadTxLdrBuf_ldrdata_sensor(num); // ch0 : ����
			break;
        case 1:
            loadTxLdrBuf_ldrdata_sensor(num); // ch1 : ZSU
            break;
        case 2:
            loadTxLdrBuf_ldrdata_sensor(num); // ch2
            break;
        case 3:
            loadTxLdrBuf_ldrdata_sensor(num); // ch3
			break;
		case 4:
			loadTxLdrBuf_ldrdata_sensor(num); // ch4
			break;
		case 5:
			loadTxLdrBuf_ldrdata_sensor(num); // ch5
			break;
		case 6:
			loadTxLdrBuf_ldrdata_sensor(num); // ch6
			break;
		case 7:
			loadTxLdrBuf_ldrdata_sensor(num); // ch7
            break;
		case 8:
			loadTxLdrBuf_ldrdata_sensor(num); // ch8
            break;
		case 9:
			loadTxLdrBuf_ldrdata_sensor_TopLow(); // top, low sensor
			break;
		case 10:
            loadTxLdrBuf_ldrdata_V(); // voltage
			break;
		case 11:
            loadTxLdrBuf_ldrdata_A(); // amp
			break;
		case 12:
        	loadTxLdrBuf_ldrdata_Volume(); // amp
			break;
		default:
			break;
	}
}



void dsplayManyStateInfo_pwm(void) {
/*
    �δ� �ʱ�ȭ�鿡 ���� ���°� �����ֱ� ���� �Լ�
*/
	uint8_t maxMenuIndex = 4;
	uint16_t showIndex = getShowIndex(maxMenuIndex);

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
	�δ� �ʱ�ȭ�� / ����Ʈ

	(���� ���⿡ ������ �������ν� �˻��ϱ� ������. !!! <== �˻� �뵵)
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
			Default_Cur_State_Display(); // ���� ���� ǥ��(���� ���)
            break;
        case 1:
			dsplayInDataState(); // �δ� ������ ǥ��
            break;
        default:
			dsplayManyStateInfo_pwm();
            break;
    }

    return (0);
}


/////////////////////////////////////
