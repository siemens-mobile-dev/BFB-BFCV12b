#ifndef _BFB_H_
#define _BFB_H_

#define MAXBFBDATA 32

// Warning: Struct Align = 1 byte !!! When send imei write what model of phone
//
// ProjectSetting->C/C++->CodeGeneration->Struct memeber alignment = 1 byte
//

// BFB
typedef struct // BFB Head
{
    BYTE  id;   // id
    BYTE  len;  // size data
    BYTE  chk;  // = id^len;
}sbfbhead;

typedef struct // BFB info
{
    sbfbhead ;
    union
    {
      BYTE data[MAXBFBDATA+2];  // +2 reserved CRC16
      struct
      {
        BYTE cmdb;                  // out cmd
        BYTE data_b[MAXBFBDATA-1];  // i/o data [max=31]
      };
      struct
      {
        WORD cmdw;                  // out cmd
        BYTE data_w[MAXBFBDATA-2];  // i/o data [max=32]
      };
      struct
      {
        DWORD cmdd;                  // out cmd
        BYTE data_d[MAXBFBDATA-4];   // i/o data [max=32]
      };
    };
}sibfb;

typedef union // BFB i/o block
{
  sibfb;
  struct
  {
    unsigned char b[sizeof(sibfb)];
    int size;
  };
}sbfb;


extern sbfb ibfb;
extern sbfb obfb;

int PingBFB(void);
int BFBReadMem(DWORD addr, WORD len);
int BFBWriteMem(DWORD addr, BYTE len, BYTE * buf);
int SendBFB_AT(char *buf);
int EndBFB(void);
int BFBphonemodel(void);

int CmdBFB(BYTE id, DWORD x);
int SendBFB(BYTE id, BYTE *buf, BYTE len);
int ReadBFB(void);
int SetSpeedBFB(int baud);
int SimSimBFB(void);
int SendBFB_to_BFC(void);
InfoOldBFB(void);

#endif _BFB_H_