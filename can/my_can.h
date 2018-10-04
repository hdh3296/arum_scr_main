


#ifndef	__my_can_h__
#define	__my_can_h__

#include "..\main.h"

#define     GROUP_NUMBER        0

#define     HOST_BOARD          0x2000
#define     HOST_ADDRESS        0x0000c000

#define     NEW_HOST_ADDRESS    0x0003c000


#define     MAX_ELEV            4
#define     CAN_RCV_DATA        0


#define     HOST_DATA_RECOD     42
#define     HOST_LAST_DATA      0x08


#define     MAX_SAVE_BUF        100




bit    				MaskSetBit = 0;
unsigned    char    CallMeAdr = 0;

bool can_bRxGood;



extern  void    can_setTxDlcReg(char MsgFlag,unsigned char DataCnt);
extern  void    InitCAN(void);
extern  void    CanCheck(void);
extern  unsigned int    can_rxInterrupt(void);
extern  void    CanTx0(void);
extern  void    CanTx1(void);
extern  void    CanLiveChk(void);
extern  void    CAN_SetFilter_SUB(char filter_no, unsigned int sid, unsigned long eid);
extern  void    CAN_SetMask_SUB(char mask_no, unsigned int sid, unsigned long eid);
extern  void    can_setTxSidEidBuf(void);



extern	unsigned    char    NoCanInt;
extern  unsigned    int     CanTime;
extern  unsigned    char    _can_rxBuffer[MAX_SAVE_BUF];

extern  unsigned    char    bCanTxAct;
extern  unsigned    char    can_8data[8];
extern  unsigned    char    CanCmd;
extern  unsigned    char    Company;
extern  unsigned    char    myAdr;
extern  unsigned    char    CAN_ReceivedAdr;
extern  unsigned    char    CanLiveTimeAdr[MAX_ELEV];

extern  unsigned    long    canTx_EidBuffer;
extern  unsigned    int     canTx_SidBuffer;
extern  unsigned    long    RxEidBuffer;
extern  unsigned    int     RxSidBuffer;




#endif
