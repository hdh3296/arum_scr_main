
#include        "loader_45k80_main.h"
//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////
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

Menu_Status CurMenuStatus;
uint16_t UserMenuSerialNm;
uint16_t ThisSelMenuNm;
uint8_t LoaderBitAreaSram1;
uint8_t LoaderBuf[17];
uint8_t new485Ladder[MAX_LADDER_BUF] = {
    0
};

extern void getFinalUserNumWhenUpKey(uint32_t signalNum[], uint32_t digit, uint8_t p);
extern void getFinalUserNumWhenDnKey(uint32_t signalNum[], uint32_t digit, uint8_t p);

extern uint32_t changeNumberMinusMethod(uint8_t p, uint32_t n);

uint32_t ThisSignalUserNumbuf[2];

uint8_t bThisMenuSaver[100]; // +/- 메뉴 여부 저장


enum {
	GOAL_SENSOR = 4,
	SRP_MAX = 34,
	SRP_MIN	= 35,
	SOP_MAX = 37,
	SOP_MIN	= 38,

};

bool isThisSelMenuNmIsSignDigit_1024(void) {

	return (bThisMenuSaver[ThisSelMenuNm]);

}


enum {
	MENU_CTa_0 = 8,
	MENU_CVb_0 = 9,
	MENU_CAc_0 = 10,

	MENU_CTa_1 = 17,
	MENU_CVb_1 = 18,
	MENU_CAc_1 = 19,

	MENU_CTa_2 = 26,
	MENU_CVb_2 = 27,
	MENU_CAc_2 = 28,

	MENU_CTa_3 = 35,
	MENU_CVb_3 = 36,
	MENU_CAc_3 = 37,

	MENU_CTa_4 = 44,
	MENU_CVb_4 = 45,
	MENU_CAc_4 = 46
};


uint32_t ThisDigitData; // 로더에 표시되는 설정값 데이터
uint16_t LoaderKey;
uint16_t EnterKey;
uint16_t LoaderKeyTime;
uint16_t BefDspMode = 0;
uint16_t SaveVerify = 0;
uint16_t MemPointer;
uint8_t arabianm[10];
uint16_t CurKeyPoint = 0;
const uint8_t blinkx[] = {
    "                "
};


uint16_t Decimal_Sort(uint32_t val) {
    arabianm[0] = (uint8_t) ((val / (uint32_t) 1000000000) + '0');
    val = (uint32_t) (val % (uint32_t) 1000000000);
    arabianm[1] = (uint8_t) ((val / (uint32_t) 100000000) + '0');
    val = (uint32_t) (val % (uint32_t) 100000000);
    arabianm[2] = (uint8_t) ((val / (uint32_t) 10000000) + '0');
    val = (uint32_t) (val % (uint32_t) 10000000);
    arabianm[3] = (uint8_t) ((val / (uint32_t) 1000000) + '0');
    val = (uint32_t) (val % (uint32_t) 1000000);
    arabianm[4] = (uint8_t) ((val / (uint32_t) 100000) + '0');
    val = (uint32_t) (val % (uint32_t) 100000);
    arabianm[5] = (uint8_t) ((val / (uint32_t) 10000) + '0');
    val = (uint32_t) (val % (uint32_t) 10000);
    arabianm[6] = (uint8_t) ((val / (uint32_t) 1000) + '0');
    val = (uint32_t) (val % (uint32_t) 1000);
    arabianm[7] = (uint8_t) ((val / (uint32_t) 100) + '0');
    val = (uint32_t) (val % (uint32_t) 100);
    arabianm[8] = (uint8_t) ((val / (uint32_t) 10) + '0');
    arabianm[9] = (uint8_t) ((val % (uint32_t) 10) + '0');
    return (0);
}


uint16_t Deciml_Dsp(uint16_t total_dig, uint16_t dp) {
    uint16_t i, j;
    uint8_t tempbuf[10];

    if (dp == 0) {
        return (0);

    }

    if (dp >= total_dig) {
        return (0);

    }

    for (i = 0; i < total_dig; i++) {
        tempbuf[i] = new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + i];
    }

    j = (total_dig - dp);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + j] = '.';

    for (i = j; i < total_dig; i++) {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + i + 1] = tempbuf[i];
    }

    return (0);
}


void One_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[9];
}


void Two_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


void Three_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[7];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


void Four_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[6];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[7];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


void Five_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[5];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[6];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = arabianm[7];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


void Six_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[4];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[5];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = arabianm[6];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = arabianm[7];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 5] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


void Seven_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[3];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[4];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = arabianm[5];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = arabianm[6];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = arabianm[7];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 5] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 6] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


void Eight_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[2];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[3];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = arabianm[4];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = arabianm[5];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = arabianm[6];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 5] = arabianm[7];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 6] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 7] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


void Nine_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[1];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[2];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = arabianm[3];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = arabianm[4];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = arabianm[5];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 5] = arabianm[6];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 6] = arabianm[7];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 7] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 8] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


void Ten_Dig_Dsp(uint32_t val, uint16_t dp) {
    Decimal_Sort(val);
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = arabianm[0];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = arabianm[1];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = arabianm[2];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = arabianm[3];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = arabianm[4];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 5] = arabianm[5];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 6] = arabianm[6];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 7] = arabianm[7];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 8] = arabianm[8];
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 9] = arabianm[9];
    Deciml_Dsp(CurMenuStatus.M_EditDigitShiftCnt, dp);
}


