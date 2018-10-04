
#include        "loader_45k80_main.h"
#include		"..\initial.h"
/*
#include    "loader_com1_port.h"
#include    "loader_serial.h"
*/
///////////////////////
#define ASCTOHEX(x) ((x <= '9') ? (x - '0') : (x - '7'))
#define L_SHIFT(x) (x << 4)
uint8_t uartTxBuffer[COM1_MAX_TX_BUF];
uint8_t rxGoodBuffer[RX_BUF_MAX];
uint8_t _uartRxBuffer[RX_BUF_MAX]; // 임시 버퍼, rx good 이전
uint8_t totalTxCnt = 0;
uint8_t txBuffer_pt = 0;
uint8_t uart_status;
uint8_t rxCnt = 0;
uint16_t serialIdleTimer;
uint16_t uart2_baudrate = 19200;
uint16_t Crc;
uint16_t Chksum1;

void Init_Com2(void) {
    uint16_t tmpbaudrate;

    SPBRG2 = DIVIDER;
    tmpbaudrate = ((uint16_t) (FOSC / (16UL * uart2_baudrate) - 1));
    SPBRG2 = tmpbaudrate;
    TXSTA2 = (SPEED | NINE_BITS | 0x20);
    RCSTA2 = (NINE_BITS | 0x90);
    TRISE7 = OUTPUT; // tx
    TRISE6 = INPUT; // rx
    RC2_IE = 1; // USART RX interrupt enable
    TX2_IE = 0; // USART TX interrupt enable
    tx485_enPin = DIS_485;
    RC2_IF = 0; // USART RX interrupt enable
    TX2_IF = 0; // USART TX interrupt enable
    UART1MD = 1;
    UART2MD = 0;
}


void Crc_Calulate(uint16_t crcdata) {
    register uint16_t i;

    Crc = Crc ^ (crcdata & 0x00ff);

    for (i = 0; i <= 7; i++) {
        if ((Crc & 0x0001) == 0x0001) {
            Crc = (Crc >> 1) ^ 0xA001;
        } else {
            Crc = Crc >> 1;
        }
    }
}


uint8_t checkSum(void) {
    uint8_t i;
    uint8_t temp;

    Chksum1 = 0;

    for (i = 0; uartTxBuffer[i]; i++) {
        Chksum1 = Chksum1 + uartTxBuffer[i];
    }

    temp = (Chksum1 & 0xf0) >> 4;

    if (temp < 0x0a) {
        temp = temp + '0';
    } else {
        temp = temp + '7';
    }
    uartTxBuffer[i] = temp;
    temp = (Chksum1 & 0x0f);

    if (temp < 0x0a) {
        temp = temp + '0';
    } else {
        temp = temp + '7';
    }
    uartTxBuffer[i + 1] = temp;
    uartTxBuffer[i + 2] = 0;
    return (0);
}


void uart2_txReadyGo(void) {
    uart_status = TX_SET;
    TXREG2 = uartTxBuffer[0];
	txBuffer_pt = 1;
    TX2_IE = 1;
    tx485_enPin = EN_485;

    can_txReady();
}

bool isTxEnd(void) {
	return (txBuffer_pt > totalTxCnt);
}

bool isTxBufOver(void) {
	return txBuffer_pt >= COM1_MAX_TX_BUF;
}

bool isTxEndCondition() {
	return isTxEnd() || isTxBufOver();
}

void setTxStop(void) {
	uart_status = STX_CHK;
	TX2_IE = 0;
	tx485_enPin = DIS_485;
	serialIdleTimer = 0;
	txBuffer_pt = 0;
	totalTxCnt = 0;
}

void USART2_TXC(void) {
    if (isTxEndCondition())	{
		setTxStop();
		return;
    }

    TXREG2 = uartTxBuffer[txBuffer_pt];
    txBuffer_pt++;
    uart_status = TX_SET;
	serialIdleTimer = 0;
}


void USART2_RXC(void) {
    uint8_t buf = 0;
    uint8_t temp = 0;

    buf = RCREG2;
    serialIdleTimer = 0;


    if (uart_status != TX_SET) {
        if (rxCnt < (RX_BUF_MAX - 1)) {
            rxCnt++;
        } else {
            rxCnt = 0;
        }

        _uartRxBuffer[rxCnt] = buf;

        switch (uart_status) {
            case STX_CHK:
                if ((buf == ACK) || (buf == ENQ)) {
                    uart_status = ETX_CHK;
                    _uartRxBuffer[0] = buf;
                    rxCnt = 0;
                    Chksum1 = buf;
                }
                break;

            case ETX_CHK:
                Chksum1 = Chksum1 + buf;

                if ((buf == ETX) || (buf == EOT)) {
                    _uartRxBuffer[rxCnt] = 0x0;
                    uart_status = BCC1_CHK;
                }
                break;

            case BCC1_CHK:
                buf = ASCTOHEX(buf);
                _uartRxBuffer[rxCnt] = 0x0;
                temp = (Chksum1 & 0xf0) >> 4;

                if (temp == buf) {
                    uart_status = BCC2_CHK;
                } else {
                    uart_status = STX_CHK;
                }
                break;

            case BCC2_CHK:
                buf = ASCTOHEX(buf);
                temp = (Chksum1 & 0x0f);
                _uartRxBuffer[rxCnt] = 0x0;
                _uartRxBuffer[RX_BUF_MAX - 1] = 0x0;

                if (temp == buf) {
                    uart_status = RX_GOOD;
                } else {
                    uart_status = STX_CHK;
                }
                break;

            case RX_ERROR:
                uart_status = STX_CHK;
                rxCnt = 0;
                break;

            default:
                uart_status = STX_CHK;
                rxCnt = 0;
                break;
        }
    }
}


unsigned char get485TxBuffer(unsigned int i) {
    return uartTxBuffer[i];
}

