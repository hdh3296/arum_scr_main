

#include    <pic18.h>
#include 	"my_can.h"


#define		CPU65K80	1
#define		U_COMPILER		1


#if defined(CPU65K80)  || defined(CPU45K80)

#define	TXB0REQ		TXB0CONbits.TXREQ // Transmit Request Status bit

#define	RXB0FUL		RXB0CONbits.RXFUL
#define	RXB0FILHIT0 RXB0CONbits.FILHIT0
#define	RXB0FILHIT1 RXB0CONbits.FILHIT1
#define	RXB0FILHIT2 RXB0CONbits.FILHIT2
#define	RXB0RTRRO 	RXB0CONbits.RXRTRRO

#define	TXB1REQ		TXB1CONbits.TXREQ
#define	RXB1FUL		RXB1CONbits.RXFUL
#define	RXB1FILHIT0 RXB1CONbits.FILHIT0
#define	RXB1FILHIT1 RXB1CONbits.FILHIT1
#define	RXB1FILHIT2 RXB1CONbits.FILHIT2
#define	RXB1RTRRO 	RXB1CONbits.RXRTRRO


#define	RXB0M0		RXB0CONbits.RXM0
#define	RXB0M1		RXB0CONbits.RXM1
#define	RXB0DBEN	RXB0CONbits.RB0DBEN



#define	RXB1M0		RXB1CONbits.RXM0
#define	RXB1M1		RXB1CONbits.RXM1
#define	RXB1DBEN	RXB1CONbits.RB1DBEN


#define	RXB0IF		PIR5bits.RXB0IF
#define	RXB1IF		PIR5bits.RXB1IF


#define	RXF0EXIDEN	RXF0SIDLbits.EXIDEN
#define	RXF1EXIDEN	RXF1SIDLbits.EXIDEN
#define	RXF2EXIDEN	RXF2SIDLbits.EXIDEN
#define	RXF3EXIDEN	RXF3SIDLbits.EXIDEN
#define	RXF4EXIDEN	RXF4SIDLbits.EXIDEN
#define	RXF5EXIDEN	RXF5SIDLbits.EXIDEN



#define	TXB0IE		TXBIEbits.TXB0IE
#define	TXB1IE		TXBIEbits.TXB1IE
#define	TXB2IE		TXBIEbits.TXB2IE

#define	RXB0IE		BIE0bits.RXB0IE
#define	RXB1IE		BIE0bits.RXB1IE


#define	TXB2REQ		TXB2CONbits.TXREQ

#define	TXB0RTR		TXB0DLCbits.TXRTR

#endif






extern     	void         	MyLampCheck(void);
extern		unsigned char  	SyncButtonCheck(unsigned char src);

extern		void			CmdSetupChk(unsigned char id);


extern      unsigned    char    CallMeAdr;
extern  	unsigned    char    LocalNumber;
extern      unsigned    char    SrcAddress;

extern      bit    MaskSetBit;              //new

unsigned    char    NoCanInt = 0;
unsigned    int     CanTime = 0;
unsigned    char    _can_rxBuffer[MAX_SAVE_BUF];
unsigned    char    can_txBuffer[MAX_SAVE_BUF];
unsigned    char    bCanTxAct;
unsigned    char    CanRxDataRxPointer;
unsigned    int     Can1TxDataTotalCnt;
unsigned    int     CanTxDataTxPointer;

unsigned    char    can_8data[8];
unsigned    char    CanCmd;
unsigned    char    Company = 'A';
unsigned    char    myAdr = 0; // can my addr

unsigned    char    CAN_ReceivedAdr = 0; // can your addr
unsigned    char    CanReceiveCnt;

unsigned    long    canTx_EidBuffer;
unsigned    int     canTx_SidBuffer;
unsigned    char    can_txDlc;
unsigned    long    RxEidBuffer;
unsigned    int     RxSidBuffer;

unsigned    char    CanLiveTimeAdr[MAX_ELEV];
unsigned    char    Can1RxDlc;
unsigned    char    can_thisRxBuf[8];



//unsigned    char    CarkeyBuf[8];

///////////////////////