uint16_t BitOnOffMessage(uint8_t byte_data) {
    uint8_t pt;

    if ((CurMenuStatus.M_EditStatus & BIT_ONOFF_EDIT)) {
        pt = (uint8_t) (byte_data);

        if (pt & 0x01) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '1';
        } else {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '0';
        }

        if (pt & 0x02) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = '2';
        } else {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 1] = '0';
        }

        if (pt & 0x04) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = '3';
        } else {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 2] = '0';
        }

        if (pt & 0x08) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = '4';
        } else {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = '0';
        }

        if (pt & 0x10) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = '5';
        } else {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = '0';
        }

        if (pt & 0x20) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 5] = '6';
        } else {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 5] = '0';
        }

        if (pt & 0x40) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 6] = '7';
        } else {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 6] = '0';
        }

        if (pt & 0x80) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 7] = '8';
        } else {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 7] = '0';
        }
    }

    return (0);
}


uint16_t DigitStringMessage(void) {
    uint16_t i, pt;

    if ((CurMenuStatus.M_EditStatus & DIGIT_STRING_EDIT)) {
        pt = (uint16_t) (ThisDigitData * 17);

        for (i = 0; (CurMenuStatus.M_EditStart + i) < 16; i++) {
            new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + i] = * (CurMenuStatus.M_EditMsgAddr + pt + i);
        }
    }

    return (0);
}


void ldr_sigh_T_1023T() {

    if (ThisSignalUserNumbuf[0] == SIGN_PLUS) {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '+';
    } else {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '-';
    }
}


void ldr_sigh_T(void) {
	uint16_t max = CurMenuStatus.M_EditDigitMaxValue; // 29
	uint16_t min = CurMenuStatus.M_EditDigitMinValue; // 10
	uint16_t pt_min;
	pt_min = (max + min + 1) / 2; 	// 20


    if (ThisDigitData >= pt_min) {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '+';
    } else {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '-';
    }

    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 3] = 0x60;
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = 'C';

}

// +/- 20V : 20 x1000mV
void ldr_sigh_V(void) {
	uint16_t max = CurMenuStatus.M_EditDigitMaxValue; // 299
	uint16_t min = CurMenuStatus.M_EditDigitMinValue; // 100
	uint16_t pt_min;
	pt_min = (max + min + 1) / 2; 	// 200

    if (ThisDigitData >= pt_min) {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '+';
    } else {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '-';
    }
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = 'V';
}

// 전류 +/- 5A : 50 x100mA
void ldr_sign_A(void) {
	uint16_t max = CurMenuStatus.M_EditDigitMaxValue; // 299
	uint16_t min = CurMenuStatus.M_EditDigitMinValue; // 100
	uint16_t pt_min;
	pt_min = (max + min + 1) / 2; 	// 200


    if (ThisDigitData >= pt_min) {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '+';
    } else {
        new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 0] = '-';
    }

    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 4] = ' ';
    new485Ladder[SECONDLINE_BASE + CurMenuStatus.M_EditStart + 5] = 'A';
}

void display_unit() {

	if (bThisMenuSaver[ThisSelMenuNm]) {
		ldr_sigh_T_1023T(); // 온도
	}


}

void Integer_Digit_1023T(uint32_t userNum) {
	// 여기에서 userNum 값을 로더에 표시해준다.
    if (CurMenuStatus.M_EditDigitMaxValue < 10) {
        CurMenuStatus.M_EditDigitShiftCnt = 1;
        One_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 100) {
        CurMenuStatus.M_EditDigitShiftCnt = 2;
        Two_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 1000) {
        CurMenuStatus.M_EditDigitShiftCnt = 3;
        Three_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 10000) {
        CurMenuStatus.M_EditDigitShiftCnt = 4;
        Four_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 100000) {
        CurMenuStatus.M_EditDigitShiftCnt = 5;
        Five_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 100000) {
        CurMenuStatus.M_EditDigitShiftCnt = 6;
        Six_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 1000000) {
        CurMenuStatus.M_EditDigitShiftCnt = 7;
        Seven_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 10000000) {
        CurMenuStatus.M_EditDigitShiftCnt = 8;
        Eight_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 100000000) {
        CurMenuStatus.M_EditDigitShiftCnt = 9;
        Nine_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    } else {
        CurMenuStatus.M_EditDigitShiftCnt = 10;
        Ten_Dig_Dsp(userNum, CurMenuStatus.M_EditDivide);
    }

	display_unit(); // @보정 ★

}


void getSignNumberByLdrDigit(uint32_t dest[], uint32_t src) {
	// 로더 변수 값을 가지고 해당 버퍼에 +/- 기호 값을 담아 저장한다.
	// 예시) src = 9000 이라면, => -1000
	if (src >= 10000) {
		dest[0] = SIGN_PLUS;		// +
		dest[1] = src - 10000; 	// 12500 - 10000 = 2500
	} else {
		dest[0] = SIGN_MINUS;
		dest[1] = 10000 - src; // 10000 - 7500 = 2500
	}
}



