#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>


int ComNum=2;
int maxbaud = 920600;
HANDLE hCom =0;
DCB dcb;
COMMTIMEOUTS ct ={ 0,20,200,MAXDWORD,MAXDWORD};

int debprt=0;  // 0 - none, 1 - rx, 2 - tx, 3 - all , 4 - scan

int InitCom(int num, int baud)
{
  char strcom[64];
  int fSuccess;
  int save_baud;
  static int PowerApUSB=1;

    if (baud>maxbaud) return 0;
    if((PowerApUSB==0)&&(dcb.BaudRate==(DWORD)baud)&&(num==ComNum)) return 2;


    if((PowerApUSB==0)&&(num==ComNum)&&(hCom!=0))
    {
        if(!GetCommState(hCom, &dcb)) { printf("Can`t get comm state, err=%08X\n",GetLastError()); return 0;};
        save_baud = dcb.BaudRate;
        EscapeCommFunction(hCom,CLRDTR);
        Sleep(25);
        dcb.BaudRate = baud;
        if (!SetCommState(hCom, &dcb)) {
          if ((baud>115200)&&(maxbaud > baud)) maxbaud = baud;
          dcb.BaudRate = save_baud;
          if (!SetCommState(hCom, &dcb)) printf("Can`t set comm state, err=%08X\n",GetLastError());
          return 0;
        };
        SetCommTimeouts(hCom,&ct);
        ClearCommError(hCom,&fSuccess,0);
        Sleep(100);
        return 1;
    }else PowerApUSB = 1;

    sprintf(strcom,"\\\\.\\COM%u",num);
    if(hCom)
    {
//        EscapeCommFunction(hCom,CLRDTR);  // Ignition?
//        Sleep(25);
//       EscapeCommFunction(hCom,CLRRTS);  // Ignition?
//        Sleep(25);
       CloseHandle(hCom); hCom=0;
//        Sleep(50);
    }

    printf("Init COM%u %u Baud...\n",num,baud);
 
    hCom = CreateFile(strcom ,
        GENERIC_READ | GENERIC_WRITE, /* access (read-write) mode */
        0,    /* share mode */
        NULL, /* pointer to security attributes */
        OPEN_EXISTING, /* how to create */
        0,    /* file attributes */
        NULL  /* handle to file with attributes to copy */
        );
    if(hCom==INVALID_HANDLE_VALUE)
    {
        printf("Unable to open COM%u port\n",num);
        return 0;
    }

    fSuccess = GetCommState(hCom, &dcb);

    if (!fSuccess) {
        printf("Can`t get comm state, err=%08X\n",GetLastError());
        return 0;
    }

    /* Fill in the DCB: baud, 8 data bits, no parity, 1 stop bit. */

    dcb.BaudRate = baud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fAbortOnError = FALSE;
    dcb.fRtsControl = 1;            // RTS flow control
    dcb.fDtrControl = 1;            // DTR flow control type
    dcb.fOutxCtsFlow= 0;            // CTS output flow control
    dcb.fOutxDsrFlow= 0;            // DSR output flow control


    fSuccess = SetCommState(hCom, &dcb);

    if (!fSuccess) {
        /* Handle the error. */
        printf("Can`t set comm state, err=%08X\n",GetLastError());
        return 0;
    }
/*    GetCommTimeouts(hCom,&ct);
    ct.ReadIntervalTimeout=0;
    ct.ReadTotalTimeoutMultiplier=20; //20;
    ct.ReadTotalTimeoutConstant=200; //50
    ct.WriteTotalTimeoutMultiplier=MAXDWORD;
    ct.WriteTotalTimeoutConstant=MAXDWORD;
*/
    SetCommTimeouts(hCom,&ct);
//  Sleep(25);
//  EscapeCommFunction(hCom,SETDTR);
    if(PowerApUSB) { Sleep(50); PowerApUSB=0; }
    return 1;
}


int SendBlk(unsigned char *blk, int blksize)
{
    int w=0;
    ClearCommError(hCom,&w,0);
    WriteFile(hCom,blk,blksize,&w,NULL);
    return (w!=blksize ? -1 : 0);
}

void SendChar(unsigned char c)
{
    SendBlk(&c,1);
}

int ReadChar(void)
{
    DWORD BuffLen=0;
    char RecvBuff=0;
    ReadFile(hCom,&RecvBuff,1,&BuffLen,NULL);
    if(BuffLen==1)  return ((int)RecvBuff)&255;
    ClearCommError(hCom,&BuffLen,0);
    return -1;
}


int ReadBlk(unsigned char *blk, int blksize)
{
    DWORD BuffLen=0;
    ReadFile(hCom,blk,blksize,&BuffLen,NULL);
    if(BuffLen==(DWORD)blksize)  return 0;
    ClearCommError(hCom,&BuffLen,0);
    return -1;
}