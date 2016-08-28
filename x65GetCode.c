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

char hashkey[128];

// This routine extract the filename portion from a given full-path filename 
LPTSTR ExtractFileName (LPTSTR pFileSpec)    
{    
  LPTSTR   pFileName = NULL ;    
  TCHAR    DIRECTORY_DELIMITER1 = TEXT('\\') ;
  TCHAR    DIRECTORY_DELIMITER2 = TEXT(':') ;
  if (pFileSpec)       
  {       
    pFileName = pFileSpec + lstrlen (pFileSpec) ;  
    while (*pFileName != DIRECTORY_DELIMITER1 && 
      *pFileName != DIRECTORY_DELIMITER2)          
    {          
      if (pFileName == pFileSpec)  
      {             // done when no directory delimiter is found    
        break ;            
      }     
      pFileName-- ; 
    }     
    if (*pFileName == DIRECTORY_DELIMITER1 ||  
      *pFileName == DIRECTORY_DELIMITER2)  
    {          // directory delimiter found, point the filename right after it      
      pFileName++ ;
    }     
  }    
  return pFileName ;    
}  // ExtractFileName 



/*  Main program  */

int main(int argc, char **argv)
{
    int c,i;
    unsigned int z;
    DWORD esn;
    FILE * hf, *hflog;

    ComNum=2;
    maxbaud=115200;
    hostid=0x01;

        printf("Get data for calculation keys for x65+ telephones Siemens. v1.4b\n\t(c) All people of the Earth and PV`\n");
        if (argc<2) printf("Uses %s [COM Number]\n\n",ExtractFileName(argv[0]));
        else sscanf(argv[1],"%i",&ComNum);
        if (argc>2) sscanf(argv[2],"%i",&debprt);
        if (ComNum == 0) ComNum = 1;

        if (maxbaud<57600) maxbaud = 57600;

        if(InitBFB()) return 11;    // mode BFB

        if(phonemodel<65) InfoOldBFB();

        if((CmdBFB(0x05,0x23))||(ibfb.len!=0x5)) printf("Error Read FSN: Not OpenBFBpatch or FSNKey on x55, or bad x65!\n");
        else
        {
            printf("\nESN:\n");
            esn = *(DWORD*)ibfb.data_b;
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
        };
        if(phonemodel>=65)
        {
          if(BFBReadMem(0xA0000200,4))
          { // addr read flash x65 = 'SIEMENS'
            printf("Error Read addres %08lX!\n",addr);
            return 12;
          }
          else if((*(DWORD*)ibfb.data_b!=0x534C0200))
          {
            printf("Unknown BCORE tab of the telephone! %08lx\a\n",*(DWORD*)ibfb.data_b);
            return 13;
          }
          addr = 0xA0000236; // x65 flg Hash
        }
        else
        {
          if(BFBReadMem(0x00800300,4)) // addr read flash x55 = 'SIEMENS'
          {
            printf("Error Read addres %08lX!\n",addr);
            return 14;
          }
          else if((*(DWORD*)ibfb.data_b!=0x534C0100))
          {
            printf("Unknown BCORE tab of the telephone! %08lx\a\n",*(DWORD*)ibfb.data_b);
            return 15;
          }
          addr = 0x0080032E; // x55 flg Hash
        }
        if(BFBReadMem(addr,16))
        {
          printf("Error Read addres %08lX! \n",addr);
          return 16;
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
          return 17;
        }
        else
        {
            z=0;
            i=0;
            for (c = 0; c < 16; c++)  
            { 
              z+=(unsigned int)ibfb.data_b[c];
              i += sprintf(hashkey+i,"%02X", ibfb.data_b[c]);
//              printf("%02X ", ibfb.data_b[c]);
              
//              fprintf(hflog,"%02X ", ibfb.data_b[c]);
            };
            printf("\nHASH: %s\n",hashkey);
            if((z==0xff*16)||(z==0)) printf("\nBad Hash or The unknown type of the telephone!\n");
            else 
            {
              printf("\n");
            }
            if ((hf = fopen("Hash.bin", "wb")) == NULL)
            {
              printf("Cannot open Hash.bin file!\n");
              return 18;
            };
            if(fwrite(ibfb.data_b, 1, 16 , hf)!= 16)
            {
              printf("Cannot write Hash.bin file!\n");
              return 19;
            };
            fclose(hf);
            printf("Saved in Hash.bin\n");
        };
        EndBFB();
        if(phonemodel>=65)
        { 
          if(InitHostBFC(0x11)<0) if(InitHostBFC(0x11)<0) return 20;
          if ((hflog = fopen("GetCode.log", "wt")) == NULL)
          {
            printf("Cannot open GetCode.log file!\n");
            return 21;
          };
          if(BFCphonemodel()<0) return 22;
          else 
          {
            printf("\nPhoneModel: %s\n",&ibfc.data[1]);
            fprintf(hflog,"\nPhoneModel: %s ",&ibfc.data[1]);
          };
          if(SendBFC_GetInfo(0x0B)<0) return 23;
          else if(ibfc.data[0]==0x0b) 
          {
            printf("SW: %s\n",&ibfc.data[1]);
            fprintf(hflog,"SW: %s\n",&ibfc.data[1]);
          }
          if((c=BFC_GetHardwareIdentification())==-1) return 24;
          else 
          {
            fprintf(hflog,"HWid: ");
            printf("HWid: ");
            if(c<0) 
            { 
              fprintf(hflog,"Error!\n");
              printf("Error!\n");
            }
            else 
            {
              fprintf(hflog,"%i\n",c);
              printf("%i\n",c);
            }
          };
          fprintf(hflog,"HASH: %s\n",hashkey);
          fprintf(hflog,"ESN: 0x%08lX\n",esn);
          if(SendBFC_GetInfo(5)<0) return 25;
          else if(ibfc.data[0]==5) printf("IMEI: %s\n",&ibfc.data[1]);
          esn=(esn<<24)|(esn>>24)|((esn<<8)&0x00FF0000)|((esn>>8)&0x0000FF00);
          if(fprintf(hflog,"\n[Model: Siemens A50][PhoneID: %08lX][Desired IMEI: %s]\n",esn,&ibfc.data[1])<=0)
          {
              printf("Cannot write GetCode.log file!\n");
              return 26;
          };
          fclose(hflog);

//        if ((hf = fopen("Newflasher.ini", "wt")) != NULL)
//        {
//          fprintf(hf,"[Configuration]\nPort=%i\nSpeed=115200\nBootMethod=1\n",ComNum);
//          fprintf(hf,"Key=%s\n",hashkey);
//          fprintf(hf,"[Bootkeys]\n%s=%s\n",&ibfc.data[1],hashkey);
//          fclose(hf);
//        };
        TstOpenBFC();
        printf("\n");
        };
   EscapeCommFunction(hCom,CLRDTR);
   EscapeCommFunction(hCom,CLRRTS);
   Sleep(100);
   return 0;
}