void Integer_Digit(void) {

	if (isThisSelMenuNmIsSignDigit_1024()) {
		// user 보여지는 값
		// 					저장할 버퍼, 			입력 로더 변수
		getSignNumberByLdrDigit(ThisSignalUserNumbuf, ThisDigitData);
		Integer_Digit_1023T(ThisSignalUserNumbuf[1]);
		return;
	}

    if (ThisDigitData >= CurMenuStatus.M_EditDigitMaxValue) {
        ThisDigitData = CurMenuStatus.M_EditDigitMaxValue;
    } else if (ThisDigitData <= CurMenuStatus.M_EditDigitMinValue) {
        ThisDigitData = CurMenuStatus.M_EditDigitMinValue;
    }


	// 여기에서 ThisDigitData 값을 로더에 표시해준다.
    if (CurMenuStatus.M_EditDigitMaxValue < 10) {
        CurMenuStatus.M_EditDigitShiftCnt = 1;
        One_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 100) {
        CurMenuStatus.M_EditDigitShiftCnt = 2;
        Two_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 1000) {
        CurMenuStatus.M_EditDigitShiftCnt = 3;
        Three_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 10000) {
        CurMenuStatus.M_EditDigitShiftCnt = 4;
        Four_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 100000) {
        CurMenuStatus.M_EditDigitShiftCnt = 5;
        Five_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 100000) {
        CurMenuStatus.M_EditDigitShiftCnt = 6;
        Six_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 1000000) {
        CurMenuStatus.M_EditDigitShiftCnt = 7;
        Seven_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 10000000) {
        CurMenuStatus.M_EditDigitShiftCnt = 8;
        Eight_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else if (CurMenuStatus.M_EditDigitMaxValue < 100000000) {
        CurMenuStatus.M_EditDigitShiftCnt = 9;
        Nine_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    } else {
        CurMenuStatus.M_EditDigitShiftCnt = 10;
        Ten_Dig_Dsp(ThisDigitData, CurMenuStatus.M_EditDivide);
    }

    DigitStringMessage();
    BitOnOffMessage((uint8_t) ThisDigitData);
}


uint16_t ReadLongData(uint16_t pt) {
    ThisDigitData = (uint32_t) (lF_LongData(pt));
    return (0);
}


uint16_t ReadIntegerData(uint16_t pt) {
    ThisDigitData = (uint32_t) (iF_IntData(pt));
    return (0);
}


uint16_t ReadByteData(uint16_t pt) {
    ThisDigitData = (uint32_t)
                    cF_ByteData(pt);
    return (0);
}


//uint16_t  LoadEditData(uint16_t MainGroup,uint16_t SubGroup)
uint16_t LoadMenuStatus(uint16_t MainGroup) {
    CurMenuStatus.M_EditDigitCursor = 0;
    CurMenuStatus.M_EditGroupNm = MenuStatus[MainGroup].M_EditGroupNm;
    CurMenuStatus.M_EditSubNm = MenuStatus[MainGroup].M_EditSubNm;
    CurMenuStatus.M_EditCursor = MenuStatus[MainGroup].M_EditCursor;
    CurMenuStatus.M_EditStatus = MenuStatus[MainGroup].M_EditStatus;
    CurMenuStatus.M_EditStart = MenuStatus[MainGroup].M_EditStart;
    CurMenuStatus.M_EditShiftCnt = MenuStatus[MainGroup].M_EditShiftCnt;
    CurMenuStatus.M_EditDivide = MenuStatus[MainGroup].M_EditDivide;
    CurMenuStatus.M_EditDigitMaxValue = MenuStatus[MainGroup].M_EditDigitMaxValue;
    CurMenuStatus.M_EditDigitMinValue = MenuStatus[MainGroup].M_EditDigitMinValue;
    CurMenuStatus.M_EditDataType = MenuStatus[MainGroup].M_EditDataType;
    CurMenuStatus.M_EditFlashAddr = MenuStatus[MainGroup].M_EditFlashAddr;
    CurMenuStatus.M_EditMsgAddr = MenuStatus[MainGroup].M_EditMsgAddr;
    CurMenuStatus.M_EditGroupMsgAddr = MenuStatus[MainGroup].M_EditGroupMsgAddr;
    //	MenuStatus[MainGroup].M_EditCurDigitData=0;
    return (0);
}


uint16_t LoadEditData(uint16_t MainGroup) {
    if ((CurMenuStatus.M_EditDataType & BYTE_TYPE)) {
        ReadByteData(CurMenuStatus.M_EditFlashAddr);
    } else if ((CurMenuStatus.M_EditDataType & INT_TYPE)) {
        ReadIntegerData(CurMenuStatus.M_EditFlashAddr);
    } else if ((CurMenuStatus.M_EditDataType & LONG_TYPE)) {
        ReadLongData(CurMenuStatus.M_EditFlashAddr);
    }

    Integer_Digit();
    return (0);
}


uint16_t SaveData(void) {
    uint16_t i, j, BlockPt;

    MenuStatus[ThisSelMenuNm].M_EditCurDigitData = ThisDigitData;

    if ((CurMenuStatus.M_EditStatus & NO_SAVE)) {
        return (0);

    }

    BlockPt = (CurMenuStatus.M_EditFlashAddr / FLASH_ONE_BLOCK_SIZE); //?
    FlashBlockRd(BlockPt); // << block read

    if ((CurMenuStatus.M_EditDataType & LONG_TYPE)) {
        lSR_LongData(CurMenuStatus.M_EditFlashAddr) = ThisDigitData;
    } else if ((CurMenuStatus.M_EditDataType & INT_TYPE)) {
        iSR_IntData(CurMenuStatus.M_EditFlashAddr) = ThisDigitData;
    } else if ((CurMenuStatus.M_EditDataType & BYTE_TYPE)) {
        cSR_ByteData(CurMenuStatus.M_EditFlashAddr) = (uint8_t) (ThisDigitData);
    } else {
        return (0);
    }

    FlashBlockWr(BlockPt); // << block write
    return (0);
}


uint16_t LaderDsp(void) {
    uint16_t i, j;

    LoadMenuStatus(ThisSelMenuNm);

    for (i = 0; i < MAX_LADDER_BUF - 2; i++) {
        new485Ladder[i] = ' ';
    }

    new485Ladder[MAX_LADDER_BUF - 1] = 0x0;

    for (i = 0; i < 16; i++) {
        new485Ladder[i + 2] = * (CurMenuStatus.M_EditGroupMsgAddr + i);
    }

    new485Ladder[0] = LoaderKey;
    new485Ladder[1] = ' ';
    new485Ladder[SECONDLINE_BASE + 0] = (CurMenuStatus.M_EditSubNm / 10) + '0';
    new485Ladder[SECONDLINE_BASE + 1] = (CurMenuStatus.M_EditSubNm % 10) + '0';
    ;
    return (0);
}


void CursorOff(void) {
    new485Ladder[SECONDLINE_CURSOR] = ' ';
}


void CursorOn(void) {
    uint16_t i;

    i = CurMenuStatus.M_EditCursor + CurMenuStatus.M_EditStart;

    if (i > 0x09) {
        i = i + '7';
    } else {
        i = i + '0';
    }
    new485Ladder[SECONDLINE_CURSOR] = i;
}


uint16_t ShiftLeftCur(void) {
    uint16_t i;

    if (CurMenuStatus.M_EditShiftCnt == 0) {
        CurMenuStatus.M_EditCursor = 0;
    } else if (CurMenuStatus.M_EditCursor > 0) {
        CurMenuStatus.M_EditCursor--;
    } else {
        CurMenuStatus.M_EditCursor = CurMenuStatus.M_EditShiftCnt - 1;
    }

    if (CurMenuStatus.M_EditDigitCursor > 0) {
        CurMenuStatus.M_EditDigitCursor--;
    } else {
        CurMenuStatus.M_EditDigitCursor = CurMenuStatus.M_EditDigitShiftCnt - 1;
    }
    i = SECONDLINE_BASE + CurMenuStatus.M_EditStart + CurMenuStatus.M_EditCursor;

    if ((CurMenuStatus.M_EditStatus & DIGIT_EDIT)) {
        if ((new485Ladder[i] == '.') || (new485Ladder[i] == ('.' + 0x80))) {
            if (CurMenuStatus.M_EditShiftCnt == 0) {
                CurMenuStatus.M_EditCursor = 0;
            } else if (CurMenuStatus.M_EditCursor > 0) {
                CurMenuStatus.M_EditCursor--;
            } else {
                CurMenuStatus.M_EditCursor = CurMenuStatus.M_EditShiftCnt - 1;
            }
        }
    } else if ((CurMenuStatus.M_EditStatus & CLOCK_EDIT)) {
        if ((new485Ladder[i] == '-') || (new485Ladder[i] == ('-' + 0x80))) {
            if (CurMenuStatus.M_EditShiftCnt == 0) {
                CurMenuStatus.M_EditCursor = 0;
            } else if (CurMenuStatus.M_EditCursor > 0) {
                CurMenuStatus.M_EditCursor--;
            } else {
                CurMenuStatus.M_EditCursor = CurMenuStatus.M_EditShiftCnt - 1;
            }
        }
    }

    return (0);
}


uint16_t ShiftRightCur(void) {
    uint16_t i;

    if (CurMenuStatus.M_EditShiftCnt == 0) {
        CurMenuStatus.M_EditCursor = 0;
    } else {
        CurMenuStatus.M_EditCursor = (CurMenuStatus.M_EditCursor + 1) % CurMenuStatus.M_EditShiftCnt;
    }
    CurMenuStatus.M_EditDigitCursor = (CurMenuStatus.M_EditDigitCursor + 1) % CurMenuStatus.M_EditDigitShiftCnt;
    i = SECONDLINE_BASE + CurMenuStatus.M_EditStart + CurMenuStatus.M_EditCursor;

    if ((CurMenuStatus.M_EditStatus & DIGIT_EDIT)) {
        if ((new485Ladder[i] == '.') || (new485Ladder[i] == ('.' + 0x80))) {
            if (CurMenuStatus.M_EditShiftCnt == 0) {
                CurMenuStatus.M_EditCursor = 0;
            } else {
                CurMenuStatus.M_EditCursor = (CurMenuStatus.M_EditCursor + 1) % CurMenuStatus.M_EditShiftCnt;
            }
        }
    } else if ((CurMenuStatus.M_EditStatus & CLOCK_EDIT)) {
        if ((new485Ladder[i] == '-') || (new485Ladder[i] == ('-' + 0x80))) {
            if (CurMenuStatus.M_EditShiftCnt == 0) {
                CurMenuStatus.M_EditCursor = 0;
            } else {
                CurMenuStatus.M_EditCursor = (CurMenuStatus.M_EditCursor + 1) % CurMenuStatus.M_EditShiftCnt;
            }
        }
    }

    return (0);
}

// @보정 #1023
void ldr_correctT_plus(uint8_t p) {
	uint16_t max = CurMenuStatus.M_EditDigitMaxValue; // 25
	uint16_t min = CurMenuStatus.M_EditDigitMinValue; // 10
	uint16_t pt_max, pt_min, nt_max, nt_min;

	pt_max = max; 					// 25
	pt_min = SIGN_POSIT_T;	//pt_min = (max + min + 1) / 2; => 20
	//--------------------------------------
	nt_max = pt_min - 5; 			// 19
	nt_min = min;  				// 15

	switch (p) {
		case 1:
			if (ThisDigitData == pt_max) { // 25
				ThisDigitData = pt_min;
				return;
			} else if (ThisDigitData == nt_max) { // 19
				ThisDigitData = nt_min;
				return;
			}
			ThisDigitData++;
			return;
		case 2:
			if (ThisDigitData >= pt_min) { // 20
				ThisDigitData -= 10;
				return;
			} else {
				ThisDigitData += 10;
				return;
			}
		case 3:
			return;
	}
}

void ldr_correctT_minus(uint8_t p) {
	uint16_t max = CurMenuStatus.M_EditDigitMaxValue; // 25
	uint16_t min = CurMenuStatus.M_EditDigitMinValue; // 10
	uint16_t pt_max, pt_min, nt_max, nt_min;

	pt_max = max; 					// 25
	pt_min = SIGN_POSIT_T; // pt_min = (max + min + 1) / 2; 	// 20
	//--------------------------------------
	nt_max = pt_min - 5; 			// 15
	nt_min = min;  				// 10

	switch (p) {
		case 1:
			if (ThisDigitData == nt_min) { // max
				ThisDigitData = nt_min;
				return;
			} else if (ThisDigitData == pt_min) {
				ThisDigitData = pt_min;
				return;
			}

			ThisDigitData--;
			return;
		case 2:
			if (ThisDigitData >= pt_min) {
				ThisDigitData -= 10;
				return;
			}

			ThisDigitData += 10;
			return;
		case 3:
			return;
	}
}

void ldr_correctV_plus(uint8_t p) {
	uint16_t max = CurMenuStatus.M_EditDigitMaxValue; // 210 / 250
	uint16_t min = CurMenuStatus.M_EditDigitMinValue; // 100
	uint16_t pt_max, pt_min; // 플러스
	uint16_t nt_max, nt_min; // 마이너스

	pt_max = max; 					// 250
	pt_min = SIGN_POSIT_V_OR_A;	// pt_min = (max + min + 1) / 2; 	// 200
	//--------------------------------------
	nt_max = min + (max - SIGN_POSIT_V_OR_A); 			// 150
	nt_min = min;  				// 100

	switch (p) {
		case 1:
			if (ThisDigitData == pt_max) { // 250
				ThisDigitData = pt_min; // 200
				return;
			} else if (ThisDigitData == nt_max) { // 150
				ThisDigitData = nt_min; // 100
				return;
			}
			ThisDigitData++;
			return;
		case 2:
			if ( (ThisDigitData >= (pt_max - 9)) && // >= 241
				 (ThisDigitData <= pt_max) ) { // <= 250
				ThisDigitData = pt_min; // 200
				return;
			} else if ( (ThisDigitData >= (nt_max - 9)) && // > 190
						(ThisDigitData <= nt_max) ) { // < 199
				ThisDigitData = nt_min; // 100
				return;
			}
			ThisDigitData += 10;
			return;
		case 3:
			if (ThisDigitData >= pt_min) { // 200
				ThisDigitData -= 100;
				return;
			} else {
				ThisDigitData += 100;
				return;
			}
	}
}
void ldr_correctV_minus(uint8_t p) {
	uint16_t max = CurMenuStatus.M_EditDigitMaxValue; // 250 / 220
	uint16_t min = CurMenuStatus.M_EditDigitMinValue; // 100 / 100
	uint16_t pt_max, pt_min, nt_max, nt_min;

	pt_max = max; 					// 250
	pt_min = SIGN_POSIT_V_OR_A;	// pt_min = (max + min + 1) / 2; 	// 200
	//--------------------------------------
	nt_max = min + (max - SIGN_POSIT_V_OR_A);			// 150
	nt_min = min;  				// 100

	switch (p) {
		case 1:
			if (ThisDigitData == nt_min) { // 100
				ThisDigitData = nt_min; // 100
				return;
			} else if (ThisDigitData == pt_min) { // 200
				ThisDigitData = pt_min; // 200
				return;
			}
			ThisDigitData--;
			return;
		case 2:
			if ( (ThisDigitData <= (pt_min + 9)) && // <= 209
				 (ThisDigitData >= pt_min) ) { // >= 200
				ThisDigitData = pt_min; // 200
				return;
			} else if ( (ThisDigitData <= (nt_min + 9)) && // <= 109
						(ThisDigitData >= nt_min) ) { // >= 100
				ThisDigitData = nt_min; // 100
				return;
			}
			ThisDigitData -= 10;
			return;
		case 3:
			if (ThisDigitData >= pt_min) { // 200
				ThisDigitData -= 100;
				return;
			} else {
				ThisDigitData += 100;
				return;
			}
	}
}

uint32_t pow_me(uint8_t p) {
	switch (p) {
		case 1:
			return 10;
		case 2:
			return 100;
		case 3:
			return 1000;
		case 4:
			return 10000;
		case 5:
			return 100000;
		case 6:
			return 1000000;
		case 7:
			return 10000000;
		case 8:
			return 100000000;
		case 9:
			return 1000000000;
	}
    return 1;
}

uint32_t changeNumberPlusMethod(uint8_t p, uint32_t num) {
	uint32_t i = pow_me(p); //
	uint32_t j = i / 10; //
	uint32_t k = 9 * j; //

	if ( (num % i / j) == 9 ) {
	// 9 라면?
		// 0으로 만들어야 한다.
		return (num - k);
	}
	return (num + j);
}
void ldr_normal_plus(uint8_t p) {
	ThisDigitData = changeNumberPlusMethod(p, ThisDigitData);
}
uint32_t changeNumberMinusMethod(uint8_t p, uint32_t num) {
	uint32_t i = pow_me(p); //
	uint32_t j = i / 10; //

	if ( (num % i / j) == 0 ) {
	// 0 이라면?
		// 그냥 아무짓도 하지 말고 원래 숫자 반환하자
		return num;
	}
	return num - j;
}
void ldr_nomal_minus(uint8_t p) {
	ThisDigitData = changeNumberMinusMethod(p, ThisDigitData);
}


bool isCorrTempMenu(uint16_t now_menu) {
	if (now_menu == MENU_CTa_0)
		return 1;
	if (now_menu == MENU_CTa_1)
		return 1;
	if (now_menu == MENU_CTa_2)
		return 1;
	if (now_menu == MENU_CTa_3)
		return 1;
	if (now_menu == MENU_CTa_4)
		return 1;

	return 0;
}

bool isCorrVoltAndAmp(uint16_t now_menu) {
	if (now_menu == MENU_CVb_0)
		return 1;
	if (now_menu == MENU_CAc_0)
		return 1;
	if (now_menu == MENU_CVb_1)
		return 1;
	if (now_menu == MENU_CAc_1)
		return 1;
	if (now_menu == MENU_CVb_2)
		return 1;
	if (now_menu == MENU_CAc_2)
		return 1;
	if (now_menu == MENU_CVb_3)
		return 1;
	if (now_menu == MENU_CAc_3)
		return 1;
	if (now_menu == MENU_CVb_4)
		return 1;
	if (now_menu == MENU_CAc_4)
		return 1;

	return 0;
}




uint32_t getSignalNumPlusMax(uint32_t max) {
	return (max - 10000);
}
uint32_t getSignalNumMinusMax(uint32_t min) {
	return (10000 - min);
}

void changeSignNumByPlusMinus(uint32_t signnum[]) {
	// 반대 sign으로 변경
	if (signnum[0] == SIGN_PLUS) {
		signnum[0] = SIGN_MINUS;
		signnum[1] = 1;
	} else {
		signnum[0] = SIGN_PLUS;
		signnum[1] = 0;
	}
}

void getFinalUserNumWhenUpKey(uint32_t dest[], uint32_t src, uint8_t p) {
// up 키늘 눌렀을 때 처리 할 내용들을 담은 함수이다.
// 즉, up 키를 누르면 dest 값을 얻데이트 한다.(더하기)
	// 일단 digit값을 해석해야 한다.
	if (src >= 10000) {
		dest[0] = SIGN_PLUS;
		dest[1] = src - 10000; // 12500 - 10000 = 2500
	} else {
		dest[0] = SIGN_MINUS;
		dest[1] = 10000 - src; // 10000 - 7500 = 2500
	}

	switch (p) {
		case 1:
		case 2:
		case 3:
		case 4:
			dest[1] = changeNumberPlusMethod(p, dest[1]);
			// max, min
			if (dest[0] == SIGN_PLUS) {
				if (dest[1] > getSignalNumPlusMax(CurMenuStatus.M_EditDigitMaxValue)) {
					dest[1] = getSignalNumPlusMax(CurMenuStatus.M_EditDigitMaxValue);
				}
			} else {
				if (dest[1] > getSignalNumMinusMax(CurMenuStatus.M_EditDigitMinValue)) {
					dest[1] = getSignalNumMinusMax(CurMenuStatus.M_EditDigitMinValue);
				}
			}
			break;
		case 5:
			changeSignNumByPlusMinus(dest);
			break;
	}
}
// down key
void getFinalUserNumWhenDnKey(uint32_t dest[], uint32_t src, uint8_t p) {
	// 일단 digit값을 해석해야 한다.
	if (src >= 10000) {
		dest[0] = SIGN_PLUS;
		dest[1] = src - 10000; // 12500 - 10000 = 2500
	} else {
		dest[0] = SIGN_MINUS;
		dest[1] = 10000 - src; // 10000 - 7500 = 2500
	}

	switch (p) {
		case 1:
		case 2:
		case 3:
		case 4:
			dest[1] = changeNumberMinusMethod(p, dest[1]);
			// max, min
			if (dest[0] == SIGN_PLUS) {
				if (dest[1] > getSignalNumPlusMax(CurMenuStatus.M_EditDigitMaxValue)) {
					dest[1] = getSignalNumPlusMax(CurMenuStatus.M_EditDigitMaxValue);
				}
			} else {
				if (dest[1] > getSignalNumMinusMax(CurMenuStatus.M_EditDigitMinValue)) {
					dest[1] = getSignalNumMinusMax(CurMenuStatus.M_EditDigitMinValue);
				}
			}
			break;
		case 5:
			changeSignNumByPlusMinus(dest);
			break;
	}
}


uint32_t updateThisDigitData(uint32_t signalNum[]) {
	uint32_t thisdigitdata;

	switch (signalNum[0]) {
		case SIGN_PLUS:
			thisdigitdata = 10000 + signalNum[1];
			break;
		case SIGN_MINUS:
			thisdigitdata = 10000 - signalNum[1];
			break;
	}

	return thisdigitdata;
}
uint16_t CusorDataUp(void) { // @보정 #1023
// up key를 누르면
    uint16_t i, dp;
	uint8_t pos;

    i = SECONDLINE_BASE + CurMenuStatus.M_EditStart + CurMenuStatus.M_EditCursor;

	pos = 0;
    if ((CurMenuStatus.M_EditStatus & DIGIT_EDIT)) {
		pos = CurMenuStatus.M_EditDigitShiftCnt - CurMenuStatus.M_EditDigitCursor;
		if (isThisSelMenuNmIsSignDigit_1024()) { // goal sensor !
		// ThisSelMenuNm == 4,,,, 등등 +/- 가 있는 메뉴 라면,
			//						+/- 기호 있는 수 		, 로더 변수값 		, 커서위치
			getFinalUserNumWhenUpKey(ThisSignalUserNumbuf, ThisDigitData, pos);
			ThisDigitData = updateThisDigitData(ThisSignalUserNumbuf);
			// 1. +/- 값에 업데이트 한다.
			// 2. 로더 변수 값을 업데이트 한다.
			// ------------------------
		} else { // 일반
			ldr_normal_plus(pos);
		}

		Integer_Digit(); // ThisDigitData값을 여기에서 로더에 표시해 준다.
    } else if ((CurMenuStatus.M_EditStatus & DIGIT_STRING_EDIT)) {
        i = CurMenuStatus.M_EditDigitMaxValue;

        if (i > 0) {
            ThisDigitData = ((ThisDigitData + 1) % i);
        } else {
            ThisDigitData = 0;
        }
        Integer_Digit();
    } else if ((CurMenuStatus.M_EditStatus & DIGIT_ALPHAR_EDIT)) {
        new485Ladder[i] ++;

        if (new485Ladder[i] > 'Z') {
            new485Ladder[i] = '0';
        } else if ((new485Ladder[i] > '9') && (new485Ladder[i] < 'A')) {
            new485Ladder[i] = 'A';
        } else if (new485Ladder[i] < '0') {
            new485Ladder[i] = '0';
        }
    } else if ((CurMenuStatus.M_EditStatus & CLOCK_EDIT)) {
        new485Ladder[i] ++;

        if ((new485Ladder[i] < '0')) {
            new485Ladder[i] = '9';
        } else if (new485Ladder[i] > '9') {
            new485Ladder[i] = '0';
        }
    } else if ((CurMenuStatus.M_EditStatus & TABLE_EDIT)) {
    } else if ((CurMenuStatus.M_EditStatus & TOGGLE_EDIT)) {
    }

    return (0);
}


uint16_t CusorDataDn(void) {
// down key 눌렀을 때 ! => 마이너스
    uint16_t i, dp;
	uint8_t pos;

    i = SECONDLINE_BASE + CurMenuStatus.M_EditStart + CurMenuStatus.M_EditCursor;

	pos = 0;
    if ((CurMenuStatus.M_EditStatus & DIGIT_EDIT)) {
		pos = CurMenuStatus.M_EditDigitShiftCnt - CurMenuStatus.M_EditDigitCursor;

		if (isThisSelMenuNmIsSignDigit_1024()) { // goal sensor !
			getFinalUserNumWhenDnKey(ThisSignalUserNumbuf, ThisDigitData, pos);
			ThisDigitData = updateThisDigitData(ThisSignalUserNumbuf);
		} else { // 일반
			ldr_nomal_minus(pos);
		}

        Integer_Digit();
    } else if ((CurMenuStatus.M_EditStatus & DIGIT_STRING_EDIT)) {
        if (ThisDigitData > CurMenuStatus.M_EditDigitMinValue) {
            ThisDigitData--;
        } else {
            ThisDigitData = (CurMenuStatus.M_EditDigitMaxValue - 1);
        }
        Integer_Digit();
    } else if ((CurMenuStatus.M_EditStatus & DIGIT_ALPHAR_EDIT)) {
        new485Ladder[i] --;

        if ((new485Ladder[i] < '0')) {
            new485Ladder[i] = 'Z';
        } else if ((new485Ladder[i] > '9') && (new485Ladder[i] < 'A')) {
            new485Ladder[i] = '9';
        } else if (new485Ladder[i] > 'Z') {
            new485Ladder[i] = 'Z';
        }
    } else if ((CurMenuStatus.M_EditStatus & CLOCK_EDIT)) {
    } else if ((CurMenuStatus.M_EditStatus & TABLE_EDIT)) {
    } else if ((CurMenuStatus.M_EditStatus & TOGGLE_EDIT)) {
    }

    return (0);
}


uint16_t CusorDigitSet(void) {
    uint16_t i;

    i = SECONDLINE_BASE + CurMenuStatus.M_EditStart + CurMenuStatus.M_EditCursor;

    if ((CurMenuStatus.M_EditStatus & DIGIT_EDIT)) {
        new485Ladder[i] = LoaderKey;
    } else if ((CurMenuStatus.M_EditStatus & DIGIT_ALPHAR_EDIT)) {
        new485Ladder[i] = LoaderKey;
    }

    return (0);
}


uint16_t EditOnCheck(void) {
    if ((CurMenuStatus.M_EditStatus & NO_EDIT)) {
        return (0);

    } else {
        return (1);
    }
}


uint16_t GroupOnChk(void) {
    return (1);
}


uint16_t MenuOnChk(void) {
    return (1);
}


void NewMenuStart(void) {
    EnterKey = 0;
    CurMenuStatus.M_EditCursor = 0;
    CurMenuStatus.M_EditShiftCnt = 0;
    CurMenuStatus.M_EditStatus = 0;
    CurMenuStatus.M_EditStart = 0;
}


uint16_t GroupSelect_Init(void) {
    if (ThisSelMenuNm == UserMenuSerialNm) {
        ThisSelMenuNm = (UserMenuSerialNm - 1);
        CurMenuStatus.M_EditGroupNm = 0xff;
        CurMenuStatus.M_EditSubNm = 0xff;
    }

    return (0);
}


uint16_t GroupSelect(uint16_t UpDn) {
    uint16_t i, m_gr, s_gr;

    GroupSelect_Init();
    i = 0;

    do {
        if (UpDn) {
            ThisSelMenuNm = (ThisSelMenuNm + 1) % UserMenuSerialNm;
        } else {
            if (ThisSelMenuNm == 0) {
                ThisSelMenuNm = (UserMenuSerialNm - 1);
            } else {
                ThisSelMenuNm--;
            }
        }

        m_gr = MenuStatus[ThisSelMenuNm].M_EditGroupNm;
        s_gr = MenuStatus[ThisSelMenuNm].M_EditSubNm;

        if ((m_gr != CurMenuStatus.M_EditGroupNm) && (s_gr == 0)) {
            i = GroupOnChk();
        }
    } while(i == 0);

    NewMenuStart();
    CursorOff();
    return (0);
}


uint16_t SubSelect(uint16_t UpDn) {
    uint16_t i, m_gr;

    GroupSelect_Init();
    i = 0;

    do {
        if (UpDn) {
            ThisSelMenuNm = (ThisSelMenuNm + 1) % UserMenuSerialNm;
        } else {
            if (ThisSelMenuNm == 0) {
                ThisSelMenuNm = (UserMenuSerialNm - 1);
            } else {
                ThisSelMenuNm--;
            }
        }

        m_gr = MenuStatus[ThisSelMenuNm].M_EditGroupNm;

        if (m_gr == CurMenuStatus.M_EditGroupNm) {
            i = 1;
        }
    } while(i == 0);

    NewMenuStart();
    CursorOff();
    return (0);
}


uint16_t KeyCheck(void) {
    uint16_t i, newmenu, ret;

    ret = 0;
    newmenu = 1;
    LoaderKey = LoaderBuf[C_SAVE_DATA + 0];

    switch (LoaderKey) {
        case MENUKEY:
            LoaderKeyTime = 0;
            GroupSelect(1);
            break;

        case ESCKEY:
            if (EnterKey == 2) {
                EnterKey = 1;
            } else if (EnterKey == 1) {
                EnterKey = 0;
                ThisSelMenuNm = UserMenuSerialNm;
            } else {
                ThisSelMenuNm = UserMenuSerialNm;
            }

            CurKeyPoint = 0;
            LoaderKeyTime = 0;
            NewMenuStart();
            CursorOff();
            break;

        case UPKEY:
            CurKeyPoint = ((CurKeyPoint + 1) % 100);
            LoaderKeyTime = 0;

            if ((EnterKey == 1) && (ThisSelMenuNm != UserMenuSerialNm)) {
                SubSelect(1);
                NewMenuStart();
                CursorOff();
            } else if (EnterKey == 2) {
                CusorDataUp();
                newmenu = 0;
            }

            break;

        case DOWNKEY:
            if (CurKeyPoint > 0) {
                CurKeyPoint--;
            } else {
                CurKeyPoint = 99;
            }
            LoaderKeyTime = 0;

            if ((EnterKey == 1) && (ThisSelMenuNm != UserMenuSerialNm)) {
                SubSelect(0);
                NewMenuStart();
                CursorOff();
            } else if (EnterKey == 2) {
                CusorDataDn();
                newmenu = 0;
            }

            break;

        case LEFTKEY:
            LoaderKeyTime = 0;

            if (EnterKey == 1) {
                GroupSelect(1);
            } else if (EnterKey == 2) {
                ShiftLeftCur();
                CursorOn();
                newmenu = 0;
            }

            break;

        case RIGHTKEY:
            LoaderKeyTime = 0;

            if (EnterKey == 1) {
                GroupSelect(0);
            } else if (EnterKey == 2) {
                ShiftRightCur();
                CursorOn();
                newmenu = 0;
            }

            break;

        case ENTKEY:
            newmenu = 0;
            LoaderKeyTime = 0;

            if ((ThisSelMenuNm != UserMenuSerialNm)) {
                EnterKey++;
            }

            if (EnterKey == 2) {
                if (!EditOnCheck()) {
                    EnterKey = 1;
                    CursorOff();
                } else {
                    CursorOn();
                }
            } else if (EnterKey == 3) {
                SaveVerify = 0x55;
                ret = SaveData();
                SaveVerify = 0x0;

                if (ret == 0) {
                    EnterKey = 0;
                }
            }

            break;

        case ONEKEY:
        case TWOKEY:
        case THREEKEY:
        case FOURKEY:
        case FIVEKEY:
        case SIXKEY:
        case SEVENKEY:
        case EIGHTKEY:
        case NINEKEY:
        case ZEROKEY:
            LoaderKeyTime = 0;
            CusorDigitSet();
            newmenu = 0;
            break;

        default:
            LoaderKey = 0xff;
            newmenu = 0;
            break;
    }

    if (newmenu) {
        for (i = 0; i < 16; i++) {
            new485Ladder[i + 2] = blinkx[i];
        }

        for (i = 0; i < 16; i++) {
            new485Ladder[SECONDLINE_BASE + i] = blinkx[i];
        }
    }

    return (0);
}


void updateNewFlash(void) {
    uint16_t i, j;

    if ((LoaderKeyTime > 36000) && (LoaderKeyTime < 36002)) {
        LoaderKeyTime = 36012;
        ThisSelMenuNm = UserMenuSerialNm;
        EnterKey = 0;
    }

    KeyCheck();

    if (EnterKey == 0) {
        EnterKey = 1;

        if (ThisSelMenuNm != UserMenuSerialNm) {
            LaderDsp();
            LoadEditData(ThisSelMenuNm);
        }
    }

    LoaderBuf[C_BASE_MEMORY] = ThisSelMenuNm;
    if (ThisSelMenuNm == UserMenuSerialNm) {
        DefaultDisplay();
    }
}


void Ladder485Init(void) {
    uint16_t i;

    //    ThisSelMenuNm=UserMenuSerialNm;
    NewMenuStart();

    for (i = 0; i < MAX_LADDER_BUF - 2; i++) {
        new485Ladder[i] = ' ';
    }

    new485Ladder[MAX_LADDER_BUF - 1] = 0x0;
}


