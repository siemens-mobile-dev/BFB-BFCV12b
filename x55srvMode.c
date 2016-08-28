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


//BYTE keybuf[64],imeibuf[32];

//FILE *in=0;//,*out = stdout;

int srvbaud = 0;

/*  Main program  */

int main(int argc, char **argv)
{
  int i,x;
ComNum=2;
   printf("Load in 'Service mode' for Siemens x55(C166) GSM telephones. v1.0\n (c) All people of the Earth and PV`\n\n");
   if (argc<2)
   {
      printf("Uses %s [[com number] BFB Speed]\n\n",argv[0]);
   }
   if (argc>1) ComNum = atoi(argv[1]);
   if (argc>2) sscanf(argv[2],"%i",&srvbaud);

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

      if((i=SendBlk(srv55mode,sizeof(srv55mode)))<0) { printf("\nError Write boot!\n\a"); return 12;};
      i=ReadChar();
      if(i==0xA5) printf("\nBoot Ok!");
      Sleep(1000);
      while(ReadChar()>=0);
    };
    printf("\n\n");

  if(PingBFB()<0) { printf("Error Ping BFB mode!\n\a"); return 13;};

  InfoOldBFB();

  if(srvbaud!=0)
  {
   printf("\nSet BFB Speed %i baud",srvbaud);
   if(SetSpeedBFB(srvbaud)!=0)
   {
     printf(" - failed!\n\n");
     return 14;
   }
   else
   {
     printf(" - Ok!\n\n");
     return 0;
   };
  }
  else printf("\nBFB Speed 57600 baud.\n\n");
  return 0;
}
