#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "RS_io.h"
#include "SWP_io.h"

sswpreadbuf swprd;

int SWP_WriteBoot(WORD len, BYTE *data)
{
 BYTE chk=0;
 WORD w;
 int i;
    SendChar(0x30);
//    w=(len>>8)|(len<<8);
    w=len;
    if((i=SendBlk((BYTE*)&w,sizeof(w)))<0) return i;
    for(i=0;i<len;i++) chk^=data[i];
    if((i=SendBlk(data,len))<0) return i;
    SendChar(chk);
    swprd.size=0;
    if((i=ReadChar())<0) return i;
    if(i!=0x06) return i|0x100;
    return 0;
}


int SWP_WriteMem(DWORD addr, WORD len, BYTE *data)
{
 sswpdata swpdb;
 int size;
 int i;
   if((len==0)||(len>MAXSWPDATA)) return -13;
   swpdb.chk=0;
   swpdb.addr=(addr>>24)|(addr<<24)|((addr>>8)&0x0000FF00)|((addr<<8)&0x00FF0000);
   swpdb.len=(len>>8)|(len<<8);
   size=len+sizeof(sswphead)+sizeof(BYTE);
   printf("BUFout: ");
   for(i=0;i<sizeof(sswphead);i++) printf(" %02X",swpdb.b[i]);
   for(i=0;i<sizeof(sswphead);i++) swpdb.chk^=swpdb.b[i];
   for(i=0;i<len;i++) swpdb.chk^=data[i];
   if((i=SendBlk(swpdb.b,sizeof(sswphead)))<0) return i;
   if((i=SendBlk(data,len))<0) return i;
   for(i=0;i<len;i++) printf(" %02X",data[i]);
   printf(".\n");
   SendChar(swpdb.chk);
   if((i=ReadChar())<0) return i;
   if(i!=0x06) return i|0x100;
   return 0;
}

int SWP_cmd(BYTE cmd)
{
 sswpcmd swpcmd;
 int i;
   swpcmd.addr=0xffffffff;
   swpcmd.len=0x0100;
   swpcmd.cmd=cmd;
   swpcmd.chk=0;
   swpcmd.size=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE);
   for(i=0;i<swpcmd.size-1;i++) swpcmd.chk^=swpcmd.b[i];
   if((i=SendBlk(swpcmd.b,swpcmd.size))<0) return i;
   printf("BUFout: ");
   for(i=0;i<swpcmd.size;i++) printf(" %02X",swpcmd.b[i]);
   printf(".\n");
   if((i=ReadChar())<0) return i;
   if(i!=0x06) return i;
   return 0;
}

int SWP_cmd_dw(BYTE cmd, DWORD dw)
{
 sswpcmdd swpcmd;
 int i;
   swpcmd.addr=0xffffffff;
   swpcmd.chk=0;
   swpcmd.len=(sizeof(BYTE)+sizeof(DWORD))<<8;
   swpcmd.size=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE)+sizeof(DWORD);
   swpcmd.cmd=cmd;
   swpcmd.dw=(dw>>24)|(dw<<24)|((dw>>8)&0x0000FF00)|((dw<<8)&0x00FF0000);
   for(i=0;i<swpcmd.size;i++) swpcmd.chk^=swpcmd.b[i];
   if((i=SendBlk(swpcmd.b,swpcmd.size))<0) return i;
   printf("BUFout: ");
   for(i=0;i<swpcmd.size;i++) printf(" %02X",swpcmd.b[i]);
   printf(".\n");
   if((i=ReadChar())<0) return i;
   if(i!=0x06) return i|0x100;
   return 0;
}

int SWP_cmd_w(BYTE cmd, WORD w)
{
 sswpcmdw swpcmd;
 int i;
   swpcmd.addr=0xffffffff;
   swpcmd.chk=0;
   swpcmd.len=(sizeof(BYTE)+sizeof(WORD))<<8;
   swpcmd.size=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE)+sizeof(WORD);
   swpcmd.cmd=cmd;
   swpcmd.w=(w>>8)|(w<<8);
   for(i=0;i<swpcmd.size;i++) swpcmd.chk^=swpcmd.b[i];
   if((i=SendBlk(swpcmd.b,swpcmd.size))<0) return i;
   printf("BUFout: ");
   for(i=0;i<swpcmd.size;i++) printf(" %02X",swpcmd.b[i]);
   printf(".\n");
   if((i=ReadChar())<0) return i;
   if(i!=0x06) return i|0x100;
   return 0;
}

