#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "BFC_io.h"
#include "xcrc16.h"
#include "RS_io.h"

sbfc obfc; // output bfc block
sbfc ibfc; // input bfc block

char strbfcdeb[1024];


BYTE hostid=0x01;
BYTE oldhost = 0;


int InitBFCout(unsigned char bfcnum, unsigned char bfctype, unsigned char *buf, int lenbuf)
{
int i;

   if(lenbuf>sizeof(obfc.data)-2) return -1;
   obfc.idtx=bfcnum;
   obfc.type=bfctype;
   obfc.idrx=hostid;
   obfc.len=(WORD)(lenbuf<<8)|(lenbuf>>8);
   obfc.size=lenbuf+sizeof(sbfchead);
   obfc.chk=obfc.idtx^obfc.idrx^obfc.len^(obfc.len>>8)^obfc.type;
   memcpy(obfc.data,buf,lenbuf);
   if(obfc.type&0x20)
   {
     crc.w=0xFFFF;
     for(i=0;i<obfc.size;i++) UpdateCRC(obfc.b[i]);
     obfc.data[lenbuf]=crc.b.h;
     obfc.data[lenbuf+1]=crc.b.l;
     obfc.size+=2;
   }
   return 0;
}

int SendBFC_GetInfo(WORD data)
{
  if(InitBFCout(0x11,0x20,(BYTE*)&data,((data&0xff00)!=0 ? 2 : 1 ))) return -3;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  return ReadBfc();
}

int SendBFC_Cmd(unsigned char bfcnum, unsigned char bfctype, unsigned char *buf, int lenbuf)
{
int i;
  if((i=InitBFCout(bfcnum,bfctype,buf,lenbuf))<0) return i;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  return ReadBfc();
}

int InitHostBFC(BYTE hid)
{
  int i=0x1180;
  if(InitBFCout(hid,0x04,(BYTE*)&i, 2 )) return -3;
  if(debprt&2) PrintBFC(obfc);
  SendBlk(obfc.b,obfc.size);
  i=ReadBfc();
  if(i<0) return i;
  if(ibfc.len!=0x200) return -5;
  if((ibfc.data[1]==0x13)||(ibfc.data[1]==0x11))
  {
    oldhost = hid;
    return 0;
  };
  return -6;
}

int SendBFC_AT(char *buf)
{
  int i;
  for(i=0;i<sizeof(obfc.data)-2;i++) if(buf[i]==0) break;
  if(i>=sizeof(obfc.data)-2) return -1;
  if(oldhost!=0x17) if(InitHostBFC(0x17)) { printf("\nError BFC InitHost!\n\a"); return -1;};
  if(InitBFCout(0x17,0x20,(BYTE*)buf,i)) return -3;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  return ReadBfc();
}

