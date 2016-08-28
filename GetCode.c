/*

  Get data for calculation keys for telephones Siemens.
  (c) PV` PapuaSoft & PapuaHard and All

*/

#include <windows.h>
#include <stdio.h>
#include <io.h>


#include "RS_io.h"
#include "BFC_io.h"
#include "BFB_io.h"
#include "Tels.h"

DWORD addr;
DWORD faddr=0xA0000236;

char outfile[128];


/*  Main program  */

int main(int argc, char **argv)
{
    int c;
    unsigned int z;
    FILE *hf;

    ComNum=2;
        hostid=0x01;

        printf("Get data for calculation keys for telephones Siemens. v1.3\n\t(c) All people of the Earth and PV`\n");
        printf("\n x55 Works WIRE(57600..460000), BLUE/IRDA (57600)\n x65 Only WIRE 57600.\n\n");
        if (argc<2) printf("Uses %s [[COM Number] MaxBaud]\n MaxBaud: 57600, 115200, 230400, 460800.\n\n",argv[0]);
        else ComNum = atoi(argv[1]);
        if (argc>2) sscanf(argv[2],"%i",&maxbaud);
        if (argc>3) sscanf(argv[3],"%i",&debprt);
        if (ComNum == 0) ComNum = 1;

        if(InitBFB()) return -1;    // mode BFB

       if(phonemodel<65) InfoOldBFB();

          if((CmdBFB(0x05,0x23))||(ibfb.len!=0x5)) printf("Error Read FSN: Not OpenBFBpatch or FSNKey on x55, or bad x65!\n");
          else
          {
            printf("\nFSN:\n");
            for (c = 0; c < 4; c++) printf("%02X ", ibfb.data_b[c]);
            printf("\n");
            if ((hf = fopen("key.bin", "wb")) == NULL)
            {
              printf("Cannot open Key.bin file!\n");
              return 2;
            }
            for(c=4;c<17;c++) ibfb.data_b[c]=0;
            if(fwrite(ibfb.data_b, 1, 16 , hf) != 16)
            {
              printf("Cannot write Key.bin file!\n");
              return 3;
            }
            fclose(hf);
            printf("Saved in Key.bin\n");
          }
        if(phonemodel>=65)
        {
          if(BFBReadMem(0xA0000200,4))
          { // addr read flash x65 = 'SIEMENS'
            printf("Error Read addres %08lX!\n",addr);
            return -3;
          }
          else if((*(DWORD*)ibfb.data_b!=0x534C0200))
          {
            printf("Unknown BCORE tab of the telephone! %08lx\a\n",*(DWORD*)ibfb.data_b);
            return -3;
          }
          addr = 0xA0000236; // x65 flg Hash
        }
        else
        {
          if(BFBReadMem(0x00800300,4)) // addr read flash x55 = 'SIEMENS'
          {
            printf("Error Read addres %08lX!\n",addr);
            return -3;
          }
          else if((*(DWORD*)ibfb.data_b!=0x534C0100))
          {
            printf("Unknown BCORE tab of the telephone! %08lx\a\n",*(DWORD*)ibfb.data_b);
            return -4;
          }
          addr = 0x0080032E; // x55 flg Hash
        }
        if(BFBReadMem(addr,16))
        {
          printf("Error Read addres %08lX! \n",addr);
          return -3;
        }
        else
        {
          if((*(WORD*)ibfb.data_b==0xFFFF))
           printf("No Set BootHash of the telephone!\a\n");
        };
        addr+=2;
        if(BFBReadMem(addr,16))
        {
          printf("Error Read addres %08lX!\n",addr);
          return -3;
        }
        else
        {
            printf("\nHASH:\n");
            z=0;
            for (c = 0; c < 16; c++)  { z+=(unsigned int)ibfb.data_b[c]; printf("%02X ", ibfb.data_b[c]); }
            if((z==0xff*16)||(z==0)) printf("\nBad Hash or The unknown type of the telephone!\n");
            else printf("\n");
            if ((hf = fopen("Hash.bin", "wb")) == NULL)
            {
              printf("Cannot open Hash.bin file!\n");
              return 2;
            };
            if(fwrite(ibfb.data_b, 1, 16 , hf)!= 16)
            {
              printf("Cannot write Hash.bin file!\n");
              return 3;
            };
            fclose(hf);
            printf("Saved in Hash.bin\n");
        };
        EndBFB();
        if(phonemodel>=65) InitHostBFC(0x11);
   EscapeCommFunction(hCom,CLRDTR);
   EscapeCommFunction(hCom,CLRRTS);
   Sleep(100);
   return 0;
}