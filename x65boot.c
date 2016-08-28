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
   printf("Open All Read/Write blocks EEPROM for Siemens x65 GSM telephones.\n (c) All people of the Earth and PV`\n\n");
   if (argc<2)
   {
      printf("Uses %s [com number]\n\n",argv[0]);
   }
   if (argc>1) ComNum = atoi(argv[1]);

   if ((in = fopen("key.bin", "rb")) == NULL)
   {
     printf("Cannot open input key.bin file!\n");
     return -2;
   }
   if((i = fread(keybin.b, 1, 8 , in)) != 8)
   {
     printf("Cannot read key.bin file!\n");
     return -3;
   }
   fclose(in);
   printf("KEY:\n'%08u'\n",keybin.key);
//    ct.ReadIntervalTimeout=0;
//    ct.ReadTotalTimeoutMultiplier=1;
    ct.ReadTotalTimeoutConstant=100;
//    ct.WriteTotalTimeoutMultiplier=MAXDWORD;
//    ct.WriteTotalTimeoutConstant=MAXDWORD;
    if(!InitCom(ComNum,115200)) return -1;
//    SetCommTimeouts(hCom,&ct);
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
//    if(i!=0) { printf("\nError Answer phone!\n\a"); return -1;};
    //ct.ReadIntervalTimeout=0;
    ct.ReadTotalTimeoutMultiplier=20;
    ct.ReadTotalTimeoutConstant=50;
//    ct.WriteTotalTimeoutMultiplier=MAXDWORD;
    //ct.WriteTotalTimeoutConstant=MAXDWORD;
    SetCommTimeouts(hCom,&ct);

    if(i==0)
    {
      printf("\nLoad Service Boot...\n");
      if((i=SWP_WriteBoot(sizeof(boot1),boot1.head.b ))<0) { printf("\nError Write boot!\n\a"); return i;};
      i=ReadChar();
      if(i==0x06) printf("\nBoot Ok!");
      Sleep(1000);
      while(ReadChar()>=0);
    };
    printf("\n");
    hostid=0x01;
    if(InitHostBFC(0x11)<0) { printf("Error1 Init BFC mode!\n\a"); return -1;};
    i=sprintf(keybuf,"\x11X%08u",keybin.key);
    if(InitHostBFC(0x25)<0) { printf("Error2 Init BFC mode!\n\a"); return -1;};
//    return 0;
    if((SendBFC_Cmd(0x25,0x20,keybuf,i+1)==0)&&(ibfc.data[0]==0x11))
    {
      printf("Key Send Ok!");
      if(ibfc.data[1]==0x01)
      {
        printf(" - Please Check up!\n\n");
        return 0;
      }
      else
      {
        printf(" - Attempt number %u!\n\n",ibfc.data[1]);
        return 1;
      };
    }
    else
    {
      printf("Key Send Error!\n\a");
      return -1;
    }

/*
    SWP_ReadCmd();
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx1),cmdx1);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx2),cmdx2);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx3),cmdx3);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
/*
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx4),cmdx4);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_ReadCmd();
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx5),cmdx5);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_ReadCmd();
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
/*
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx6),cmdx6);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx7),cmdx7);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx8),cmdx8);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_WriteMem(0xFFFFFFFF,sizeof(cmdx9),cmdx9);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};


    i=SWP_cmd_b(0x21,20);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_ReadCmd();
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_cmd_b(0x32,00);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_ReadCmd();
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};

/*
    for(x=0;x<0x100;x++)
    {
    i=SWP_cmd((BYTE)x);
    if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
    i=SWP_ReadCmd();
    }

//    SWP_WriteMem(0,0x20,boot1.b);
//    i=ReadChar();
    if(i!=0x106) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
*/
//  i=SWP_ReadCmd();
//  if(i!=0) { printf("Error no answer Ok! (%02X)\n\a",i); return i;};
//    printf("\n\nBoot Ok!\n\a\a\a");
    return 0;
}
