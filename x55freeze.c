#include <windows.h>
#include <stdio.h>

#include "RS_io.h"
#include "BFB_io.h"



unsigned char srv55mode[]= { 0x068,
        0x0A5,0x05A,0x0A5,0x0A5,0x0E6,0x089,0x046,0x014,
        0x0D1,0x090,0x0FF,0x09A,0x0FE,0x09B,0x0E6,0x00C,
        0x009,0x000,0x0E6,0x08A,0x0BF,0x004,0x0D1,0x090,
        0x0E6,0x00A,0x0F0,0x00E,0x0E6,0x08A,0x0AF,0x004,
        0x0B5,0x04A,0x0B5,0x0B5,0x0E6,0x000,0x040,0x000,
        0x0E7,0x0F0,0x000,0x000,0x0F7,0x0F0,0x000,0x002,
        0x0E7,0x0F0,0x007,0x000,0x0F7,0x0F0,0x001,0x002,
        0x0E6,0x003,0x003,0x000,0x0E7,0x0F0,0x000,0x000,
        0x0F7,0x0F0,0x000,0x0F6,0x0E7,0x0F0,0x007,0x000,
        0x0F7,0x0F0,0x001,0x0F6,0x0E7,0x0F0,0x080,0x000,
        0x0F7,0x0F0,0x002,0x0F6,0x0E7,0x0F0,0x001,0x000,
        0x0F7,0x0F0,0x003,0x0F6,0x0E6,0x0F0,0x080,0x000,
        0x074,0x0F0,0x0E0,0x0EF,0x0B7,0x048,0x0B7,0x0B7,
        0x0FE };
/*
A5 5A A5 A5-E6 89 46 14-
D1 90 FF 9A-FE 9B E6 0C
09 00 E6 8A-BF 04 D1 90-
E6 0A F0 0E-E6 8A AF 04
B5 4A B5 B5-E6 00 40 00-
E7 F0 00 00-F7 F0 00 02
E7 F0 07 00-F7 F0 01 02-
E6 03 03 00-E7 F0 00 00
F7 F0 00 F6-E7 F0 07 00-
F7 F0 01 F6-E7 F0 80 00
F7 F0 02 F6-E7 F0 01 00-
F7 F0 03 F6-E6 F0 80 00
74 F0 E0 EF-B7 48 B7 B7-
*/
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


BYTE imeibuf[20];

//FILE *in=0;//,*out = stdout;
char * enFreeze[] = { "Done","NotSupported","NoServiceMode","AccessDenied",
"IMEINotValid","IMEIFreezeFailed","BootFreezeFailed","NoFreezeData" };



/*  Main program  */

int main(int argc, char **argv)
{
  int i,x;
  BYTE b;
ComNum = 2;
   printf("Freeze for Siemens x55 GSM telephones. v1.0\n (c) All people of the Earth and PV`\n\n");
   if (argc<2)
   {
      printf("Uses %s [com number]\n\n",argv[0]);
   }
   if (argc>1) ComNum = atoi(argv[1]);

    ct.ReadTotalTimeoutConstant=100;
    if(!InitCom(ComNum,57600)) return 11;
    while(ReadChar()>=0);
    printf("Press red button or power to cable");
    for(i=1;i<33;i++)
    {
        SendChar(0x55);
        while((x=ReadChar())>=0) if(x==0xA0) break;
        if(x==0xA0) { i=0; break; };
        printf(".");
    }
    ct.ReadTotalTimeoutMultiplier=20;
    ct.ReadTotalTimeoutConstant=50;
    SetCommTimeouts(hCom,&ct);

    if(i==0)
    {
      printf("\nLoad Service Boot...\n");
      if(SendBlk(srv55mode,sizeof(srv55mode))<0) { printf("\nError Write boot!\n\a"); return 12;};
      i=ReadChar();
      if(i==0xA5) printf("\nBoot Ok!");
      Sleep(1000);
      while(ReadChar()>=0);
    };
    printf("\n\n");

  if(PingBFB()<0) { printf("Error Ping BFB mode!\n\a"); return 13;};


  InfoOldBFB();
  printf("\nFreeze(%s)...\n",&imeibuf[1]);

/* PV`[] Короче если имеем стандартный 52 EEP и меняем первый байт
- то фризе проходит глубоко по всем калам в теле и вылетает на
IMEIFreezeFailed  - не может прописать IMEI в OTP похоже,
это не совсем точно, но точно в flash пишет IMEI и там уже вылетает.
Даже если проскочит - вроде более ничего интересного :( */

  // calk chk
  b=0x52;
  imeibuf[0]=b;
  for(i=1;i<16;i++) b^=imeibuf[i];
  imeibuf[16]=b;
  i=16;
  if(SendBFB(0x14,imeibuf,17)==0)
  {
      if(ReadBFB()==0)
      {
        printf("Freeze result = %i -> ",*(WORD*)(ibfb.data_b));
        if (*(WORD*)(ibfb.data_b)<8) printf("%s\n",enFreeze[*(WORD*)(ibfb.data_b)]);
        else printf("Unknown Error\a\n");
        i=0;//*(WORD*)(ibfb.data_b);
      }else
      {
        printf("\nError Read result Freeze!\n\a");
        i=15;
      };

  }else
  {
      printf("\nError Send Freeze!\n\a");
      i=14;
  };
  CmdBFB(0x0E,0x04);
  printf("\nMobileOff...\n\n");
  return i;
}
