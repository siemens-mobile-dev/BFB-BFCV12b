#include <windows.h>
#include <stdio.h>

#include "RS_io.h"
#include "BFC_io.h"
#include "SWP_io.h"

struct
{
  sboothead head;
  BYTE srvdat[7];
} boot1 = {
0xF1,0x04,0xA0,0xE3,0x20,0x10,0x90,0xE5,0xFF,0x10,0xC1,0xE3,0xA5,0x10,0x81,0xE3,
0x20,0x10,0x80,0xE5,0x1E,0xFF,0x2F,0xE1,0x04,0x01,0x08,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x53,0x49,0x45,0x4D,0x45,0x4E,0x53,0x5F,0x42,0x4F,0x4F,0x54,0x43,0x4F,0x44,0x45,
0x70001,
115200,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x01,0x04,0x05,0x00,0x8B,0x00,0x8B };

union
{
 struct
 {
   DWORD fsn;
   DWORD key;
   BYTE xor[8];
 };
 BYTE b[16];
} keybin;


BYTE keybuf[64];

FILE *in=0;//,*out = stdout;


/*  Main program  */

int main(int argc, char **argv)
{
  int i,x;
   printf("Loader in 'Service mode' for Siemens x65+(ARM) GSM telephones. v1.0\n (c) All people of the Earth and PV`\n\n");
   if (argc<2)
   {
      printf("Uses %s [com number]\n\n",argv[0]);
   }
   if (argc>1) ComNum = atoi(argv[1]);

    ct.ReadTotalTimeoutConstant=100;
    if(!InitCom(ComNum,115200)) return 11;
    while(ReadChar()>=0);
    printf("Press red button or power to cable");
    for(i=1;i<33;i++)
    {
        SendChar(0x41);
        SendChar(0x54);
        while((x=ReadChar())>=0) if(x==0xB0) break;
        if(x==0xB0) { i=0; break; };
        printf(".");
    }
    ct.ReadTotalTimeoutMultiplier=20;
    ct.ReadTotalTimeoutConstant=50;
    SetCommTimeouts(hCom,&ct);

    if(i==0)
    {
      printf("\nLoad Service Boot...\n");
      if(SWP_WriteBoot(sizeof(boot1),boot1.head.b )<0) { printf("\nError Write boot!\n\a"); return 12;};
      i=ReadChar();
      if(i==0x06) printf("\nBoot Ok!");
      Sleep(1000);
      while(ReadChar()>=0);
    };
    printf("\n");
    hostid=0x01;
    if(InitHostBFC(0x11)<0) if(InitHostBFC(0x11)<0) if(InitHostBFC(0x11)<0) { printf("Error Init BFC mode!\n\a"); return 13;};
    i=BFC_TestServiceMode();
    if(i==0x12)
    {
      printf("Telephone in 'Normal Mode', 115200 baud.\n\n\a");
      return 1;
    }
    else if(i==0x16)
    {
      printf("Telephone in 'Service Mode', 115200 baud.\n\n");
      return 0;
    }
    else
    {
      printf("Telephone in 'Unknown Mode'!\n\n\a");
      return 2;
    }
    return 14;
}
