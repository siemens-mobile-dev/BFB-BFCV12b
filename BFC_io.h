#ifndef _BFC_H_
#define _BFC_H_

#define MAXBFCDATA 1024
// BFC
typedef struct // BFC Head
{
    BYTE  idtx; // for all bfc input equ 0x01 ?
    BYTE  idrx; // for all bfc output equ 0x01 ?
    WORD  len;  // Motorola (hi,lo)
    BYTE  type; // 0,4,0x20,?? 0x20 -> CRC16
    BYTE  chk;  // = idtx^idrx^len^(len>>8)^type;
}sbfchead;

typedef struct // BFC info
{
    sbfchead ;
    BYTE data[MAXBFCDATA+2]; // +2 reserved CRC16
}sibfc;

typedef union // BFC block
{
  sibfc;
  struct
  {
    unsigned char b[sizeof(sibfc)];
    int size;
  };
}sbfc;

extern sbfc obfc;
extern sbfc ibfc;
extern BYTE hostid;
extern char strbfcdeb[];

int InitBFCout(unsigned char bfcnum, unsigned char bfctype, unsigned char *buf, int lenbuf);
int SendBFC_GetInfo(WORD data); // BFC 11
int InitHostBFC(BYTE hid);
int SendBFC_AT(char *buf);
int ReadBfc(void);
void PrintBFC(sbfc pbfc);
int SetBFB_x65(void);
int BFCphonemodel(void);
int BFC_01(DWORD i);
int SendBFC_Cmd(unsigned char bfcnum, unsigned char bfctype, unsigned char *buf, int lenbuf);
int BFCReadMem(DWORD addr, DWORD len);
int SimSimBFC(void);
int BFC_GetHardwareIdentification(void);
int TestOpenBFC(void); // =01 -> FactoryMode (OpenBFC) , 00 -> Standard CloseBFC 
int SetSpeedBFC(int baud); //  1200,2400,4800,9600,19200,38400,57600,115200,200000,230400,400000,460800
int ClearBFC(int baud);
int BFC_TestServiceMode(void); // 0x12=NormalMode, 0x16=ServiceMode, 0x00=BurninMode ?
int TstOpenBFC(void);
#define ATtoBFC() SendBFC_AT("AT^SQWE=1\r")
#define EndBFC() SendBFC_AT("AT^SQWE=0\r")
#define GipsyBFC() SendBFC_AT("AT^SQWE=2\r")

#endif _BFC_H_