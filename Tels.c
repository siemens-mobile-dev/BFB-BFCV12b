#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

#include "RS_io.h"
#include "BFC_io.h"
#include "BFB_io.h"
#include "Tels.h"

//unsigned char atsqwe0[] = {"AT^SQWE=0\r"}; // RCCP mode
//unsigned char atsqwe1[] = {"AT^SQWE=1\r"}; // BFB or BFC mode
//unsigned char atsqwe2[] = {"AT^SQWE=2\r"}; // Gipsy mode
//unsigned char atsqwe3[] = {"AT^SQWE=3\r"}; // OBEX mode
BYTE errbootx65[] = {0x01,0x3,0x00,2,4,4,0x80,0x11};

int phonemodel = 0;
int wire = 1;
int mode=0;  //=0 - not init, =0xBFB - BFB , =0xBFC - BFC, =0x05441 - AT, = 0xDEAD - ? , =0xBAD - not set comport

unsigned char at_data[512];

int ReadOK(void)
{
    int c,i=0,z=0;
    at_data[0]=0;
    do
    {
      c=ReadChar();
      at_data[z++]=(BYTE)c;
      if(z>=sizeof(at_data)) { at_data[z]=0; return 0; }
      switch(c)
      {
        case 0x0A:
         if(i==101) return 1; // OK
         else if(i==15) return 2; // ERROR
           i=1;
         break;
        case 0x0D:
         if((i==100)||(i==14)) i++;
         else  i=1;
         break;
        case 'O':
         if((i==1)||(i==12)) i++;
         else i=0;
         break;
        case 'K':
         if(i==2) i=100;
         else i=0;
         break;
        case 'E':                // e10r11r12o13r14
         if(i==1) i=10;
         else i=0;
         break;
        case 'R':                // e10r11r12o13r14
         if((i==10)||(i==11)||(i==13)) i++;
         else i=0;
         break;
        default:
         i=0;
      }
    }
    while(c!=-1);
    return 0;
}

int SendAT(char *buf)
{
  int i;
  for(i=0;i<512;i++) if(buf[i]==0) break;
//  if(i==0) return -1;
  if(((*(WORD*)(buf))&0xDFDF)!=0x5441) SendBlk("AT",2);
  if(i==0) SendChar(0x0D);
  else SendBlk(buf,i);
  return ReadOK();
}

int SetMaxSpeedBFB(void)
{
   if(dcb.BaudRate<(DWORD)maxbaud)
    if(wire!=0)
    {
      printf("Test max speed BFB mode...\n");
        if(SetSpeedBFB(maxbaud)!=0)
        if(SetSpeedBFB(460000)!=0)
//      if(SetSpeedBFB(406000)!=0)
//      if(SetSpeedBFB(400000)!=0)
        if(SetSpeedBFB(230000)!=0)
//      if(SetSpeedBFB(203000)!=0)
//      if(SetSpeedBFB(200000)!=0)
        if(SetSpeedBFB(115200)!=0)
          SetSpeedBFB(57600);
      printf("Max speed BFB mode = %u Baud\n",dcb.BaudRate);
    };
    return dcb.BaudRate;
}

int SetMaxSpeedBFC(void)
{
   if(dcb.BaudRate<(DWORD)maxbaud)
    if(wire!=0)
    {
      printf("Test max speed BFC mode...\n");
//        if(SetSpeedBFC(maxbaud)!=0)
        if(SetSpeedBFC(460800)!=0)
//      if(SetSpeedBFB(400000)!=0)
        if(SetSpeedBFC(230400)!=0)
//      if(SetSpeedBFB(200000)!=0)
        if(SetSpeedBFC(115200)!=0)
          SetSpeedBFC(57600);
      printf("Max speed BFC mode = %u Baud\n",dcb.BaudRate);
    };
    return dcb.BaudRate;
}


