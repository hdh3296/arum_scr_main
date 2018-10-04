
#ifndef __loader_com1_port_h__
#define __loader_com1_port_h__

#include    "..\main.h"


extern void Init_Com2(void);
extern void USART2_TXC(void);
extern void USART2_RXC(void);
extern void uart2_txReadyGo(void);
extern uint8_t checkSum(void);

/*
#define RUNLED 			LATC5	//0
#define RXLED       	LATC4 	//0
#define TXLED       	LATD3	//0

#define TX_EN       	LATD2	//0
*/
//////////////////////////////////////////////////
//////////////////////////////////////////////////
#define EOT 0x04
#define ETX 0x03
#define ENQ 0x05
#define ACK 0x06
#define STX_CHK 0
#define ADDRESS_CHK 1
#define COMMAND_CHK 2
#define DATACNT_CHK 3
#define DATA_CHK 4
#define CHKSUM_CHK 5
#define ETX_CHK 6
#define RX_ERROR 7
#define RX_GOOD 8
#define TX_SET 9
#define CHKSUM_LOW_CHK 10
#define CHKSUM_HIGH_CHK 11

#endif

#define TX_ALL 12
#define BCC1_CHK 13
#define BCC2_CHK 14
//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
#define COM1_MAX_TX_BUF 100
#define RX_BUF_MAX 100
extern uint8_t uartTxBuffer[COM1_MAX_TX_BUF];
extern uint8_t rxGoodBuffer[RX_BUF_MAX];
extern uint8_t _uartRxBuffer[RX_BUF_MAX];
extern uint8_t totalTxCnt;
extern uint8_t txBuffer_pt;
extern uint8_t uart_status;
extern uint8_t rxCnt;
extern uint16_t serialIdleTimer;
extern uint16_t uart2_baudrate;

extern unsigned char get485TxBuffer(unsigned int i);

extern uint8_t uart_status;