void CAN_SetPort(void)//------------------------------------------------------------------------
{
    TRISB3 = 1 ; // CAN RX
    TRISB2 = 0 ; // CAN TX

#ifdef		 U_COMPILER
    CANCONbits.REQOP = 4 ;// Entry Configration Mode

#else
    REQOP2 = 1 ;// Entry Configration Mode
    REQOP1 = 0 ;
    REQOP0 = 0 ;
#endif


}


void CAN_SetBaudrate(void)
{
    SEG2PHTS = 1 ;      // Freely programmable
    SAM      = 1 ;      // Bus Line is sampled three times prior to the sample point

    SEG2PH2	 = 0 ;      // 2Tq
    SEG2PH1	 = 0 ;
    SEG2PH0	 = 1 ;

    PRSEG2	 = 0 ;      // 3xTq
    PRSEG1	 = 1 ;
    PRSEG0	 = 0 ;

    SEG1PH2	 = 0 ;      // 2Tq
    SEG1PH1	 = 0 ;
    SEG1PH0	 = 1 ;


    SJW1    = 0 ;       // 1Tq
    SJW0    = 0 ;


    BRP5    = 0 ;       // Tq = (2x4)/Fosc = (2x4)/8000000 = 1us
    BRP4    = 1 ;
    BRP3    = 0 ;
    BRP2    = 0 ;
    BRP1    = 1 ;  // BRP is setting value 4 when Fosc is 8 Mhz ( tq=0.00000125 )
    BRP0    = 1 ;


    BRP5    = 1 ; // Tq = 0.00000125/2
    BRP4    = 0 ;
    BRP3    = 0 ;
    BRP2    = 1 ;
    BRP1    = 1 ;
    BRP0    = 1 ;

}


void CAN_SetFilter_SUB(char filter_no, unsigned int sid, unsigned long eid)
{
    unsigned int uppereid;
    volatile unsigned char * pt;

    sid     = (sid << 5) & 0xffe0;
    eid     = (eid & 0x0003ffff);
    uppereid = (unsigned int)(eid >> 16);
    sid     = (sid | uppereid);

    uppereid = (unsigned int)(eid);


    switch (filter_no)
    {
    case 0:
        pt = (&RXF0SIDH);
        break;
    case 1:
        pt = (&RXF1SIDH);
        break;
    case 2:
        pt = (&RXF2SIDH);
        break;
    case 3:
        pt = (&RXF3SIDH);
        break;
    case 4:
        pt = (&RXF4SIDH);
        break;
    case 5:
        pt = (&RXF5SIDH);
        break;
    default:
        pt = (&RXF0SIDH);
        break;

    }


    *(pt + 0)  = (unsigned char)(sid >> 8);
    *(pt + 1)  = (unsigned char)(sid | 0x08);
    *(pt + 2)  = (unsigned char)(uppereid >> 8);
    *(pt + 3)  = (unsigned char)(uppereid);

}


// 0x007f => 0111 1111
void CAN_SetMask_SUB(char mask_no, unsigned int sid, unsigned long eid) // 1, 0x007f, 0x0
{
    unsigned int uppereid;


    sid     = (sid << 5) & 0xffe0;
    eid     = (eid & 0x0003ffff);
    uppereid = (unsigned int)(eid >> 16);
    sid     = (sid | uppereid);
    uppereid = (unsigned int)(eid);

    switch (mask_no)
    {
    case 0:
        RXM0SIDH = (unsigned char)(sid >> 8);
        RXM0SIDL = (unsigned char)(sid);
        RXM0EIDH = (unsigned char)(uppereid >> 8);
        RXM0EIDL = (unsigned char)(uppereid);
        break;
    case 1:
        RXM1SIDH = (unsigned char)(sid >> 8);
        RXM1SIDL = (unsigned char)(sid);
        RXM1EIDH = (unsigned char)(uppereid >> 8);
        RXM1EIDL = (unsigned char)(uppereid);
        break;
    default:
        RXM0SIDH = (unsigned char)(sid >> 8);
        RXM0SIDL = (unsigned char)(sid);
        RXM0EIDH = (unsigned char)(uppereid >> 8);
        RXM0EIDL = (unsigned char)(uppereid);
        break;
    }
}


