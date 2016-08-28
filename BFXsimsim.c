/*

  BFB SimSim.
  (c) PV` PapuaSoft & PapuaHard and all...

*/

#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "RS_io.h"
#include "BFC_io.h"
#include "BFB_io.h"
#include "Tels.h"

/*  Main program  */

int main(int argc, char **argv)
{
  int i=-1;
//        hostid=0x07;
        printf("BFB and BFC Simulate SIM for telephones Siemens V1.0.Beta\n(c) All people of the Earth and PV`\n");
        printf("\n x55 Works WIRE(57600..460000), BLUE/IRDA (57600)\n x65 Only WIRE 57600.\n\n");
        if (argc<2)
        {
          printf("Uses %s [com number [x55MaxBaud]]\n x55MaxBaud = 57600, 115200, 230400, 460800.\n\n",argv[0]);
          printf("Examle: %s 1 115200\n\n",argv[0]);
        }
        if (argc>1) ComNum = atoi(argv[1]);
        if (argc>2) sscanf(argv[2],"%i",&maxbaud);
        if (maxbaud<57600) maxbaud = 57600;
        if (ComNum == 0) ComNum = 2;
        if(InitBFx()!=0) return 11;
        if(mode==0xBFB)
        {
          if(phonemodel<65)
          {
           i=SimSimBFB(); 
           EndBFB();
          }
          else printf("Reset Phone!\n");
        }
        else if(mode==0xBFC)
        {
          i=SimSimBFC(); 
          EndBFC();
        }
        if(i!=0) 
        {
          printf("Error SimSIM!\n\a");
          return 1;
        }
        else 
        {
          printf("SimSIM - Ok.\n");
          return 0;
        }
        return 13;
}