

#ifndef __LOADER_CMD_HEADER
#define __LOADER_CMD_HEADER

#include    "..\main.h"

#define SIGN_POSIT_T	20
#define SIGN_POSIT_V_OR_A	200


#define ADR_NO_SET 'N'
#define ADR_SET_SEND 'S'
#define ADR_SET_REC 's'
#define ADR_SET_ACK 'A'
#define NORMAL '0'
#define WRITE_DATA 'W'
#define READ_DATA 'R'
#define BIT_WRITE_DATA 'w'
#define BIT_READ_DATA 'r'
#define LOGADR_CLR 'C'
#define NO_DATA 'Z'
#define RET_GOOD 'G'
#define RET_NOGOOD 'X'
#define LONG_READ_DATA 'L'
#define FLASH_RD_WR 'F'
#define NEW_FLASH_RD_WR 'f'
#define LONG_WR_DATA 'M'
#define SECONDLINE_CURSOR 1
#define FIRSTLINE_BASE 2
#define SECONDLINE_BASE 18
#define RCV_GROUP 0
#define RCV_LOGADR 1
#define RCV_ABSADR 2
#define RCV_CMD 3
#define RCV_CMDTYPE 4
#define RCV_BASE_ADR 5
#define RCV_OFFS_ADR 6
#define RCV_DATA_CNT 7
#define RCV_DATA 8
#define C_CMD 0
#define C_CMDTYPE 1
#define C_BASE_MEMORY 2
#define C_OFFSET_MEMORY 3
#define C_DATA_CNT 4
#define C_SAVE_DATA 5
#define MAX_LADDER_BUF 40
#define ONEKEY '1'
#define TWOKEY '2'
#define THREEKEY '3'
#define FOURKEY '4'
#define FIVEKEY '5'
#define SIXKEY '6'
#define SEVENKEY '7'
#define EIGHTKEY '8'
#define NINEKEY '9'
#define ZEROKEY '0'
#define ENTKEY 'e'
#define MENUKEY 'm'
#define ESCKEY 'c'
#define UPKEY 'u'
#define DOWNKEY 'd'
#define LEFTKEY 'l'
#define RIGHTKEY 'r'
#define ENTKEY 'e'
#define DIGIT_EDIT 0x0001
#define DIGIT_ALPHAR_EDIT 0x0002
#define TABLE_EDIT 0x0004
#define TOGGLE_EDIT 0x0008
#define DIGIT_STRING_EDIT 0x0010
#define CLOCK_EDIT 0x0020
#define BIT_ONOFF_EDIT 0x0040
#define NO_EDIT 0x0080
#define NO_SAVE 0x8000
#define BIT_TYPE 1
#define BYTE_TYPE 2
#define INT_TYPE 4
#define LONG_TYPE 8
#define DIVIDE_0 0
#define DIVIDE_10 1
#define DIVIDE_100 2
#define DIVIDE_1000 3
#define IMPORT_DATA 2
#define EXPORT_DATA 1
#define MAIN_GROUP01 0
#define MAIN_GROUP02 1
#define MAIN_GROUP03 2
#define MAIN_GROUP04 3
#define MAIN_GROUP05 4
#define MAIN_GROUP06 5
#define MAIN_GROUP07 6
#define MAIN_GROUP08 7
#define MAIN_GROUP09 8
#define MAIN_GROUP10 9
#define SUB_GROUP01 0
extern void Ladder485Init(void);
extern void updateNewFlash(void);
extern void Three_Dig_Dsp(uint32_t val, uint16_t dp);
extern void Five_Dig_Dsp(uint32_t val, uint16_t dp);
extern void Ten_Dig_Dsp(uint32_t val, uint16_t dp);
extern uint16_t UserMenuSerialNm;
extern uint16_t ThisSelMenuNm;
extern uint8_t new485Ladder[MAX_LADDER_BUF];
extern uint8_t LoaderBuf[17];
extern uint16_t LoaderKeyTime;
extern uint16_t ReadIntegerData(uint16_t pt);
extern uint16_t SaveVerify;
extern uint32_t ThisDigitData;
extern uint16_t CurKeyPoint;


/*
typedef struct _Menu_Status {
    uint16_t M_EditGroupNm;
    uint16_t M_EditSubNm;
    uint16_t M_EditDataType;
    uint16_t M_EditDivide;
    uint32_t M_EditCurDigitData;
    uint32_t M_EditDigitMaxValue;
    uint32_t M_EditDigitMinValue;
    uint16_t M_EditDigitCursor;
    uint16_t M_EditDigitShiftCnt;
    uint16_t M_EditCursor;
    uint16_t M_EditShiftCnt;
    uint16_t M_EditStart;
    uint16_t M_EditFlashAddr;
    uint16_t M_EditStatus;
    uint8_t * M_EditGroupMsgAddr;
    uint8_t * M_EditMsgAddr;
} Menu_Status;
*/


typedef union _Menu_Status {
	struct _sT{
	    uint16_t M_EditGroupNm;
	    uint16_t M_EditSubNm;
	    uint16_t M_EditDataType;
	    uint16_t M_EditDivide;
	    uint32_t M_EditCurDigitData;
	    uint32_t M_EditDigitMaxValue;
	    uint32_t M_EditDigitMinValue;
	    uint16_t M_EditDigitCursor;
	    uint16_t M_EditDigitShiftCnt;
	    uint16_t M_EditCursor;
	    uint16_t M_EditShiftCnt;
	    uint16_t M_EditStart;
	    uint16_t M_EditFlashAddr;
	    uint16_t M_EditStatus;
	    uint8_t * M_EditGroupMsgAddr;
	    uint8_t * M_EditMsgAddr;
	};

	struct _sU{
	    uint32_t tW[10];
	};
}Menu_Status;


extern Menu_Status MenuStatus[MAX_MENU];
extern uint16_t Menu_Status_Set(void);
extern Menu_Status CurMenuStatus;

#define CORRECTION_TEMP_ZERO 10 // +/- 10 => 0~19
#define VOLTAGE_ZERO 100 // +/- 10 => 0~19
#define CURRENT_ZERO 100 // +/- 10 => 0~19

extern void getSignalUserNumXXX(uint32_t signalNum[], uint32_t digit);

#endif

