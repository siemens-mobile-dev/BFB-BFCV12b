#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "BFB_io.h"
#include "xcrc16.h"
#include "RS_io.h"
#include "Tels.h"

//BYTE oldbfbcmd=0;
//BYTE bfb_data[MAXBFBDATA+8];
//BYTE len_data;

sbfb ibfb;
sbfb obfb;

/*
460000=0x00704E0
406000=0x00631F0
400000=0x0061A80
230000=0x0038270
203000=0x00318F8
200000=0x0030D40
115200=0x001C200
 57600=0x000E100
*/

BYTE bfbspd4800[]   = { 0x34,0x38,0x30,0x30,0x3F,0x87,0xCF };
BYTE bfbspd9600[]   = { 0x39,0x36,0x30,0x30,0x3F,0x49,0xCF };
BYTE bfbspd14400[]  = { 0x31,0x34,0x34,0x30,0x30,0xCE,0x8B,0xCF };
BYTE bfbspd19200[]  = { 0x31,0x39,0x32,0x30,0x30,0xCE,0x4D,0xCF };
BYTE bfbspd23040[]  = { 0x32,0x33,0x30,0x34,0x30,0xCD,0xCF,0x8F };
BYTE bfbspd28800[]  = { 0x32,0x38,0x38,0x30,0x30,0xCD,0x47,0xCF };
BYTE bfbspd38400[]  = { 0x33,0x38,0x34,0x30,0x30,0xCC,0x4B,0xCF };
BYTE bfbspd57600[]  = { 0x35,0x37,0x36,0x30,0x30,0xCA,0x89,0xCF };
BYTE bfbspd100000[] = { 0x31,0x30,0x30,0x30,0x30,0x30,0x0C,0x90,0x2B };
BYTE bfbspd115200[] = { 0x31,0x31,0x35,0x32,0x30,0x30,0x0D,0xD2,0x2B };
BYTE bfbspd200000[] = { 0x32,0x30,0x30,0x30,0x30,0x30,0x0C,0x90,0x2B };
BYTE bfbspd203000[] = { 0x32,0x30,0x33,0x30,0x30,0x30,0x0C,0x90,0x2B };
BYTE bfbspd230000[] = { 0x32,0x33,0x30,0x30,0x30,0x30,0x0F,0x90,0x2B };
BYTE bfbspd400000[] = { 0x34,0x30,0x30,0x30,0x30,0x30,0x4C,0xD0,0x2B };
BYTE bfbspd406000[] = { 0x34,0x30,0x36,0x30,0x30,0x30,0x4C,0xD0,0x2B };
BYTE bfbspd460000[] = { 0x34,0x36,0x30,0x30,0x30,0x30,0x4A,0x90,0x2B };

struct
{
    int spd;
    int len;
    BYTE *str;
} bfbcomspd[] = {{ 4800,  sizeof(bfbspd4800)  ,bfbspd4800 },
                 { 9600,  sizeof(bfbspd9600)  ,bfbspd9600 },
                 { 14400, sizeof(bfbspd14400) ,bfbspd14400 },
                 { 19200, sizeof(bfbspd19200) ,bfbspd19200 },
                 { 23040, sizeof(bfbspd23040) ,bfbspd23040 },
                 { 38400, sizeof(bfbspd38400) ,bfbspd38400 },
                 { 57600, sizeof(bfbspd57600) ,bfbspd57600 },
                 { 115200,sizeof(bfbspd115200),bfbspd115200 },
                 { 200000,sizeof(bfbspd200000),bfbspd200000 },
                 { 203000,sizeof(bfbspd203000),bfbspd203000 },
                 { 230000,sizeof(bfbspd230000),bfbspd230000 },
                 { 400000,sizeof(bfbspd400000),bfbspd400000 },
                 { 406000,sizeof(bfbspd406000),bfbspd406000 },
                 { 460000,sizeof(bfbspd460000),bfbspd460000 },
                 { 0,0, NULL }};

int SendBFB(BYTE id, BYTE *buf, BYTE len)
{
   if(len>MAXBFBDATA) return -1;
   obfb.id=id;
   obfb.len=len;
   obfb.chk=obfb.len^obfb.id;
   obfb.size=obfb.len+sizeof(sbfbhead);
   if(len!=0) memcpy(obfb.data,buf,len);
   if(SendBlk(obfb.b,obfb.size)) return -1;
   return 0;
}

