
#include "loader_45k80_main.h"
#include "..\pwm.h"
#include "..\ad_conversion.h"
#include "..\main.h"

uint8_t UserSystemStatus = 0;
uint8_t UserRam_8[VARIABLE_8_END];
uint16_t UserRam_16[VARIABLE_16_END];
uint32_t UserRam_32[VARIABLE_32_END];
Menu_Status MenuStatus[MAX_MENU];

/////////////////////////////////////////////////////
#define DFL_SETUP 0x55						//default val
#define DFL_VERSION 0						//version
#define DFL_SCR_GOAL_VOLTAGE 500 // 50.0V
#define DFL_SCR_GOAL_CURRENT 100 // 10.0A
#define DFL_SCR_GOAL_SENSOR  10250 // 1000mV ����
// ���� ��/�� ����Ʈ
#define DFL_REVERSE_0 0 // 0 : zinc
#define DFL_REVERSE_1 0
#define DFL_REVERSE_2 0
#define DFL_REVERSE_3 0
#define DFL_REVERSE_4 0
#define DFL_REVERSE_5 0
#define DFL_REVERSE_6 0
#define DFL_REVERSE_7 0
#define DFL_REVERSE_8 0

#define DFL_CH0_TEMP_LOW 27	 //11234	  //2800
#define DFL_SRP_MAX 10100 // 7A
#define DFL_SRP_MIN 10101 // V
#define DFL_SRP_TIME 1000 // time

#define DFL_SOP_MAX 10200 // V
#define DFL_SOP_MIN 9800 // 7A
#define DFL_SOP_TIME 2000 // V


#define DFL_AOP_DUTY 20 // 7A
#define DFL_AOP_TIME 10000 // 7A

// ����
#define DFL_CH0_CORRENT_T	20
#define DFL_CH0_CORRENT_V	200
#define DFL_CH0_CORRENT_A	200

#define DFL_CH1_CORRENT_T	20
#define DFL_CH1_CORRENT_V	200
#define DFL_CH1_CORRENT_A	200

#define DFL_CH2_CORRENT_T	20
#define DFL_CH2_CORRENT_V	200
#define DFL_CH2_CORRENT_A	200

#define DFL_CH3_CORRENT_T	20
#define DFL_CH3_CORRENT_V	200
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
    sub_gr = 0;
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

// �׷�2 : �ִ� �Ѱ� ����/���� �׸��� ��ǥ ����(����)
uint16_t G2_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;
    //////////////////////////////////////////////////////////////////////
    // main group2
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP02;
    sub_gr = 0;
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
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 4;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 250; // 25.0A
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_GOAL_CURRENT;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //sub group7 - ch0 ��ǥ ����(�ν�) ����) 1000 mV ��
    /////////////////////////////////////////////////////////////////////
    // ThisSelMenuNm => 4 #1025
    bThisMenuSaver[UserMenuSerialNm] = 1;
    IntType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 5;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 12500;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue =  6000;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_GOAL_SENSOR;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;






    return (0);
}

