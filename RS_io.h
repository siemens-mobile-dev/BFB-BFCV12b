#ifndef _RS_IO_H_
#define _RS_IO_H__

//#define TIMCMD 10  // - беда у меня со шнурами - всегда по разному...

extern int ComNum;
extern HANDLE hCom;
extern DCB dcb;
extern COMMTIMEOUTS ct;
extern int debprt;  // 0 - none, 1 - rx, 2 - tx, 3 - all , 4 - scan
extern int maxbaud;

int InitCom(int num, int baud);
int SendBlk(unsigned char *blk, int blksize);
void SendChar(unsigned char c);
int ReadChar(viod);
int ReadBlk(unsigned char *blk, int blksize);

#endif _RS_IO_H_