int ReadBFB(void)
{
 int c,i;
 DWORD BuffLen;

  ibfb.size=0;
  ibfb.len=0;
  ReadFile(hCom,ibfb.b,sizeof(sbfbhead),&BuffLen,NULL);
  if(BuffLen!=sizeof(sbfbhead)) return -1;
  do
  {
    if((ibfb.id==obfb.id)&&(ibfb.len<=MAXBFBDATA)&&(ibfb.chk==(ibfb.id^ibfb.len)))
    {
      if(ibfb.len!=0)
      {
        ReadFile(hCom,ibfb.data,(int)ibfb.len,&BuffLen,NULL);
        if(debprt&3)
        {
          printf(">BFB@%02X-%02X data:",ibfb.id,ibfb.cmdb);
          for(i=1;i<(int)ibfb.len;i++) printf(" %02X",ibfb.data[i]);
          printf("\n");
        };
        if(BuffLen==(DWORD)ibfb.len) return 0;
        if(BuffLen<(DWORD)ibfb.len) return -2;
      }else return 0;
    }
    for(i=1;i<sizeof(sbfbhead);i++) ibfb.b[i-1]=ibfb.b[i];
    c = ReadChar();
    if(c<0) return -1;
    ibfb.b[i] = (BYTE)c;
  }while(1);
}

int BFBoi(void)
{
 int i;
    if((i=SendBlk(obfb.b,obfb.size))!=0) return i;
    if((i=ReadBFB())!=0) return i;
    if(ibfb.len==0) return 2;
    if(ibfb.cmdb!=obfb.cmdb) return 1;
    return 0;
}

int CmdBFB(BYTE id, DWORD x)
{
   obfb.id=id;
   if(x>0xFFFFFF) obfb.len=4;
   else if(x>0xFFFF) obfb.len=3;
   else if(x>0xFF) obfb.len=2;
   else if(x>0) obfb.len=1;
   else obfb.len=0;
   obfb.cmdd=x;
   obfb.chk=obfb.len^obfb.id;
   obfb.size=obfb.len+sizeof(sbfbhead);
   return BFBoi();
}

int PingBFB(void)
{
int i;
  if((i=CmdBFB(0x02,0x14))!=0) return i; // Ping BFB
  if(ibfb.len!=2) return -2;
  if(ibfb.cmdb!=obfb.cmdb) return -3;
  if(ibfb.data_b[0]!=0xAA) return 1;
  return 0;
}

int SimSimBFB(void)
{
int i;
  if((i=CmdBFB(0x05,0x39))!=0) return i; // SimSIM BFB 05 01 04 39
  if(ibfb.len!=0x01) return -2;
  return 0;
}


int BFBReadMem(DWORD addr, WORD len)
{
 int i;
   obfb.id=0x02;
   obfb.len=sizeof(BYTE)+sizeof(WORD)+sizeof(DWORD);
   obfb.chk=obfb.len^obfb.id;
   obfb.cmdb=0x02;
   *(DWORD*)(obfb.data_b)=addr;
   *(WORD*)(&obfb.data_b[sizeof(DWORD)])=len;
   obfb.size=obfb.len+sizeof(sbfbhead);
   if((i=BFBoi())!=0) return i;
   if(ibfb.len!=len+1) return -2;
   return 0;
}

int BFBWriteMem(DWORD addr, BYTE len, BYTE * buf)
{
 int i;
   if(len>MAXBFBDATA-(sizeof(BYTE)+sizeof(WORD)+sizeof(DWORD))) return -13;
   obfb.id=0x02;
   obfb.len=len+sizeof(BYTE)+sizeof(WORD)+sizeof(DWORD);
   obfb.chk=obfb.len^obfb.id;
   obfb.cmdb=0x01;
   *(DWORD*)(obfb.data_b)=addr;
   *(WORD*)(&obfb.data_b[sizeof(DWORD)])=(WORD)len;
   if(len!=0) memcpy(&obfb.data_b[sizeof(WORD)+sizeof(DWORD)],buf,len);
   obfb.size=obfb.len+sizeof(sbfbhead);
   if((i=BFBoi())!=0) return i;
   if(ibfb.len!=len+1) return -2;
   return 0;
}


int SendBFB_AT(char *buf)
{
  int i;
  for(i=0;i<=MAXBFBDATA+2;i++) if(buf[i]==0) break;
  if(i>=MAXBFBDATA) return -1;
  if((i=SendBFB(0x06,buf,(BYTE)i))!=0) return i;
  if((i=ReadBFB())!=0) return i;
  if(ibfb.len!=0) return 2;
  if(ibfb.cmdb!=obfb.cmdb) return 1;
  return 0;
}

int EndBFB(void)
{
  int i;
//    return SendBFB_AT("AT^SBFB=2\r"); // Mode Gipsy Old Phone
   if(phonemodel>=65)
   {
     if((i=SendBFB_to_BFC())<0) return i;
     if(!InitCom(ComNum,115200)) return -1;
     return i;
   }
   else return SendBFB_AT("AT^SQWE=2\r"); // Mode Gipsy
}

