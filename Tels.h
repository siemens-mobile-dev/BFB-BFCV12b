#ifndef _TEL_IO_H_
#define _TEL_IO_H_

extern int phonemodel; //=35...72?
extern int wire;

int mode;
//=0 - not init
//=0xBFB - BFB
//=0xBFC - BFC
//=0x05441 - AT
//=0xDEAD - ?
//=0xBAD - not set comport

int ReadOK(void);
int GetTelMode(int baud);


int FindPhone(void);
int InitBFx(void);
int InitBFB(void);
int InitBFC(void);
int SendAT(char *buf);

#endif _TEL_IO_H_