// ���� �׷� : ������ ���� �Է� ����/����/���� �� ���� ���
// ZSU ������ ���� ��ɵ� �־���� - �̰��� ������ �׷쿡 �ٰ� �߰� ����.
uint16_t G3_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group3
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP03;
    sub_gr = 0;

    /////////////////////////////////////////////////////////////////////
    //scr : ����  ����
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_V;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
    /////////////////////////////////////////////////////////////////////
    //scr : ����  ����
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_10);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 4; // �� �Ҽ��� �����ؼ� ����ؾ� �Ѵ�.
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 250;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_A;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;
	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� - 0 (����)
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S_MAIN;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� ä��0
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� ä��1
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� ä��2
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� ä��3
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� ä��4
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� ä��5
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� ä��6
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
    UserMenuSerialNm++;
    sub_gr++;

	/////////////////////////////////////////////////////////////////////
    //scr : ����  ���� ä��7
    /////////////////////////////////////////////////////////////////////
    ByteType_DIGIT_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
    MenuStatus[UserMenuSerialNm].M_EditShiftCnt = 3;
    MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 210;
	MenuStatus[UserMenuSerialNm].M_EditDigitMinValue = 100;
    MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_SCR_CORRECT_S0;
    MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *)
            GroupLineMessage[UserMenuSerialNm];
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
    sub_gr = 0;

	/////////////////////////////////////////////////////////////////////
	//���� ���� - ch0 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH0_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//���� ���� - ch1 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH1_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//���� ���� - ch2 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH2_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//���� ���� - ch3 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH3_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//���� ���� - ch4 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH4_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//���� ���� - ch5 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH5_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//���� ���� - ch6 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH6_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	//���� ���� - ch7 use/nouse
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_CH7_USE;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_use_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;


    return (0);
}


// ��/�� �޴� 9�� + �׽�Ʈ �޴� 1��
uint16_t G5_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    //////////////////////////////////////////////////////////////////////
    // main group1
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP05;
    sub_gr = 0;

	/////////////////////////////////////////////////////////////////////
	// 0 ���� +/- ���� type ���� 0ä��
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_0; // ���̾�/�÷���
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 1 ���� ��/�� ���� ZSU 1ä��
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_1;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 2 ���� ��/�� ���� ZSU 2ä��
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_2;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 3 ���� ��/�� ���� ZSU 3ä��
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_3;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 4 ���� ��/�� ���� ZSU 4ä��
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_4;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 5 ���� ��/�� ���� ZSU 5ä��
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_5;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 6 ���� ��/�� ���� ZSU 6ä��
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_6;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 7 ���� ��/�� ���� ZSU 7ä��
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_REVERSE_7;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) ch_reverse_sel_list;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
	UserMenuSerialNm++;
	sub_gr++;

	/////////////////////////////////////////////////////////////////////
	// 8 ���� ��/�� ���� main ����
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

    //////////////////////////////////////////////////////////////////////
    // main group1
    /////////////////////////////////////////////////////////////////////
    main_gr = MAIN_GROUP06;
    sub_gr = 0;
	/////////////////////////////////////////////////////////////////////
	// �� 10���� ���� Ÿ�� ����
	/////////////////////////////////////////////////////////////////////
	ByteType_DIGIT_STRING_EDIT_Set(main_gr, sub_gr, DIVIDE_0);
	MenuStatus[UserMenuSerialNm].M_EditFlashAddr = F_A_TYPE_NUM;
	MenuStatus[UserMenuSerialNm].M_EditGroupMsgAddr = (uint8_t *) GroupLineMessage[UserMenuSerialNm];
	MenuStatus[UserMenuSerialNm].M_EditMsgAddr = (uint8_t *) currentTypeList;
	MenuStatus[UserMenuSerialNm].M_EditDigitMaxValue = 2;
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


	// end ----------------
    MenuStatus[UserMenuSerialNm].M_EditGroupNm = 0xff;
    return (0);
}