void PrintBFC(sbfc pbfc)
{
 int i,len,flg=0,sd;
 DWORD dw;
 BYTE chk,cd;
  len=(int)((pbfc.len<<8)|(pbfc.len>>8))&0xFFFF;
  if(hostid==pbfc.idrx) cd=pbfc.idtx;
  else cd=pbfc.idrx;
  sd=sprintf(strbfcdeb, "HostID:%02X to HostID:%02X Len=%02X Type%02X CHK=%02X ",pbfc.idrx,pbfc.idtx,len,pbfc.type,pbfc.chk);
  for(i=0;i<sizeof(sbfchead)-1;i++) chk^=pbfc.b[i];
  if(pbfc,chk!=chk) sd+=sprintf(strbfcdeb+sd, "-Error! ");
  i=0;
  if(!(pbfc.type&4)) switch(cd)
  {
    case 0x01:
      i=1;
      sd+=sprintf(strbfcdeb+sd,"BFC#01(%02X) ",pbfc.data[0]);
      switch(pbfc.data[0])
      {
       case 0x01:
          if(hostid==pbfc.idrx) sd+=sprintf(strbfcdeb+sd,"Get");
          sd+=sprintf(strbfcdeb+sd,"CurBaud ");
          flg=5;
          break;
       case 0x10:
        if(hostid==pbfc.idrx) sd+=sprintf(strbfcdeb+sd,"InitBFB ");
        break;
/*
       case 0x01: // 00 01 C2 00
       case 0x02: // EE
       case 0x03: // 00 40
       case 0x04: // 00 40
       case 0x05: // 00
       case 0x06: // 00 00
       case 0x07: // 00 00
*/
       default:
        if(hostid==pbfc.idrx) sd+=sprintf(strbfcdeb+sd,"Get/Set? ");
      }
      break;
    case 0x11:
      i=1;
      sd+=sprintf(strbfcdeb+sd,"fun(%02X) ",pbfc.data[0]);

      if(hostid==pbfc.idrx) sd+=sprintf(strbfcdeb+sd,"Get");

      switch(pbfc.data[0])
      {
        case 0x01:
        case 0x03:
        case 0x04:
        case 0x0F:
        case 0x10:
          sd+=sprintf(strbfcdeb+sd,"ID? ");
          flg=-1;
          break;
        case 0x05:
          sd+=sprintf(strbfcdeb+sd,"IMEI "); 
          flg=1;
          break;
        case 0x06:
          if(len==2)
          {
            sd+=sprintf(strbfcdeb+sd,"SwInfo(%02X)",pbfc.data[1]);
            i=2;
          }
          else flg=1;
          break;
        case 0x07:
          sd+=sprintf(strbfcdeb+sd,"OperateTime ");
          flg=4; 
          break;
        case 0x08:
          sd+=sprintf(strbfcdeb+sd,"TalkTime ");
          flg=4;
          break;
        case 0x09:
          sd+=sprintf(strbfcdeb+sd,"Addr1 ");
          flg=2;
          break;
        case 0x0A:
          sd+=sprintf(strbfcdeb+sd,"Addr2 ");
          flg=2;
          break;
        case 0x0B:
          sd+=sprintf(strbfcdeb+sd,"SW ");
          flg=1;
          break;
        case 0x0C:
          sd+=sprintf(strbfcdeb+sd,"DevMan ");
          flg=1;
          break;
        case 0x0D:
          sd+=sprintf(strbfcdeb+sd,"DevName ");
          flg=1;
          break;
        case 0x0E:
          sd+=sprintf(strbfcdeb+sd,"LgVer ");
          flg=1;
          break;
        case 0x11:
          sd+=sprintf(strbfcdeb+sd,"PhoneType ");
          flg=-1;
          break;
        default:
          sd+=sprintf(strbfcdeb+sd,"? ");
          break;
      }
      break;
     case 0x1C:
      switch(pbfc.data[0])
      {
        case 0x01:
          i=1;
          sd+=sprintf(strbfcdeb+sd,"SimulateSim ");
          break;
      }

  }
  else
  if(len==2)
  {
    sd+=sprintf(strbfcdeb+sd,"InitHost(%02X) ",cd);
    switch(*(WORD*)(&pbfc.data))
    {
      case 0x1180:
        flg=3;
        break;
      case 0x1143:
        sd+=sprintf(strbfcdeb+sd,"Ok ");
        flg=3;
        break;
      case 0x1341:
        sd+=sprintf(strbfcdeb+sd,"Error ");
        flg=3;
        break;
      default:
        sd+=sprintf(strbfcdeb+sd,"Data? ");
    };
  };
  if(len>i)
  {
    switch(flg)
    {
      case 1:
        sd+=sprintf(strbfcdeb+sd,"'%s' ",&pbfc.data[1]);
        break;
      case 2:
        sd+=sprintf(strbfcdeb+sd,"= %08lX ",*(DWORD*)(&pbfc.data[1]));
        break;
      case 3:
        sd+=sprintf(strbfcdeb+sd,"(");
        while(i<len) sd+=sprintf(strbfcdeb+sd,"%02X",pbfc.data[i++]);
        sd+=sprintf(strbfcdeb+sd,")");
        break;
      case 4:
        sd+=sprintf(strbfcdeb+sd,"= %i ",*(DWORD*)(&pbfc.data[1]));
        break;
      case 5:
        dw=*(DWORD*)(&pbfc.data[1]);
        dw=(dw<<24)|(dw>>24)|((dw>>8)&0xFF00)|((dw<<8)&0xFF0000);
        sd+=sprintf(strbfcdeb+sd,"= %i ",dw);
        break;
      default:
        if(flg!=-1) sd+=sprintf(strbfcdeb+sd,"data: ");
        while(i<len) sd+=sprintf(strbfcdeb+sd,"%02X ",pbfc.data[i++]);
    }
  };
  if(pbfc.type&0x20)
  {
    sd+=sprintf(strbfcdeb+sd,"CRC16=%02X%02X ",pbfc.data[len],pbfc.data[len+1]);
    crc.w=0xFFFF;
    for(i=0;i<pbfc.size-2;i++) UpdateCRC(pbfc.b[i]);
    if((pbfc.data[len]!=crc.b.h)||(pbfc.data[len+1]!=crc.b.l))
      sd+=sprintf(strbfcdeb+sd,"- Error!");
  };
  sd+=sprintf(strbfcdeb+sd,"\n");
  printf(strbfcdeb);
}

