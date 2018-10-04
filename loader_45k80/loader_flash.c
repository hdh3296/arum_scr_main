
#include        "loader_45k80_main.h"
#define ASCTOHEX(x) ((x <= '9') ? (x - '0') : (x - '7'))
#define BACKUP_COM_BLK 16  //16 byte hex --> ascii 32 byte
uint16_t LoaderChk = 0;
uint16_t SrcAdrBlk = 0;
uint16_t src, firstdata, seconddata;

bool bRequestWriteState;

void HextoASCIIByte(void) {
    uint16_t i;

    i = (src & 0xf0) >> 4;

    if (i < 0x0a) {
        i = i + '0';
    } else {
        i = i + '7';
    }
    firstdata = i;
    i = (src & 0x0f);

    if (i < 0x0a) {
        i = i + '0';
    } else {
        i = i + '7';
    }
    seconddata = i;
}


void DataLoadSBuf(void) {
    uint16_t i, j, k;

    i = ((SrcAdrBlk * BACKUP_COM_BLK) / FLASH_ONE_BLOCK_SIZE); //pointer search in block
    FlashBlockRd(i);
    i = ((SrcAdrBlk * BACKUP_COM_BLK) % FLASH_ONE_BLOCK_SIZE); //pointer search in block

    for (j = 0; j < BACKUP_COM_BLK; j++) {
        //16 byte load(ascii == 32 byte)
        k = (j * 2);
        src = (uint8_t) (cSR_ByteData((i + j)));
        HextoASCIIByte();
        uartTxBuffer[5 + k] = (uint8_t)
                              firstdata;
        uartTxBuffer[6 + k] = (uint8_t)
                              seconddata;
    }

    k = (j * 2);
    uartTxBuffer[5 + k] = ETX;
    uartTxBuffer[6 + k] = 0;
}


void IntegerToString(uint16_t i) {
    i = (i % 100);
    uartTxBuffer[3] = (uint8_t) ((i / 10) + '0');
    uartTxBuffer[4] = (uint8_t) ((i % 10) + '0');
}


void DataDumpToLoader(void) {
/*
    main -> loader
*/
    if (((SrcAdrBlk * BACKUP_COM_BLK) <= (MAX_FLASH_BLOCK_NM * FLASH_ONE_BLOCK_SIZE)) && (LoaderChk < 20)) {
        //////////////////////	if((SrcAdrBlk < BACKUP_MAX_BLOCK) && (LoaderChk < 20) ){    // area check and tim out check
        uartTxBuffer[0] = ACK;
        uartTxBuffer[1] = 'W'; // main 입장에서 write !!!
        uartTxBuffer[2] = 'F';
        IntegerToString(SrcAdrBlk);
        DataLoadSBuf();
        LoaderChk++;
        bRequestWriteState = 1;
    } else {
        MenuStatus[ThisSelMenuNm].M_EditCurDigitData = 0;
    }
}


void ReceiveDataLoad(void) {
    unsigned int i, j, k;

    j = ((SrcAdrBlk * BACKUP_COM_BLK) % FLASH_ONE_BLOCK_SIZE); //pointer search in block

    for (i = 0; i < BACKUP_COM_BLK; i++) {
        k = (i * 2);
        src = (uint8_t) (cSR_ByteData((i + j)));
        HextoASCIIByte();

        if ((firstdata < '0') || (firstdata > 'F')) {
            firstdata = '0';
        }

        if ((seconddata < '0') || (seconddata > 'F')) {
            seconddata = '0';
        }

        uartTxBuffer[5 + k] = firstdata;
        uartTxBuffer[6 + k] = seconddata;
    }

    uartTxBuffer[37] = ETX;
    uartTxBuffer[38] = 0;
}


void DataReqToLoader(void) {
/*
    main <-- loader
*/
    if (((SrcAdrBlk * BACKUP_COM_BLK) <= (MAX_FLASH_BLOCK_NM * FLASH_ONE_BLOCK_SIZE)) && (LoaderChk < 20)) {
        uartTxBuffer[0] = ACK;
        uartTxBuffer[1] = 'R';
        uartTxBuffer[2] = 'F';
        IntegerToString(SrcAdrBlk);
        ReceiveDataLoad();
        LoaderChk++;
        // main <-- loader
        // read 시에
        // 여기 제일 처음으로 온다.
        bRequestWriteState = 1;
    } else {
        MenuStatus[ThisSelMenuNm].M_EditCurDigitData = 0;
    }
}