uint16_t G7_Menu_Status_Set(void) {
    uint16_t main_gr, sub_gr;

    main_gr = MAIN_GROUP07;
    sub_gr = SUB_GROUP01;
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
		// ���� ��/�� ����Ʈ
        cSR_ByteData(F_REVERSE_0) = DFL_REVERSE_0;
        cSR_ByteData(F_REVERSE_1) = DFL_REVERSE_1;
        cSR_ByteData(F_REVERSE_2) = DFL_REVERSE_2;
        cSR_ByteData(F_REVERSE_3) = DFL_REVERSE_3;
        cSR_ByteData(F_REVERSE_4) = DFL_REVERSE_4;
        cSR_ByteData(F_REVERSE_5) = DFL_REVERSE_5;
        cSR_ByteData(F_REVERSE_6) = DFL_REVERSE_6;
        cSR_ByteData(F_REVERSE_7) = DFL_REVERSE_7;
        cSR_ByteData(F_REVERSE_8) = DFL_REVERSE_8;

		// ch0
        cSR_ByteData(F_A_TYPE_NUM) = DFL_CH0_TEMP_LOW;
        cSR_ByteData(F_CH5_USE) = 1;
        cSR_ByteData(F_CH0_USE) = 1;
		// ch1
        cSR_ByteData(F_CH6_USE) = 1;
        cSR_ByteData(F_CH1_USE) = 1;
		// ch2
		cSR_ByteData(F_CH7_USE) = 1;
        cSR_ByteData(F_CH2_USE) = 1;
		// ch3
		cSR_ByteData(F_CH3_ENABLE) = 1;
        cSR_ByteData(F_CH3_USE) = 1;
		// ch4
		cSR_ByteData(F_CH4_ENABLE) = 1;
        cSR_ByteData(F_CH4_USE) = 1;
		// ����
        cSR_ByteData(F_SCR_CORRECT_S_MAIN) = DFL_CH0_CORRENT_T;
        cSR_ByteData(F_SCR_CORRECT_V) = DFL_CH0_CORRENT_V;
        cSR_ByteData(F_SCR_CORRECT_A) = DFL_CH0_CORRENT_A;
        cSR_ByteData(F_SCR_CORRECT_S0) = DFL_CH1_CORRENT_T;
        cSR_ByteData(F_SCR_CORRECT_S1) = DFL_CH1_CORRENT_V;
        cSR_ByteData(F_SCR_CORRECT_S2) = DFL_CH1_CORRENT_A;
        cSR_ByteData(F_SCR_CORRECT_S3) = DFL_CH2_CORRENT_T;
        cSR_ByteData(F_SCR_CORRECT_S4) = DFL_CH2_CORRENT_V;
        cSR_ByteData(F_SCR_CORRECT_S5) = DFL_CH2_CORRENT_A;
        cSR_ByteData(F_SCR_CORRECT_S6) = DFL_CH3_CORRENT_T;
        cSR_ByteData(F_SCR_CORRECT_S7) = DFL_CH3_CORRENT_V;
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

	// ����
    num = micom_getSensorNowSuwi(0); // <<< #1025
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


	// ����
    num = heater[ch].userNowInAmp_100mA;
    ascii_100   = num / 100;
    num = num % 100;
    ascii_10    =   num / 10;
    num = num % 10;
    ascii_1     = num;

    ascii_100 = getAscii(ascii_100);
    ascii_10 = getAscii(ascii_10);
    ascii_1 = getAscii(ascii_1);

	new485Ladder[SECONDLINE_BASE + 5] = ascii_100;
	new485Ladder[SECONDLINE_BASE + 6] = ascii_10;
	new485Ladder[SECONDLINE_BASE + 7] = '.';
	new485Ladder[SECONDLINE_BASE + 8] = ascii_1;
	new485Ladder[SECONDLINE_BASE + 9] = 'A';
	new485Ladder[SECONDLINE_BASE + 10] = '/';


	// �µ�

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
// ch0 ���� ���� �δ����� ���� ���� �Լ�
void dsplayManyStateInfo_allCH(void) {
/*
    �δ� �ʱ�ȭ�鿡 ���� ���°� �����ֱ� ���� �Լ�
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
    �δ� �ʱ�ȭ�鿡 ���� ���°� �����ֱ� ���� �Լ�
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
    �δ� �ʱ�ȭ�鿡 ���� ���°� �����ֱ� ���� �Լ�
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

    if (UserSystemStatus == 0) {
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
            dsplayManyStateInfo_allCH__copy();
            break;
        case 1:
			dsplayManyStateInfo_pwm();
            break;
        default:
            Default_Cur_State_Display();
            break;
    }

    return (0);
}


/////////////////////////////////////