int SWP_cmd_b(BYTE cmd, BYTE b)
{
 sswpcmdb swpcmd;
 int i;
   swpcmd.addr=0xffffffff;
   swpcmd.chk=0;
   swpcmd.len=(sizeof(BYTE)+sizeof(BYTE))<<8;
   swpcmd.size=sizeof(sswphead)+sizeof(BYTE)+sizeof(BYTE)+sizeof(WORD);
   swpcmd.cmd=cmd;
   swpcmd.bb=b;
   for(i=0;i<swpcmd.size;i++) swpcmd.chk^=swpcmd.b[i];
   if((i=SendBlk(swpcmd.b,swpcmd.size))<0) return i;
   printf("BUFout: ");
   for(i=0;i<swpcmd.size;i++) printf(" %02X",swpcmd.b[i]);
   printf(".\n");
   if((i=ReadChar())<0) return i;
   if(i!=0x06) return i|0x100;
   return 0;
}


int SWP_TstReadCmd(void)
{
  int i,c;   
static WORD len=0;
static DWORD addr=0;
   if(swprd.size>=sizeof(swprd)-1) return -13;
   if((c=ReadChar())<0) return c;
   swprd.b[swprd.size++]=(BYTE)c;
//   printf("BUF[%i]:%08lX-%04X,",swprd.size,addr,len);
//  for(i=0;i<swprd.size;i++) printf(" %02X",swprd.b[i]);
   if(swprd.size==sizeof(sswphead))
   {
     addr=(swprd.addr>>24)|(swprd.addr<<24)|((swprd.addr>>8)&0x0000FF00)|((swprd.addr<<8)&0x00FF0000);
     len=(swprd.len<<8)|(swprd.len>>8);
     if((len>MAXSWPDATA)||(len==0)) c|=0x200;
//     printf(" HEAD:%08lX-%04X",addr,len);
   }
   else if(swprd.size==(int)(len+sizeof(sswphead)+1)) 
   {
     swprd.chk=0;  
     for(i=0;i<swprd.size-1;i++) swprd.chk^=swprd.b[i];
//   printf("BUF[%i]:%08lX-%04X,",swprd.size,addr,len);
//   for(i=0;i<len;i++) printf(" %02X",swprd.data[i]);
     swprd.size=0;
//     printf(" Chk %02X==%02X?",swprd.chk,(BYTE)c);
     if(swprd.chk==(BYTE)c)
     {
       SendChar(0x06);
//       printf(" Chk Ok\n");
//     addr=(swprd.addr>>24)|(swprd.addr<<24)|((swprd.addr>>8)&0x0000FF00)|((swprd.addr<<8)&0x00FF0000);
//       len=(swprd.len<<8)|(swprd.len>>8);
       return 0; 
     }
     else 
     {
       printf(" Chk Error!\n");
       return c|0x800;
     };
   };
   if(c&0x200)
   {
     if(swprd.size!=0)
     {
     swprd.size--;
//     swprd.b[swprd.size++]=(BYTE)c;
     for(i=0;i<swprd.size;i++) swprd.b[i]=swprd.b[i+1];
     };
     c^=0x200;
     printf(" shift\n");
   };
   if(swprd.size==sizeof(sswphead)+sizeof(BYTE))
   {
     addr=(swprd.addr>>24)|(swprd.addr<<24)|((swprd.addr>>8)&0x0000FF00)|((swprd.addr<<8)&0x00FF0000);
     len=(swprd.len<<8)|(swprd.len>>8);
   };
//    printf("\n");
   return 0x100|c;
}


int SWP_ReadCmd(void)
{
  int i;   
  WORD len;
  DWORD addr;
    do
    {
      i=SWP_TstReadCmd();
      if(i<=0) break;
    }
    while((i>0)&&(i<0x200));
    if(i==0)
    {
      addr=(swprd.addr>>24)|(swprd.addr<<24)|((swprd.addr>>8)&0x0000FF00)|((swprd.addr<<8)&0x00FF0000);
      len=(swprd.len<<8)|(swprd.len>>8);
      printf("SWP:%08lX-%04X ",addr,len);
      for(i=0;i<(int)len;i++) printf(" %02X",swprd.data[i]);
      printf("\n");
    }
    else { printf("Error(%i) Read cmd!\n\a",i); return i&0xff;};
    return 0;
}