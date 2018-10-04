
#ifndef BIT_STR
#define BIT_STR 1


#include    "..\main.h"



/*
typedef unsigned    char			uint8_t;
typedef unsigned	short	int 	uint16_t;
typedef unsigned    int 			uint32_t;
typedef unsigned    long 			uint32_t;


typedef	signed		char 			int8_t;
typedef	signed		short	int 	int16_t;
typedef	signed      int 			int32_t;
typedef signed    	long 			int32_t;
*/
#define FLASH_ONE_BLOCK_SIZE 64	//64 byte  -- flash rd/wr block
#define FLASH_ONE_RECORD_BYTE 4	//long type== 4byte

typedef struct _bit_struct {
    unsigned bit0 : 1;
    unsigned bit1 : 1;
    unsigned bit2 : 1;
    unsigned bit3 : 1;
    unsigned bit4 : 1;
    unsigned bit5 : 1;
    unsigned bit6 : 1;
    unsigned bit7 : 1;
} bit_field;


typedef union _long_union {
    uint8_t byte[4];
    uint16_t intger[2];
    uint32_t long_data;
} long_field;




#define GET_BITFIELD(addr) (*((volatile bit_field*) (addr)))
#define GET_LONGFIELD(addr) (*((volatile long_field*) (addr)))
#define GET_LONGFIELD_BYTE0(addr) (*((volatile long_field*) (addr))).byte[0]
#define GET_LONGFIELD_BYTE1(addr) (*((volatile long_field*) (addr))).byte[1]
#define GET_LONGFIELD_BYTE2(addr) (*((volatile long_field*) (addr))).byte[2]
#define GET_LONGFIELD_BYTE3(addr) (*((volatile long_field*) (addr))).byte[3]
#define cF_ByteData(adr) GET_LONGFIELD(&information[(adr/4)])					.byte[(adr	% 4)]
#define iF_IntData(adr) GET_LONGFIELD(&information[adr/4])						.intger[(adr%4)/2]
#define lF_LongData(adr) GET_LONGFIELD(&information[(adr/4)])					.long_data
#define cSR_ByteData(adr) GET_LONGFIELD(&new_value[((adr % FLASH_ONE_BLOCK_SIZE)/4)]).byte[(adr%4)]
#define iSR_IntData(adr) GET_LONGFIELD(&new_value[((adr % FLASH_ONE_BLOCK_SIZE)/4)]).intger[((adr % 4 ) / 2)]
#define lSR_LongData(adr) GET_LONGFIELD(&new_value[((adr % FLASH_ONE_BLOCK_SIZE)/4)]).long_data


#endif