int BFBphonemodel(void)
{
int i;
  if(CmdBFB(0x0E,0x07)==0) // BFB Get PhoneName (not work in new phone!)
  {
    if(ibfb.data[0]==0x07)
    {
      for(i=0;i<ibfb.len;i++)
      {
        if((ibfb.data[i]>='0')&&(ibfb.data[i]<='9')&&(ibfb.data[i+1]>='0')&&(ibfb.data[i+1]<='9'))
        {
          ibfb.data[i+2]=0;
          i=atoi(&ibfb.data[i]);
          return i;
        };
      };
    };
  }
  return 0;
}

int SetSpeedBFB(int baud)
{
 int save_baud=dcb.BaudRate;
 int i,x;
   for(i=0; bfbcomspd[i].spd != 0;i++) if(bfbcomspd[i].spd == baud) break;
   if(bfbcomspd[i].spd == 0) return -13;
   if(bfbcomspd[i].spd>maxbaud) return -1;
   if(!InitCom(ComNum,baud))
   {
     if(!InitCom(ComNum,save_baud)) return -13;
     else return -1;
   }
   else if(!InitCom(ComNum,save_baud)) return -13;
   if((x=CmdBFB(0x01,0xA1))<0) return x;
   if((ibfb.len==0)&&(ibfb.cmdb!=0xA3)) return -1;
   obfb.id=0x01;
   obfb.cmdb=0xC0;
   obfb.len=(BYTE)(bfbcomspd[i].len+1);
   obfb.chk=obfb.len^obfb.id;
   obfb.size=obfb.len+sizeof(sbfbhead);
   memcpy(obfb.data_b,bfbcomspd[i].str,bfbcomspd[i].len);
   if((i=BFBoi())<0) return i;
   if((ibfb.len==0)&&(ibfb.cmdb!=0xCC)) return -1;
   if(!InitCom(ComNum,baud)) return -1;
   if((i=PingBFB())!=0) return i;
   return 0;
}

/*
  BYTE chk,b=1;
   if((id==0x14)&&(obfb.len!=0))
   {
     obfb.len++;
     chk = obfb.data[0];
     while(b<obfb.len)
     {
       chk^=obfb.data[b];
       b++;
     }
     obfb.data[b]=chk;
   }
*/

int InfoOldBFB(void)
{
int i,SMode;
BYTE b;

  if(CmdBFB(0x0E,0x07)==0) printf("Model: %s",ibfb.data_b);
  if(CmdBFB(0x0E,0x09)==0) printf(" %s",ibfb.data_b);
  if(CmdBFB(0x0E,0x03)==0) printf(" Sw%02X",ibfb.data_b[0]);
  if(CmdBFB(0x0E,0x08)==0)
  {
    b=ibfb.data_b[8];
    ibfb.data_b[8]=0;
    printf(" %s",ibfb.data_b);
    ibfb.data_b[8]=b;
    ibfb.data_b[16]=0;
    printf(" %s",&ibfb.data_b[8]);
  }
  if((CmdBFB(0x0E,0x0A)==0)&&(ibfb.len==0x12)) printf("\nIMEI: %s\n",&ibfb.data_b[1]);
  else
  {
    printf("\nError Read IMEI!\n\a");
//    CmdBFB(0x0E,0x04);
//    printf("MobileOff...\n\n");
    return -3;
  };

  if(CmdBFB(0x0E,0x000C)==0)
  {
    SMode=ibfb.data_b[0];
//    printf("SecurityMode(%i):\n",ibfb.data_b[0]);
    if(SMode==0) printf("Warning: Rd/Wr Security EEP blocks enable, Open BFB disable!");
//    else if(ibfb.data_b[0]==1) printf("Error!");
    else if(SMode==2) printf("FactorySecurityMode: Rd/Wr Security EEP blocks enable, Open BFB enable.");
    else if(SMode==3) printf("Warning: Rd/Wr Security EEP blocks disable, Open BFB disable!");
    else printf("Warning: Unknown(%i) SecurityMode!",SMode);
  }

  if(CmdBFB(0x0E,0x05)==0) printf("\nFlagStatus: %02X\n",ibfb.data_b[0]);

  if((CmdBFB(0x0E,0x080B)==0)&&(ibfb.cmdw == 0x080B))
  {
    printf("SecurityStatus:");
    for(i=0;i<8;i++) printf(" %02X",(WORD)ibfb.data_w[i<<1]);
    printf("\n");
  }
  return SMode;
}

int SendBFB_to_BFC(void)
{
int i=0x04;
  return SendBFB(0x01, (BYTE *)&i, 1);  
}
