

#ifndef __LOADER_45K80_MAIN_H__
#define __LOADER_45K80_MAIN_H__

#include    <pic18.h>


#include    "..\system\system.h"
#include    "loader_UserDesign.h"
#include	"loader_cmd.h"
#include	"loader_com1_port.h"
#include	"loader_serial.h"
#include	"loader_setup.h"
#include	"loader_flash.h"

#include    "..\main.h"



extern uint16_t initLoaderUart2(void);
extern void rxtx_loop(void);
extern void loader_msecTimer(void);
extern void loader_uart_interrupt(void);
extern uint8_t UserSystemStatus;
extern uint8_t UserRam_8[];
extern uint16_t UserRam_16[];
extern uint32_t UserRam_32[];
extern uint8_t LoaderBitAreaSram1;

#define bExportData GET_BITFIELD(&LoaderBitAreaSram1).bit0
#define bImportData GET_BITFIELD(&LoaderBitAreaSram1).bit1

#define TX2_IE TX2IE
#define TX2_IF TX2IF
#define RC2_IE RC2IE
#define RC2_IF RC2IF

extern bool uart_isIdelState(void);
extern void procTxComunication(void);
extern bool uart_RxGoodSucess(void);
extern void ldr_chkRxKey(void);


#endif