void can_setTxSidEidReg(char MsgFlag, unsigned int sid, unsigned long eid) // MsgFlag=0(기본) , canTx_SidBuffer, canTx_EidBuffer
{
    unsigned int uppereid;

    sid     = (sid << 5) & 0xffe0;
    eid     = (eid & 0x0003ffff);
    uppereid = (unsigned int)(eid >> 16);
    sid     = (sid | uppereid);

    uppereid = (unsigned int)(eid);


    switch (MsgFlag)
    {
    case 0:
        TXB0SIDH  = (unsigned char)(sid >> 8);
        TXB0SIDL  = (unsigned char)(sid);
        TXB0SIDL  = (TXB0SIDL | 0x08);
        TXB0EIDH  = (unsigned char)(uppereid >> 8);
        TXB0EIDL  = (unsigned char)(uppereid);
        break;
    case 1:
        TXB1SIDH  = (unsigned char)(sid >> 8);
        TXB1SIDL  = (unsigned char)(sid);
        TXB1SIDL  = (TXB1SIDL | 0x08);
        TXB1EIDH  = (unsigned char)(uppereid >> 8);
        TXB1EIDL  = (unsigned char)(uppereid);
        break;
    case 2:
        TXB2SIDH  = (unsigned char)(sid >> 8);
        TXB2SIDL  = (unsigned char)(sid);
        TXB2SIDL  = (TXB2SIDL | 0x08);
        TXB2EIDH  = (unsigned char)(uppereid >> 8);
        TXB2EIDL  = (unsigned char)(uppereid);
        break;
    default:
        TXB0SIDH  = (unsigned char)(sid >> 8);
        TXB0SIDL  = (unsigned char)(sid);
        TXB0SIDL  = (TXB0SIDL | 0x08);
        TXB0EIDH  = (unsigned char)(uppereid >> 8);
        TXB0EIDL  = (unsigned char)(uppereid);
        break;
    }

}



void can_setTxDlcReg(char MsgFlag, unsigned char DataCnt)
{
    switch (MsgFlag)
    {
    case 0:
        TXB0DLC  = DataCnt;
        break;
    case 1:
        TXB1DLC  = DataCnt;
        break;
    case 2:
        TXB2DLC  = DataCnt;
        break;
    default:
        TXB0DLC  = DataCnt;
        break;
    }
}










void    CanLiveChk(void)
{
    unsigned char id, i;
    unsigned int  IdPt;

    TXERRCNT = 0;
    RXERRCNT = 0;

    for (id = 0;id < MAX_ELEV;id++)
    {
        if (CanLiveTimeAdr[id] > 120)
        {
            IdPt = (id * HOST_DATA_RECOD);

            for (i=0; i<HOST_DATA_RECOD; i++)  _can_rxBuffer[IdPt+i] = 0x0;
            CanLiveTimeAdr[id] = 0;
        }
    }

    if (NoCanInt > 100)
    {
        InitCAN();
        NoCanInt = 0;
    }
}


void setSidEidBuffer(void) {

    unsigned int  itmp1, itmp2;
    unsigned long ltmp1;


    itmp1 = (unsigned int)Company; // 회사 아이디 저장
    itmp2 = (unsigned int)GROUP_NUMBER; // 그룹 넘버 저장
    itmp2 = (itmp2 << 7);
    canTx_SidBuffer = (unsigned int)(itmp2 | itmp1);

    ltmp1 = 0;
    ltmp1 = (ltmp1 | (unsigned long)myAdr);
    ltmp1 = (ltmp1 << 14);
    ltmp1 = (ltmp1 | (unsigned long)CallMeAdr);
    canTx_EidBuffer = ltmp1;

}


bool can_bTxLastData;
bool can_isLastData(void) {
    return can_bTxLastData;
}

unsigned    char     setEidBuffer(void) {
    unsigned long last_data=0,dataseq=0;
    unsigned char dlc;

	dlc = (Can1TxDataTotalCnt-CanTxDataTxPointer);
	dataseq=(CanTxDataTxPointer / 8);
	dataseq=(dataseq << 8);

	if( dlc <= 8){
		last_data=0x1000;
        can_bTxLastData = 1;
	}
	else	dlc=8;

	canTx_EidBuffer=(canTx_EidBuffer | dataseq);
	canTx_EidBuffer=(canTx_EidBuffer | last_data);

    return dlc;
}

void    can_setTxSidEidBuf(void) {

    setSidEidBuffer();
    can_txDlc = setEidBuffer();

}


