
#include        "loader_45k80_main.h"
/*
#include	"loader_setup.h"
#include	"UserDesign.h"
*/
far uint8_t * source_ptr = (far uint8_t *)
                           new_value; /*pointers to data*/
far uint8_t * dest_ptr = (far uint8_t *)
                         information;
uint16_t size = (FLASH_ONE_BLOCK_SIZE); // 64


uint16_t FlashBlockRd(uint16_t BlkNm) {
/*
	플래쉬 영역 값을 쓰고자 할때
	에 이 함수를 제일 먼저 호출해야 한다.

	※ 주의 :   new_value 는 long 타입니다.
*/
    uint16_t i, start;

    if (BlkNm <= MAX_FLASH_BLOCK_NM) {
        start = (BlkNm * (FLASH_ONE_BLOCK_SIZE / 4)); // ※ new_value 가 long 타입이므로 나누기 4 해줌.
        for (i = 0; i < (size / FLASH_ONE_RECORD_BYTE); i++) { // 64 / 4 = 16
            new_value[i].long_data = information[start + i].long_data;
        }

        return (1);
    }

    return (0);
}


uint16_t FlashBlockWr(uint16_t BlkNm) {
/*
	플래쉬 영역 값을 쓰고자 할때
	에 이 함수를 제일 마지막에 불러야 한다.
*/

    uint16_t start;

    if (BlkNm <= MAX_FLASH_BLOCK_NM) {
        start = (BlkNm * FLASH_ONE_BLOCK_SIZE);
        flash_write(source_ptr, size, (dest_ptr + start));
        FlashBlockRd(BlkNm);
        return (1);
    }

    return (0);
}


void SetClearBit(uint8_t * TargetBuf, uint8_t value, uint8_t bitnm) {
    uint8_t bitvalue;

    bitvalue = 0x01;
    bitvalue = (bitvalue << bitnm);

    if (*TargetBuf & bitvalue) {
        if (value == 1) {
            *TargetBuf = (*TargetBuf & ~bitvalue);
        }
    } else {
        if (value > 1) {
            *TargetBuf = (*TargetBuf | bitvalue);
        }
    }
}


void SetClearByte(uint8_t * TargetBuf, uint8_t value) {
    if (value > 0) {
        value = (value - 1);

        if (value != *TargetBuf) {
            *TargetBuf = value;
        }
    }
}


/*
void	CmdSetupChk(uint8_t id)
{

    uint8_t	tmpb;
    uint16_t    IdPt;

    IdPt=IsBufferPt(id);

    if((RcvBuf[IdPt+DSP1] == '9') && (RcvBuf[IdPt+DSP2] == '1') && !(RcvBuf[IdPt+1] & S1_AUTO) && ((RcvBuf[IdPt+SL_
    mCarOpCl] & 0xc0)== 0xc0) ){
        tmpb= (RcvBuf[IdPt+SL_mCarOpCl] & 0x03);
        SetClearBit(&new_value[6],tmpb,0);

        tmpb= (RcvBuf[IdPt+SL_mCarOpCl] & 0x0c);
        tmpb=(tmpb >> 2);
        SetClearBit(&new_value[6],tmpb,1);

        tmpb= (RcvBuf[IdPt+SL_mCarOpCl] & 0x30);
        tmpb=(tmpb >> 4);
        SetClearBit(&new_value[6],tmpb,2);


        tmpb= (RcvBuf[IdPt+SL_mCarKey25] & 0x03);
        SetClearBit(&new_value[6],tmpb,3);

        tmpb= (RcvBuf[IdPt+SL_mCarKey25] & 0x0c);
        tmpb=(tmpb >> 2);
        SetClearBit(&new_value[6],tmpb,4);


        SetClearByte( &new_value[7],RcvBuf[IdPt+SL_mCarKey1]);
        SetClearByte( &new_value[4],RcvBuf[IdPt+SL_mCarKey9]);
        SetClearByte( &new_value[3],RcvBuf[IdPt+SL_mCarKey17]);
        SetClearByte( &new_value[0],(id+1));
    }
 }

*/