int GetTelMode(int baud)
{
  int c=0;
        hostid=0x03;
//        printf("Init COM%u Speed %u Baud...\n",ComNum,baud);
        if(!InitCom(ComNum,baud)) return 0x0BAD; // not set speed
        PurgeComm(hCom,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
        if(!PingBFB()) return 0x0BFB;            // BFB mode
        if(baud==57600) if(!PingBFB()) return 0x0BFB;            // BFB mode
        if(!InitHostBFC(0x01)) return 0x0BFC;    // BFC mode
        if(baud==115200) if(!InitHostBFC(0x01)) return 0x0BFC;    // BFC mode
        SendBlk("AT\r",3);
//        Sleep(70);
        if(ReadOK()) return 0x05441; // AT mode
        return 0xDEAD; // ? mode
}

int FindPhone(void)
{
//      mode=GetTelMode(460800);
//  if(InitCom(ComNum,115200))
//  {
    //ReadChar();
  //}
  //else  return 0x0BAD;
  if(maxbaud<=57600)
  {
    maxbaud = 57600;
    mode=GetTelMode(maxbaud);
  }
  else mode=GetTelMode(115200);

  printf("%u Mode='%03X'\n",dcb.BaudRate,mode);
  if(mode==0xBAD) { printf("Com port ?\n"); return -1;};

  if(mode==0xDEAD)
  {
      if(maxbaud<=57600) return -1;
      mode=GetTelMode(460800);
      printf("%u Mode='%03X'\n",dcb.BaudRate,mode);
      if((mode==0xBAD)||(mode==0xDEAD))
      {
        if(maxbaud >= 230400)
        {
          mode=GetTelMode(230400);
          printf("%u Mode='%03X'\n",dcb.BaudRate,mode);
        };
      };
      if((mode==0xBAD)||(mode==0xDEAD))
      {
        mode=GetTelMode(115200);
        printf("%u Mode='%03X'\n",dcb.BaudRate,mode);
        if(mode==0xDEAD)
        {
          mode=GetTelMode(57600);
          printf("%u Mode='%03X'\n",dcb.BaudRate,mode);
        };
      };
  };
  return 0;
}

int InitBFC(void)
{
  int c;
  if(mode!=0x0BFC) if(FindPhone()) return(-1);
  switch(mode)
  {
    case 0xDEAD: // Telephones not found
      printf("Telephones not found!\n\a");
      return -1;
    case 0x0BFB: // x55 or x65 in BFB mode
      printf("Warning! Telephones on BFB mode!\n\a");
      return -1;
      break;
    case 0x0BFC: // x65 in BFC mode
      phonemodel=BFCphonemodel();
      break;
    case 0x05441: // x55 or x65 in AT mode
      SendAT("ATE0\r");
      if(SendAT("+CGMM\r")==1)
      {
        for(c=0;c<sizeof(at_data);c++)
        {
          if((at_data[c]>='0')&&(at_data[c]<='9')&&(at_data[c+1]>='0')&&(at_data[c+1]<='9'))
          {
           at_data[c+2]=0;
           phonemodel=atoi(&at_data[c]);
           break;
          };
        };
      };
      if(phonemodel<65)
      {
        if(phonemodel==0) printf("Unknown model of the telephone!\n\a");
        else  printf("No BFC for x%u phone model!\n\a",phonemodel);
        return(-3);
      }
      else  //x65
      {
        if(SendAT("^SIFS\r")!=1) printf("Error Get current interfacing!\n\a");
        else
        {
          if((at_data[9]=='B')||(at_data[9]=='I')) { printf("Error: x65+ BLUE and IRDA Bag! -> Works Only WIRE Interfacing!\n\a"); return -1; };
        };
        wire=1;
        if(SendAT("^SQWE=1\r")!=1) { printf("Error Set BFC\n\a"); return -2;}
        if(!InitCom(ComNum,115200)) return(-2);
        Sleep(100);  // x65 sleep!
      }
    default: // ? mode
      hostid=0x01;
      if(!InitHostBFC(0x11)) break;
  };
  if(phonemodel==0) phonemodel=BFCphonemodel();
  if(phonemodel==0)
  {
    printf("Unknown model of the telephone!\n\a");
    return -1;
  }
  else printf("The telephone of model x%u\n",phonemodel);
  c = TestOpenBFC();
  if(c==0x12) printf("FactorySecurityMode - enabled!\n");
  else if(c==0x11) printf("Warning! RepairMode (Service Centre) only! OpenBFC fractionally is opened! Please use PapuaKey!\n");
  else if(c==0x10) printf("Warning! CustomerMode only! OpenBFC disabled! Please use PapuaKey!\n");

//  SetMaxSpeedBFC();
  return 0;
}

int InitBFB(void)
{
  int c;
  if(mode!=0x0BFB) if(FindPhone()) return(-1);
  switch(mode)
  {
    case 0x0BFB: // x55 or x65 in BFB mode
//      phonemodel=BFBphonemodel();
      break;
    case 0x0BFC: // x65 in BFC mode
      phonemodel=BFCphonemodel();
      if((c=SetBFB_x65())!=0) return c;
      break;
    case 0xDEAD: // Telephones not found
      printf("Telephones not found!\n\a");
      return -1;
    case 0x05441: // x55 or x65 in AT mode
      SendAT("ATE0\r");
      if(SendAT("+CGMM\r")==1)
      {
        for(c=0;c<sizeof(at_data);c++)
        {
          if((at_data[c]>='0')&&(at_data[c]<='9')&&(at_data[c+1]>='0')&&(at_data[c+1]<='9'))
          {
           at_data[c+2]=0;
           phonemodel=atoi(&at_data[c]);
           break;
          };
        };
      };
      if(phonemodel<65)
      {
      if(phonemodel>=50)
        if(SendAT("^SIFS\r")!=1) printf("Error Get current interfacing!\n");
        else
        {
          if((at_data[9]=='B')||(at_data[9]=='I'))
          {
              wire=0;
              printf("Warning! x%u Phones BLUE and IRDA Interfacing -> Works Only 57600 Baud!\n",phonemodel);
          }
          else
          {
             ct.ReadTotalTimeoutConstant=50;
             wire=1;
             SetCommTimeouts(hCom,&ct);
          };
        };
        if(SendAT("^SBFB=1\r")==1)
        {
          Sleep(100);  // x55 sleep!
          if(!InitCom(ComNum,57600)) return(-2);
          break;
        };
      }
      else
      {
        if(phonemodel>=65)
        {
          if(SendAT("^SIFS\r")!=1) printf("Error Get current interfacing!\n");
          else
          {
            if((at_data[9]=='B')||(at_data[9]=='I')) { printf("Error: x65+ BLUE and IRDA Bag! -> Works Only WIRE Interfacing!\n"); return -1; };
          };
          wire=1;
          if(SendAT("^SQWE=1\r")!=1) { printf("Error Set BFC\n\a"); return -2;}
          if(!InitCom(ComNum,115200)) return(-2);
          Sleep(100);  // x65 sleep!
          hostid=0x01;
          if(!InitHostBFC(0x11))
          {
            if(phonemodel==0) phonemodel=BFCphonemodel();
            if((c=SetBFB_x65())!=0) return c;  // x65 in BFC mode
            else break;
          };
          if(!InitCom(ComNum,57600)) return(-2);
          break;
        }
        else
        {
          SetBFB_x65();
          SendAT("^SQWE=1\r");
          if(!InitCom(ComNum,57600)) return(-2);
        }
      }
    default: // ? mode
      if(!PingBFB()) break;
      printf("??? Mode!\n");
      if(SendAT("^SBFB=1\r"))             // AT Set Mode BFB
      {
        // no 'OK'
        SendAT("^SQWE=1\r");
        if(!InitCom(ComNum,115200)) return(-2);
        if(!InitHostBFC(0x01)) { if((c=SetBFB_x65())!=0) return c; } // x65 in BFC mode
      };
      if(InitCom(ComNum,57600)==0) return(-2);
  };

  if(PingBFB()) { printf("Error Ping BFB\n\a"); return 1;};
  if(!CmdBFB(0x02,0x42)) // Read DebugName
  {
    if((ibfb.len>=2)&&(ibfb.cmdb==ibfb.cmdb))
    {
      printf("%s",ibfb.data_b);
      if(ibfb.len==MAXBFBDATA)
      {
        if((!ReadBFB())&&(ibfb.len!=1))
        {
          if(ibfb.len>=1)
          {
            ibfb.data[ibfb.len]=0;
            printf("%s\n",ibfb.data);
            if(ibfb.data[2]=='0') if(phonemodel==0) phonemodel=65;
          }
        }
      }
    }
  }
  if(phonemodel==0) phonemodel=BFBphonemodel();
  if(phonemodel==0) printf("Unknown model of the telephone!\n");
  else printf("The telephone of model x%u\n",phonemodel);
  SetMaxSpeedBFB();
  return 0;
}


int InitBFx(void)
{
  int c;
  if(FindPhone()) return(-1);
  switch(mode)
  {
    case 0x0BFB: // x55 or x65 in BFB mode
      break;
    case 0x0BFC: // x65 in BFC mode
      phonemodel=BFCphonemodel();
      break;
//      return 0x0BFC;
    case 0xDEAD: // Telephones not found
      printf("Telephones not found!\n\a");
      return -1;
    case 0x05441: // x55 or x65 in AT mode
      SendAT("ATE0\r");
      if(SendAT("+CGMM\r")==1)
      {
        for(c=0;c<sizeof(at_data);c++)
        {
          if((at_data[c]>='0')&&(at_data[c]<='9')&&(at_data[c+1]>='0')&&(at_data[c+1]<='9'))
          {
           at_data[c+2]=0;
           phonemodel=atoi(&at_data[c]);
           break;
          };
        };
      };
      if(phonemodel<65)
      {
        if(phonemodel>=50)
        {
        if(SendAT("^SIFS\r")!=1) printf("Error Get current interfacing!\n");
        else
        {
          if((at_data[9]=='B')||(at_data[9]=='I'))
          {
              wire=0;
              printf("Warning! x%u Phones BLUE and IRDA Interfacing -> Works Only 57600 Baud!\n",phonemodel);
          }
          else
          {
             ct.ReadTotalTimeoutConstant=50;
             wire=1;
             SetCommTimeouts(hCom,&ct);
          };
        };
        };
        if(SendAT("^SBFB=1\r")==1)
        {
          Sleep(100);  // x55 sleep!
          if(!InitCom(ComNum,57600)) return(-2);
          mode = 0xBFB;
          break;
        };
//        };
      }
      else // phonemodel>=65
      {
          if(SendAT("^SIFS\r")!=1) printf("Error Get current interfacing!\n");
          else
          {
            if((at_data[9]=='B')||(at_data[9]=='I')) { printf("Error: x65+ BLUE and IRDA Bag! -> Works Only WIRE Interfacing!\n"); return -1; };
          };
          wire=1;
          if(SendAT("^SQWE=1\r")!=1) { printf("Error Set BFC\n\a"); return -2;}
          if(!InitCom(ComNum,115200)) return(-2);
          Sleep(100);  // x65 sleep!
          mode = 0xBFC;
          break;
      }
    default: // ? mode
      if(!PingBFB()) break;
      printf("??? Mode!\n");
      if(SendAT("^SBFB=1\r"))             // AT Set Mode BFB
      {
        // no 'OK'
        SendAT("^SQWE=1\r");
        if(!InitCom(ComNum,115200)) return(-2);
        if(!InitHostBFC(0x01))
        { // x65 in BFC mode
          mode = 0xBFC;
          break;
        }
      };
      if(InitCom(ComNum,57600)==0) return(-2);
//      if(PingBFB()) { printf("Error Ping BFB\n\a"); return 1;};
      mode=0xBFB;
//      return (-1);
  };
  if(mode==0xBFB)
  {
    if(PingBFB()) { printf("Error Ping BFB\n\a"); return 1;};
    if(!CmdBFB(0x02,0x42)) // Read DebugName
    {
      if((ibfb.len>=2)&&(ibfb.cmdb==ibfb.cmdb))
      {
        printf("%s",ibfb.data_b);
        if(ibfb.len==MAXBFBDATA)
        {
          if((!ReadBFB())&&(ibfb.len!=1))
          {
            if(ibfb.len>=1)
            {
              ibfb.data[ibfb.len]=0;
              printf("%s\n",ibfb.data);
              if(ibfb.data[2]=='0') if(phonemodel==0) phonemodel=65;
            }
          }
        }
      }
    }
    if(phonemodel==0) phonemodel=BFBphonemodel();
    SetMaxSpeedBFB();
  }
  else
    if(mode==0xBFC)
    {
      if(phonemodel==0) phonemodel=BFCphonemodel();
      c = TestOpenBFC();
      if(c==0x12) printf("FactorySecurityMode - enabled!\n");
      else if(c==0x11) printf("Warning! RepairMode (Service Centre) only! OpenBFC fractionally is opened! Please use PapuaKey!\n");
      else if(c==0x10) printf("Warning! CustomerMode only! OpenBFC disabled! Please use PapuaKey!\n");
      SetMaxSpeedBFC();
    }

  if(phonemodel==0) printf("Unknown model of the telephone!\n");
  else printf("The telephone of model x%u\n",phonemodel);
  return 0;
}