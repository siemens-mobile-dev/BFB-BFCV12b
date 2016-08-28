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


BYTE keybuf[64],imeibuf[32];

FILE *in=0;//,*out = stdout;


/*  Main program  */

int main(int argc, char **argv)
{
  int i,x;
ComNum=2;
   printf("Open All Read/Write blocks EEPROM for Siemens x55 GSM telephones. v1.0\n (c) All people of the Earth and PV`\n\n");
   if (argc<2)
   {
      printf("Uses %s [com number]\n\n",argv[0]);
   }
   if (argc>1) ComNum = atoi(argv[1]);

   if ((in = fopen("key.bin", "rb")) == NULL)
   {
     printf("Cannot open input key.bin file!\n");
     return 12;
   }
   if((i = fread(keybin.b, 1, 8 , in)) != 8)
   {
     printf("Cannot read key.bin file!\n");
     return 13;
   }
   fclose(in);
   printf("KEY:\n'%08u'\n",keybin.key);
    ct.ReadTotalTimeoutConstant=100;
    if(!InitCom(ComNum,57600)) return 14;
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

      if((i=SendBlk(srv55mode,sizeof(srv55mode)))<0) { printf("\nError Write boot!\n\a"); return 15;};
      i=ReadChar();
      if(i==0xA5) printf("\nBoot Ok!");
      Sleep(1000);
      while(ReadChar()>=0);
    };
    printf("\n\n");

    if(PingBFB()<0) { printf("Error Ping BFB mode!\n\a"); return 16;};

    InfoOldBFB();

    i=sprintf(keybuf,"X%08u",keybin.key);

    if(SendBFB(0x0b,keybuf,(BYTE)i)<0) { printf("Error Send key!\n\a"); return 17;};
    if((ReadBFB()==0)&&(ibfb.cmdb==0x57))
    {
      printf("Key Send Ok!");
      if(ibfb.data_b[0]==0x01)
      {
        printf(" - Please Check up!\n\n");
        SetSpeedBFB(115200);
        return 0;
      }
      else
      {
        if(ibfb.data_b[0]==33)
        {
          printf(" - Key Error or BAG Mobile!\n\a");
        }
        else
        {
          printf(" - TimeOut %u000 msec!\n\n",ibfb.data[1]);
        }
        Sleep(1000);
//        SetSpeedBFB(115200);
        return 1;
      };
    }
    else
    {
      printf("Old x55 Phone?\n");
        Sleep(1000);
//      SetSpeedBFB(115200);
      return 18;
    }
    printf("Incorrect Key or Key Close?\n");

  return 19;
}
