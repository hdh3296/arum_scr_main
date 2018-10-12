
#include        "loader_45k80_main.h"
#include        "..\initial.h"
#include        "..\main.h"

uint16_t updateNewFlashTimer;


// --------




uint16_t initLoaderUart2(void) {

    uart2_baudrate = 19200;
    Init_Com2();
    serialIdleTimer = 0;
    TX2_IE = 0;
//	tx485_enPin = DIS_485;
    RC2_IE = 1;
    uart_status = STX_CHK;
    Ladder485Init();
    Menu_Status_Set();
    ThisSelMenuNm = UserMenuSerialNm;
    return (0);
}


uint16_t getTxTotalCnt(void) {
    uint16_t i;
    for (i = 0; uartTxBuffer[i]; i++);
    return i;
}


void txReadyAndStartForLoader(uint8_t txbuf[]) {
/*
	로더에 display 할 아스키 값 tx

	* 로더에서는 이 값을 그대로 display 한다.
*/
    uint16_t i;

    if (updateNewFlashTimer > 100) {
        LoaderBuf[C_SAVE_DATA + 0] = 'N';
        updateNewFlash();
        updateNewFlashTimer = 0;
    }

    txbuf[0] = ACK;
    txbuf[1] = 'A';
    txbuf[36] = ' ';
    txbuf[37] = ' ';
    txbuf[38] = ' ';
    txbuf[39] = ' ';
    txbuf[40] = ' ';
    txbuf[41] = ETX;
    txbuf[42] = 0;

    for (i = 0; i < 36; i++) {
        if (new485Ladder[i + 0] > 0x80) {
            txbuf[2 + i] = new485Ladder[i + 0] -0x80;
        } else {
            txbuf[2 + i] = new485Ladder[i + 0];
        }

        if (new485Ladder[i + 0] == 0) {
            txbuf[2 + i] = new485Ladder[i + 0] = ' ';
        }
    }
    TxImportExportChk();

    checkSum();
    totalTxCnt = getTxTotalCnt();
    uart2_txReadyGo();
}


uint8_t getAsciiFirstTo(uint8_t hex) {
    uint8_t i;

    // hex 앞 자리
    i = (hex & 0xf0) >> 4;

    if (i < 0x0a) {
        i = i + '0';
    } else {
        i = i + '7';
    }

    return i;
}

uint8_t getAsciiSecondTo(uint8_t hex) {
    uint8_t i;

    // hex 뒷자리
    i = (hex & 0x0f);

    if (i < 0x0a) {
        i = i + '0';
    } else {
        i = i + '7';
    }
    return i;
}


void loadChUse(uint8_t txbuf[]) {
    uint8_t ch, j, i;

    j = 0;
    for (ch = 0; ch < ZSU_CH_MAX; ch++) {
        txbuf[14 + j] = getAsciiFirstTo((uint8_t)bufZSU_use_not[ch]);
        txbuf[15 + j] = getAsciiSecondTo((uint8_t)bufZSU_use_not[ch]);
        j += 10;
    }
}


void txReadyAndStartForTemp(uint8_t txbuf[]) {
/*
    온도 보드에 데이터 요청 하기

        * tx buffer 에 해당 바이트 저장
        * 체크섬 값 계산해서 저장
        * 이후, tx start !
*/
    uint16_t i;

    for(i = 0; i < COM1_MAX_TX_BUF; i++) {
        txbuf[i] = ' ';
    }

    txbuf[0] = ACK;
    txbuf[1] = 'T';
    txbuf[2] = 'M';
    txbuf[3] = ',';

	loadChUse(txbuf);

    txbuf[96] = ETX;
    txbuf[97] = 0; // checksum high
    txbuf[98] = 0; // checksum low
    txbuf[99] = 0; // 반드시 0

    checkSum();

    totalTxCnt = getTxTotalCnt();
    uart2_txReadyGo();
}





void loadPwm0_100percentTo(uint8_t txbuf[]) {
    uint8_t ch, j;

    j = 0;
    for (ch = 0; ch < 5; ch++) {
        txbuf[14 + j] = getAsciiFirstTo(heater[ch].db_heatingOnState);
        txbuf[15 + j] = getAsciiSecondTo(heater[ch].db_heatingOnState);
        j += 10;
    }
}

void loadErrorInfoTo(uint8_t txbuf[]) {
    uint8_t ch, j;

    j = 0;
    for (ch = 0; ch < 5; ch++) {
        txbuf[18 + j] = getAsciiFirstTo(heater[ch].db_errorCode);
        txbuf[19 + j] = getAsciiSecondTo(heater[ch].db_errorCode);
        j += 10;
    }
}