int ReadBfc(void)
{
 int c,i;
 WORD len;
 BYTE chk;
  if(ReadBlk(ibfc.b,sizeof(sbfchead))) return -3;
  do
  {
    len=((ibfc.len<<8)|(ibfc.len>>8))&0x0FFFF;
//    if(len>0x4000) return -13;
//    len&=0x0FFFF;
    chk=0;
    for(i=0;i<sizeof(sbfchead)-1;i++) chk^=ibfc.b[i];
    if((ibfc.idrx==obfc.idtx)&&(len<=MAXBFCDATA)&&(ibfc.chk==chk)&&(!(ibfc.type&0xCB)))
    {
      ibfc.size=len+sizeof(sbfchead);
      if(ibfc.type&0x20)
      {
        if(len!=0) if(ReadBlk(ibfc.data,len+2)) return -2;
        crc.w=0xffff;
        for(i=0;i<ibfc.size;i++) UpdateCRC(ibfc.b[i]);
        ibfc.size+=2;
        if((ibfc.data[len]!=crc.b.h)||(ibfc.data[len+1]!=crc.b.l))
        {
          if(debprt&3)
           {
             PrintBFC(obfc); PrintBFC(ibfc);
             printf("ErorCRC: %02X%02X!=%02X%02X\n",ibfc.data[len],ibfc.data[len+1],crc.b.h,crc.b.l);
           }
          return 1;
        };
      }
      else if(len!=0)
      {
        if(ReadBlk(ibfc.data,len)) return -2;
      }
      if((debprt&4)&&(len>1)) { PrintBFC(obfc); PrintBFC(ibfc); }
      else if(debprt&1) PrintBFC(ibfc);
      return 0;
    }
    for(i=1;i<sizeof(sbfchead);i++) ibfc.b[i-1]=ibfc.b[i];
    c = ReadChar();
    if(c<0) return -1;
    ibfc.b[i] = c;
  }while(1);
}

int BFCphonemodel(void)
{
 int i;
 BYTE b;
        if(InitHostBFC(0x11)) { printf("\nError BFC InitHost!\n\a"); return -1;};
        if(!SendBFC_GetInfo(0x0D))
        {
          if((ibfc.data[0]==0x0D)&&(ibfc.len>0x400))
          {
            for(i=0;i<8;i++)
            {
              if((ibfc.data[i]>='0')&&(ibfc.data[i]<='9')&&(ibfc.data[i+1]>='0')&&(ibfc.data[i+1]<='9'))
              {
               b=ibfc.data[i+2];
               ibfc.data[i+2]=0;
               i=atoi(&ibfc.data[i]);
               ibfc.data[i+2]=b;
               return i;
              };
            };
          };
        };
        return 0;
}

int BFC_01(DWORD i)
{
 int c=1;
   if(i>0xFFFFFF) c=4;
   else if(i>0xFFFF) c=3;
   else if(i>0xFF) c=2;
   if(InitBFCout(0x01,0x00,(BYTE*)&i, c )) return -1;
   if(SendBlk(obfc.b,obfc.size)) return -2;
   if(debprt&2) PrintBFC(obfc);
   if(i!=0x10)
   {
     if(ReadBfc()) return -3;
     if(ibfc.data[0]!=(BYTE)i) return -4;
   }
   return 0;
}

int SetBFB_x65(void)
{
  if(InitHostBFC(0x01)) { printf("\nError BFC InitHost!\n\a"); return -1;};
  printf("Set 57600 BFB Mode\n");
  BFC_01(0x10); // BFC->BFB
  if(!InitCom(ComNum,57600)) return(-2);
  return 0;
}

int BFCReadMem(DWORD addr, DWORD len)
{
 union
 {
  struct
  {
   BYTE sub;
   DWORD addr;
   DWORD len;
  };
  BYTE b[9];
 }bfcrdmem;
  if(len>0x40) return -13;
  if(oldhost!=0x06) if(InitHostBFC(0x06)) { printf("\nError BFC InitHost!\n"); return -1;};
  bfcrdmem.sub=0x01;
  bfcrdmem.addr = addr; //(addr<<24)|(addr>>24)|((addr>>8)&0xFF00)|((addr<<8)&0xFF0000);
  bfcrdmem.len = len; //(len<<24)|(len>>24)|((len>>8)&0xFF00)|((len<<8)&0xFF0000);
  if(InitBFCout(0x06,0x20,bfcrdmem.b,sizeof(bfcrdmem))) return -3;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  if(ReadBfc()==0)
    if(*(WORD*)(&ibfc.data)==0x0001)
      if(ReadBfc()==0)
        if((WORD)((ibfc.len<<8)|(ibfc.len>>8))==len) return 0;
  return -1;
}

