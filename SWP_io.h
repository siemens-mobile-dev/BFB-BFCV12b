#ifndef _WS_H_
#define _WS_H_

#define MAXSWPDATA 1024
// WinSwup i/o
typedef struct  // Swup Head
{
    DWORD addr; // Motorola (hi,mb,mb,lo)
    WORD  len;  // Motorola (hi,lo)
}sswphead;

typedef union
{
  struct  // SWP data
  {
    sswphead ;
    BYTE chk;
  };
  BYTE b[sizeof(sswphead)];
  //int size;//=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE)+sizeof(DWORD);
}sswpdata;

typedef union // SWP cmd
{
  struct
  {
    sswphead;
    BYTE cmd;
    BYTE chk;
    int size;//=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE);
  };
  BYTE b[sizeof(sswphead)+sizeof(BYTE)];
}sswpcmd;

typedef union // SWP cmd + byte
{
  struct
  {
    sswphead;
    BYTE cmd;
    BYTE bb;
    BYTE chk;
    int size;//=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE)+sizeof(DWORD);
  };
  BYTE b[sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE)];
}sswpcmdb;

typedef union // SWP cmd + word
{
  struct
  {
    sswphead;
    BYTE cmd;
    WORD w;
    BYTE chk;
    int size;//=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE)+sizeof(DWORD);
  };
  BYTE b[sizeof(sswphead)+sizeof(BYTE)+sizeof(WORD)];
}sswpcmdw;

typedef union // SWP cmd + dword
{
  struct
  {
    sswphead;
    BYTE cmd;
    DWORD dw;
    BYTE chk;
    int size;//=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE)+sizeof(DWORD);
  };
  BYTE b[sizeof(sswphead)+sizeof(BYTE)+sizeof(DWORD)];
}sswpcmdd;

typedef union
{
  struct
  { 
   BYTE  code[0x28];
   BYTE  id[16];
   DWORD type;
   DWORD par;
   BYTE  key[16];
  };
  BYTE  b[0x50];
}sboothead;

typedef union
{
  struct  // Swup Head
  {
    DWORD addr; // Motorola (hi,mb,mb,lo)
    WORD  len;  // Motorola (hi,lo)
    union
    {
      BYTE cmd;
      BYTE data[MAXSWPDATA];
    };
    int size;
    BYTE chk;
  };
  BYTE b[sizeof(DWORD)+sizeof(WORD)+MAXSWPDATA+sizeof(int)+sizeof(BYTE)];
}sswpreadbuf;


//extern swswup swup;
//extern swswup swup;
extern sswpreadbuf swprd;

int SWP_WriteBoot(WORD len, BYTE *data);
int SWP_WriteMem(DWORD addr, WORD len, BYTE *data);
int SWP_cmd(BYTE cmd);
int SWP_cmd_b(BYTE cmd, BYTE b);
int SWP_cmd_w(BYTE cmd, WORD w);
int SWP_cmd_dw(BYTE cmd, DWORD dw);
int SWP_TstReadCmd(void);
int SWP_ReadCmd(void);


#endif _WS_H_