void loadChEnable(uint8_t txbuf[]) {
    uint8_t ch, j, i;
	uint8_t enable_set_info[5] = {0,};

    j=0;
    for (ch = 0; ch < 5; ch++) {
		enable_set_info[ch] = (uint8_t)heater[ch].db_bFlashChEnable_state & 0x01;
        txbuf[16 + j] = getAsciiFirstTo(enable_set_info[ch]);
        txbuf[17 + j] = getAsciiSecondTo( enable_set_info[ch]);
        j += 10;
    }

}


// ------------------

void loadMyInfoTo(uint8_t txbuf[]) {
    // pwm 어떤 정보?
    // * 입력 전압, 전류가 정상적인지 제어 되고 있는지 여부
    // * ch 별 에러 여부 정보
    // 히터 on, off (pwm) 상태 정보

    loadPwm0_100percentTo(txbuf);

    loadErrorInfoTo(txbuf);

    loadChEnable(txbuf);


}

void txReadyAndStartForPannel(uint8_t txbuf[]) {
    uint16_t i;

    for(i = 0; i < COM1_MAX_TX_BUF; i++) {
        txbuf[i] = ' ';
    }

    txbuf[0] = ACK;
    txbuf[1] = 'P';
    txbuf[2] = 'M';
    txbuf[3] = ',';
    txbuf[96] = ETX;
    txbuf[97] = 0; // checksum high
    txbuf[98] = 0; // checksum low
    txbuf[99] = 0; // 반드시 0

    loadMyInfoTo(txbuf); // main -> pannel

    checkSum();
    totalTxCnt = getTxTotalCnt();
    uart2_txReadyGo();
}



bool uart_RxGoodSucess(void) {
    if (uart_status == RX_GOOD) {
        uart_status = STX_CHK;
        if (rxGoodSucessTimer > RX_GOOD_SUCESS_T) {
            rxGoodSucessTimer = 0;
            return 1;
        }
    }
    return 0;
}

bool uart_isIdelState(void) {
    return (uart_status != TX_SET) && (serialIdleTimer > 50);
}


void trasmitToLoader(void) {
/*
	로더에 표시할 아스키 값을 보낸다.

	로더에서는 이 값을 받으면 자신의 저장된 키 값을 메인으로 보내온다.
*/
    txReadyAndStartForLoader(uartTxBuffer);
}

void trasmitToTemp(void) {
    txReadyAndStartForTemp(uartTxBuffer);

}


void trasmitToPannel(void) {
    txReadyAndStartForPannel(uartTxBuffer);
}


void ldr_chkRxKey(void) {

    chkRxImExport();
    LoaderBuf[C_SAVE_DATA + 0] = rxGoodBuffer[2];
    updateNewFlash();
    updateNewFlashTimer = 0;
}

void procTxComunication(void) {
    static uint8_t sequence, old_sequence;
	serialIdleTimer = 0;
    switch (sequence) {
        case 0:
            trasmitToLoader();
			if (old_sequence == 1) {
            	sequence = 2;
			} else if (old_sequence == 2) {
				sequence = 1;
			} else {
				sequence = 1;
			}

            if (bRequestWriteState) {
                sequence = 0;
                bRequestWriteState = 0;
            }
            break;
        case 1:
			// pannel 데이터 요청하기 (tx)
            trasmitToPannel();
            sequence = 0;
			old_sequence = 1;
            break;
        case 2:
            // 온도 데이터 요청하기 (tx)
            trasmitToTemp();
            sequence = 0;
			old_sequence = 2;
            break;

        default:
            sequence = 0;
            break;
    }

}



enum {
    NONE,
    FROM_LDR,
    FROM_TEMP,
    FROM_PANNEL,
};
uint8_t fromWhoRx(uint8_t type, uint8_t who, uint8_t to) {
    if (type == 'A') {
        return FROM_LDR;
    }
    return FROM_LDR;
}



void loader_msecTimer(void) {
    serialIdleTimer++;

    if (updateNewFlashTimer < 20002) {
        updateNewFlashTimer++;
    }
}

void errorItr(void) {
    if (OERR2) {
         TXEN2 = 0;
         TXEN2 = 1;
         SPEN2 = 0;
         SPEN2 = 1;
         CREN2 = 0;
     }

     if (!CREN2) {
         CREN2 = 1;
     }
}

void loader_uart_interrupt(void) {

    if ((TX2_IE) && (TX2_IF)) {
        TX2_IF = 0;
        USART2_TXC();
    }

    if ((RC2_IE) && (RC2_IF)) {
        RC2_IF = 0;
        USART2_RXC();
    }

    errorItr();
}