uint16_t ReceiveCmp(void) {
    uint16_t i, j, k, good;

    good = 1;
    j = ((SrcAdrBlk * BACKUP_COM_BLK) % FLASH_ONE_BLOCK_SIZE); //pointer search in block

    for (i = 0; i < BACKUP_COM_BLK; i++) {
        k = (i * 2);
        src = (uint8_t) (cSR_ByteData((i + j)));
        HextoASCIIByte();

        if (rxGoodBuffer[5 + k] != (uint8_t)
                firstdata) {
            good = 0;
        }

        if (rxGoodBuffer[6 + k] != (uint8_t)
                seconddata) {
            good = 0;
        }
    }

    if (rxGoodBuffer[0] != ACK) {
        good = 0;
    }

    if (rxGoodBuffer[1] != 'w') {
        good = 0;
    }

    if (rxGoodBuffer[2] != 'F') {
        good = 0;
    }

    return (good);
}


void LoaderDataWriteToMe(void) {
    uint16_t i, j, k, l, pt;

    i = (uint16_t)
        rxGoodBuffer[3];
    i = ASCTOHEX(i);
    j = (uint16_t)
        rxGoodBuffer[4];
    j = ASCTOHEX(j);
    SrcAdrBlk = ((i * 10) +j);

    if ((SrcAdrBlk * BACKUP_COM_BLK) > (MAX_FLASH_BLOCK_NM * FLASH_ONE_BLOCK_SIZE)) {
        SrcAdrBlk = 0;
    }

    if (SrcAdrBlk > 0) {
        if (((SrcAdrBlk * BACKUP_COM_BLK) % FLASH_ONE_BLOCK_SIZE) == 0) {
            i = (SrcAdrBlk * BACKUP_COM_BLK) / FLASH_ONE_BLOCK_SIZE;
            i = i - 1;
            FlashBlockWr(i);
        }
    }

    if ((SrcAdrBlk * BACKUP_COM_BLK) == (MAX_FLASH_BLOCK_NM * FLASH_ONE_BLOCK_SIZE)) {
        MenuStatus[ThisSelMenuNm].M_EditCurDigitData = 0;
    }

    pt = ((SrcAdrBlk * BACKUP_COM_BLK) % FLASH_ONE_BLOCK_SIZE);

    for (i = 0; i < BACKUP_COM_BLK; i++) {
        l = (i * 2);
        k = (uint16_t)
            rxGoodBuffer[5 + l];
        k = ASCTOHEX(k);
        k = ((k << 4) & 0xf0);
        j = (uint16_t)
            rxGoodBuffer[6 + l];
        j = ASCTOHEX(j);
        j = (k | j);
        cSR_ByteData((pt + i)) = (uint8_t) (j);
    }
}


uint16_t chkRxImExport(void) {
    if (MenuStatus[ThisSelMenuNm].M_EditFlashAddr != F_FLASH_COPY) {
        return (0);

    }

    if (MenuStatus[ThisSelMenuNm].M_EditCurDigitData == EXPORT_DATA) {
        if (ReceiveCmp()) {
            SrcAdrBlk++;
        }

        LoaderBuf[C_SAVE_DATA + 0] = 'N';
        LoaderChk = 0;
        LoaderKeyTime = 0;
    } else if (MenuStatus[ThisSelMenuNm].M_EditCurDigitData == IMPORT_DATA) {
        if ((rxGoodBuffer[0] == ACK) && (rxGoodBuffer[1] == 'r') && (rxGoodBuffer[2] == 'F')) {
            // main <- loader
            // read 시에
            // loader에서 rF 값을 보내면 여기로 들어 온다. (두번 째)
            SaveVerify = 0x55;
            LoaderDataWriteToMe();
            SaveVerify = 0;
            rxGoodBuffer[0] = 'n';
            rxGoodBuffer[1] = 'n';
            rxGoodBuffer[2] = 'n';
        }

        LoaderBuf[C_SAVE_DATA + 0] = 'N';
        LoaderChk = 0;
        LoaderKeyTime = 0;
    }

    return (0);
}


uint16_t TxImportExportChk(void) {
    if (MenuStatus[ThisSelMenuNm].M_EditFlashAddr != F_FLASH_COPY) {
        LoaderChk = 0;
        return (0);
    }

    if (MenuStatus[ThisSelMenuNm].M_EditCurDigitData == EXPORT_DATA) {
        DataDumpToLoader(); // main -> loader = data request
    } else if (MenuStatus[ThisSelMenuNm].M_EditCurDigitData == IMPORT_DATA) {
        DataReqToLoader();
    } else {
        SrcAdrBlk = 0;
    }

    return (0);
}


