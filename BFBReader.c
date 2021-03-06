/*

  BFB reader.
  (c) PV` PapuaSoft & PapuaHard and all...

*/

#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "RS_io.h"
#include "BFC_io.h"
#include "BFB_io.h"
#include "Tels.h"


DWORD addr;
DWORD faddr=0xA0000000;
DWORD flen=256;//0x00010000;

char outfile[128];

/*  Main program  */

int main(int argc, char **argv)
{
    int i,len;
    FILE *hf;

        hostid=0x07;

        printf("BFB reader for telephones Siemens V1.1.\n(c) All people of the Earth and PV`\n");
        printf("\n x55 Works WIRE(57600..460000), BLUE/IRDA (57600)\n x65 Only WIRE 57600.\n\n");
        if (argc<2)
        {
          printf("Uses %s [<com number> <addr> <length> [x55MaxBaud]\n x55MaxBaud = 57600, 115200, 230400, 460800.\n\n",argv[0]);
          printf("Examle: %s 1 0xA0000000 65536 230400\n\n",argv[0]);
        }
        if (argc>1) ComNum = atoi(argv[1]);
        if (argc>2) sscanf(argv[2],"%i",&faddr);
        if (argc>3) sscanf(argv[3],"%i",&flen);
        if (argc>4) sscanf(argv[4],"%i",&maxbaud);
        if (argc>5) sscanf(argv[5],"%i",&debprt);
        if (ComNum == 0) ComNum = 1;
        if (flen==0) flen = 65536;

        if(InitBFB()) return 11;

        if(phonemodel<65) InfoOldBFB();

        sprintf(outfile,"%08lX_%08lX.bin",faddr,flen);
        if ((hf = fopen(outfile, "wb")) == NULL)
        {
          printf("Cannot open %s file!\n\a",outfile);
          return 2;
        }
        printf("Read addr 0x%08lX and write %u bytes in outfile '%s'...\n",faddr,flen,outfile);
        addr=faddr;
        len=31; // max BFB read 31 bytes in frame !
        while(flen)
        {
          printf("\r%08lX",addr);
          if(flen<(DWORD)len) len=(WORD)flen;
          if((i=BFBReadMem(addr,(WORD)len))!=0)
           if(PingBFB()==0) i=BFBReadMem(addr,(WORD)len);
          if(i!=0)
          {
            printf("\rError BFB Read addres %08lX size %u!\nNot OpenBFBpatch or FSNKey on x55, or bad addr x65!\n\a",addr,len);
            fcloseall();
            return 12;
          }
          if(fwrite(ibfb.data_b, 1, len , hf)!=(DWORD)len)
          {
            printf("\rCannot write %s file!\n\a",outfile);
            fcloseall();
            return 13;
          }
          flen-=len;
          addr+=len;
        }
        fcloseall();
        printf("\rWrite file '%s' - Ok.\n",outfile);
        EndBFB();
        return 0;
}