void    can_updateRxSidEidBuffer(void) {
    unsigned int  itmp1, itmp2;
    unsigned long ltmp1;
    unsigned char TmpRcvAdr;

    itmp1 = (RXB0SIDH << 8);
    itmp2 = RXB0SIDL;
    RxSidBuffer = (unsigned int)(itmp1 | itmp2);
    RxSidBuffer = (RxSidBuffer >> 5);

    ltmp1 = (unsigned long)(RXB0SIDL & 0x03);
    RxEidBuffer = (ltmp1 << 16);

    ltmp1 = (unsigned long)(RXB0EIDH);
    ltmp1 = (ltmp1 << 8);
    RxEidBuffer = (RxEidBuffer | ltmp1);

    ltmp1 = (unsigned long)(RXB0EIDL);
    RxEidBuffer = (RxEidBuffer | ltmp1);

    Can1RxDlc=(RXB0DLC & 0x0f);
}



void    CAN_SetFilter(void)
{
    unsigned char i;

    can_setTxSidEidBuf();

    for (i = 0;i <= 5;i++)
    {
        CAN_SetFilter_SUB(i, canTx_SidBuffer, 0x0);
    }
}


void    CAN_SetMask(void)       //new
{
    can_setTxSidEidBuf();

    if (MaskSetBit)
    {
        CAN_SetMask_SUB(0, 0x007f, 0x0);
        CAN_SetMask_SUB(1, 0x007f, 0x0);
    }
    else
    {
        CAN_SetMask_SUB(0, 0x0000, 0x0);
        CAN_SetMask_SUB(1, 0x0000, 0x0);
    }
}


void    can_set8DataTo(unsigned char *pt)
{
    unsigned char	i;

    for (i=0; i<8; i++)	*(pt + i) = can_8data[i];
}



void  CanTxDataLoad(unsigned char pt)
{
	TXB0D0=can_txBuffer[pt+0];
	TXB0D1=can_txBuffer[pt+1];
	TXB0D2=can_txBuffer[pt+2];
	TXB0D3=can_txBuffer[pt+3];
	TXB0D4=can_txBuffer[pt+4];
	TXB0D5=can_txBuffer[pt+5];
	TXB0D6=can_txBuffer[pt+6];
	TXB0D7=can_txBuffer[pt+7];
}


void    CanTx0(void) {
    unsigned int i;

    if (!TXB0REQ) {
        can_setTxSidEidBuf(); // Tx를 위한 SidEidBuffer 세팅
        can_setTxDlcReg(0, can_txDlc); // DLC 세팅
        can_setTxSidEidReg(0, canTx_SidBuffer, canTx_EidBuffer); // sid, eid 레지스터에 저장
        CanTxDataLoad(CanTxDataTxPointer); // data 레지스터에 저장

        IPR5 = 0; // IPR5: PERIPHERAL INTERRUPT PRIORITY REGISTER 5
        TXB0IP = 1; // TXB0IP: CAN Transmit Buffer 0 Interrupt Priority bit
        TXB0REQ = 1 ; // 전송 요구 비트 셋 : TX 시작 하기전 반드시 1로 셋 해줘야만 Tx를 하기 시작한다.

        CanTxDataTxPointer=(CanTxDataTxPointer + can_txDlc);
        if(can_isLastData()){
            can_bTxLastData = 0;
			Can1TxDataTotalCnt=0;
            bCanTxAct = FALSE;
		}
    }
}





void can_rxErrInterrupt(void) {
    if (RXB0OVFL)
    {
        RXB0OVFL = 0;
    }
    if (RXB1OVFL)
    {
        RXB1OVFL = 0;
    }
    if (TXBO)
    {
        TXBO = 0;
    }
    if (TXBP)
    {
        TXBP = 0;
    }
    if (RXBP)
    {
        RXBP = 0;
    }
    if (TXWARN)
    {
        TXWARN = 0;
    }
    if (RXWARN)
    {
        RXWARN = 0;
    }
    if (EWARN)
    {
        EWARN = 0;
    }
    COMSTAT = 0;

}