int SimSimBFC(void)
{
 //int i;
 BYTE x=0x01;
 hostid=0x01;
  if(oldhost!=0x1C) if(InitHostBFC(0x1C)) { printf("\nError BFC InitHost!\n"); return -1;};
  if(InitBFCout(0x1C,0x20,&x,1)) return -3;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  if(ReadBfc()==0)
   if(ibfc.data[0]==0x01) return 0;
  return -1;
};

int BFC_GetHardwareIdentification(void)
{
 //int i;
 BYTE x=0x01;
 //hostid=0x01;
  if(oldhost!=0x11) if(InitHostBFC(0x11)) { printf("\nError BFC InitHost!\n"); return -1;};
  if(InitBFCout(0x11,0x20,&x,1)) return -3;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  if(ReadBfc()==0)
   if(ibfc.data[0]==0x01) return (*(WORD*)(&ibfc.data[1]))&0x0FFFF;
  return -2;
};


int TestOpenBFC(void)
{
DWORD dw=0x100015;
  if(oldhost!=0x25) if(InitHostBFC(0x25)) { printf("\nError BFC InitHost!\n"); return -1;};
  if(InitBFCout(0x25,0x20,(BYTE*)&dw,4)) return -3;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  if(ReadBfc()==0)
   if((ibfc.data[0]==0x15)&&(ibfc.data[1]==0x01)) return ibfc.data[2];
  return -1;
}

int SetSpeedBFC(int baud)
{
 char spdx[32];
 int save_baud=dcb.BaudRate;
 int i;
 WORD w = 0x0115;

   if(baud<=0) return -13;
   if(baud>maxbaud) return -1;
   if(!InitCom(ComNum,baud))
   {
     if(!InitCom(ComNum,save_baud)) return -13;
     else return -1;
   }
   else if(!InitCom(ComNum,save_baud)) return -13;

  if(oldhost!=0x01) if(InitHostBFC(0x01)) { printf("\nError BFC InitHost!\n"); return -1;};

  i=sprintf(&spdx[1],"%i",baud);
  spdx[0]=2;
  if(InitBFCout(0x01,0x20,spdx,i+1)) return -3;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  if(ReadBfc()==0)
    if(memcmp(ibfc.data,obfc.data,i)==0)
    {
      if(InitBFCout(0x01,0x02,(BYTE*)&w,2)) return -3;
      SendBlk(obfc.b,obfc.size);
      Sleep(50);
      return ClearBFC(baud);
    }
  return -1;
}

int ClearBFC(int baud)
{
      EscapeCommFunction(hCom,CLRDTR);
      EscapeCommFunction(hCom,CLRRTS);
      Sleep(100);
      InitCom(ComNum,baud);
      Sleep(100);
      InitHostBFC(0x01);
      InitHostBFC(0x11);
      return InitHostBFC(0x01);
}

int BFC_TestServiceMode(void)
{
 BYTE x=0x02;
 //hostid=0x01;
  if(oldhost!=0x19) if(InitHostBFC(0x19)) { printf("\nError BFC InitHost!\n"); return -1;};
  if(InitBFCout(0x19,0x20,&x,1)) return -3;
  SendBlk(obfc.b,obfc.size);
  if(debprt&2) PrintBFC(obfc);
  if(ReadBfc()==0)
   if(ibfc.data[0]==0x02) return (ibfc.data[1])&0xff; // 0x12=NormalMode, 0x16=ServiceMode, 0x00=BurninMode ?
  return -2;
}

int TstOpenBFC(void)
{
  int i;
       i = TestOpenBFC();
       if(i==0x12)
       {
         printf("FactorySecurityMode - Enabled!\nOpenAll - Enable!\n");
         return 0;
       }
       else if(i==0x11) 
       {
         printf("Warning: RepairMode (Service Centre) only!\nOpenBFC fractionally is opened!\n");
         return 1;
       }
       else if(i==0x10)
       {
         printf("Warning: CustomerMode only!\nOpenBFC - Disabled!\nSecyrity EEP blocks - Closed!\nUse OpenAll or PapuaKey!\n");
         return 2;
       }
       else 
       { 
         printf("Error or Unknown SecyrityMode!\n"); 
         return -1; 
       };
}