void   can_receiveData(void) {
	unsigned char	j,AckHost;
	unsigned int	hostnm;
	unsigned char	tmpbuf[10];
	unsigned char	tmpeid;
	unsigned char	RcvSrcAdr;
	unsigned char	RcvDectAdr;

	tmpeid=(unsigned char)(RxEidBuffer >> 8);
	CanRxDataRxPointer = ((tmpeid & 0x0f) * 8);

	for(j=0;j<Can1RxDlc;j++){
  		_can_rxBuffer[0 + CanRxDataRxPointer] = can_thisRxBuf[j];
		CanRxDataRxPointer++;
	}

	if(tmpeid & 0x10){ // rx end data
         can_bRxGood = 1;
	}
}


unsigned int	can_rxInterrupt(void) {
    unsigned int i;
    unsigned char *ptr;


    if (IRXIF) {
        IRXIF = 0;
    } else if (WAKIF) {
        WAKIF = 0;
    } else if (ERRIF) {
        ERRIF = 0;
        can_rxErrInterrupt();
    } else if (RXB1IF) {
        RXB1IF = 0;
    } else if (RXB0IF) {
        bCanTxAct = 0;
        NoCanInt = 0;

        can_updateRxSidEidBuffer();
        ptr = (unsigned char*) &RXB0D0;   // RXB0D0레지스터 주소를 구하여 ptr변수에 지정
        for(i=0;i<Can1RxDlc;i++)	can_thisRxBuf[i]=ptr[i];

        can_receiveData();

        RXB0IF = 0;
        RXB0FUL = 0;
    }
    return(0);
}





void InitCAN(void)
{
    unsigned int i;

    CAN_SetPort();
    CAN_SetBaudrate();

    ENDRHI = 1; // CANTX pin will drive when recessive
    CANCAP = 0; // enable CAN Capture contMode
    WAKFIL = 0; // CAN bus line filter is not used for wake-up

    /***********************************************/
    RXB0FUL     = 0;   // clear receive buffer0 full flag
    RXB0M1      = 1;   // Receive extended identifier valid messages
    RXB0M0      = 0;   // Receive extended identifier valid messages
    RXB0DBEN    = 0;   // no data write Receive buffer0 to receive buffer1
    RXB0FILHIT0 = 0;    // acceptance filter0
    RXB0RTRRO   = 0;   // No remote transfer request(read only)
    JTOFF       = 0;   // (read only)
    /***********************************************/

    /***********************************************/
    RXB1FUL     = 0 ;
    RXB1M1      = 1 ; // Receive extended identifier valid messages
    RXB1M0      = 0 ;
    RXB1FILHIT2 = 0 ; //filter2
    RXB1FILHIT1 = 1 ;
    RXB1FILHIT0 = 0 ;
    RXB1RTRRO   = 0 ; // No remote transfer request(read Only)
    /***********************************************/

    RXF0EXIDEN = 1;
    RXF1EXIDEN = 1;
    RXF2EXIDEN = 1;
    RXF3EXIDEN = 1;
    RXF4EXIDEN = 1;
    RXF5EXIDEN = 1;

	// 현재 sid 에만, 마스크 되어있다.
    CAN_SetFilter(); // 마스크 1로 셋된 비트에 대해서만 비교를 한다. 필터값이 0이면 0인 값들만 수용하고, 1이면 1인 값만 수용한다.
    CAN_SetMask(); // 마스크 값이 0이면 필터 무시, 1이면 필터값과 비교

    TXB0RTR  = 0;

    TXB0REQ = 0 ;
    TXB1REQ = 0 ;
    TXB2REQ = 0 ;


    RXB1IP = 0;


#if  defined(CPU65K80)   ||  defined(CPU45K80)
    IPR5 = 0;
#else
    IPR3 = 0;
#endif

    IRXIE       = 0;
    WAKIE       = 0;
    ERRIE       = 1;
    TXB2IE      = 0;
    TXB1IE      = 0;
    TXB0IE      = 0;

    RXB1IE      = 1;
    RXB0IE      = 1;





#ifdef		 U_COMPILER		1
    CANCONbits.REQOP = 0 ;// Entry Configration Mode
#else
    REQOP2 = 0 ;// Entry Configration Mode
    REQOP1 = 0 ;
    REQOP0 = 0 ;
#endif



    for (i = 0; i<MAX_SAVE_BUF; i++)    _can_rxBuffer[i] = 0;

}//--------------------------------------------------------------------------------------------------




