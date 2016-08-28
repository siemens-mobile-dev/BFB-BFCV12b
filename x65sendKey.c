#include <windows.h>
#include <stdio.h>

#include "RS_io.h"
#include "BFC_io.h"
#include "Tels.h"


union
{
 struct
 {
   DWORD fsn;
   DWORD key;
   BYTE xor[8];
 };
 BYTE b[16];
} keybin;

BYTE imeibuf[32];
BYTE keybuf[64];
//char buf_fname[512],drive[1],dir[64],name[9],ext[5];


FILE *in=0;//,*out = stdout;

DWORD inkey = 0;


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
  int x,i,keylen;
   printf("Send PapuaKey for Siemens x65 GSM telephones. v1.1\n (c) All people of the Earth and PV`\n\n");
   if((argc>1)&&((argv[1][0]<'0')||(argv[1][0]>'9')))
   {
     printf("Uses %s [[com number] PapuaKey or options]\n\n",ExtractFileName(argv[0]));
     printf("Example1: %s 7 12345678\n\n - To transmit PapuaKey='12345678' in the telephone through COM7.\n\n",ExtractFileName(argv[0]));
     printf("Example2: %s 2\n\n - To transmit PapuaKey from the 'key.bin' file in the telephone through COM2.\n\n",ExtractFileName(argv[0])); 
     printf("Example3: %s 5 CLOSE\n\n - Close PapuaKey (OpenAll) in the telephone through COM5.\n\n",ExtractFileName(argv[0])); 
     printf("Example4: %s 11 ?\n\n - Test OpenAll in the telephone through COM11.\n\n",ExtractFileName(argv[0])); 
     return(100);
   }
   if (argc<2)
   {
      printf("Use Help %s ?\n",ExtractFileName(argv[0]));
//      return(101);
   };

   hostid=0x01;

   if(argc>1) sscanf(argv[1],"%i",&ComNum);

   if(ComNum==0) ComNum=1;
   if(!InitCom(ComNum,115200)) return 11;
   printf("\n");
   SendAT("^SQWE=1\r");
   while(ReadChar()!=-1);

   //while(ReadChar()!=-1);
   if(BFCphonemodel()<0) return 12;
   else printf("\nPhoneModel: %s\n",&ibfc.data[1]);
   if(SendBFC_GetInfo(5)<0) return 13;
   else if(ibfc.data[0]==5) printf("IMEI: %s\n",&ibfc.data[1]);
   if((i=BFC_GetHardwareIdentification())==-1) return 14;
   else 
   {
     printf("HWid: ");
     if(i<0) printf("Error!\n");
     else printf("%i\n",i);
   };
   i = TstOpenBFC();
   if (argc<2) return 15; 
   if (argc>2) 
   {
     if((argv[2][0]<'0')||(argv[2][0]>'9'))
     {
       if((argv[2][0]=='C')&&(argv[2][1]=='L')&&(argv[2][2]=='O')&&(argv[2][3]=='S')&&(argv[2][4]=='E'))
       {
         i=0x005812;
         if((SendBFC_Cmd(0x25,0x20,(unsigned char *)&i,1)==0)&&(ibfc.data[0]==0x12))
         {
           printf("\nClose 'OpenAll' (PapuaKeys) - Ok!\n");
           return 0;
         }
         else
         {
           printf("\nClose 'OpenAll' - Error!\n");
           return 16;
         };
       };
//       if(argv[2][0]=='?') return i;
       return 17;
     }
     else
       sscanf(argv[2],"%i",&inkey);
   };

   if (inkey==0) 
   {
    if ((in = fopen("key.bin", "rb")) == NULL)
    {
      printf("Cannot open input key.bin file!\n");
      return 18;
    }
    if((i = fread(keybin.b, 1, 16 , in)) != 16)
    {
      printf("Cannot read key.bin file!\n");
      return 19;
    }
    fclose(in);
    x=1;
    for(i=0;i<8;i++) if(keybin.xor[i]!=(keybin.b[i]^keybin.b[i+3])) { x=0; break;}

    if(x==0) 
    {
      printf("Error file key.bin!\n\a\a");
      return 20;
    };
    keylen=sprintf(keybuf,"\x11X%08u",keybin.key);
   }
   else 
    keylen=sprintf(keybuf,"\x11X%08u",inkey);

   printf("\nSend KEY: '%s'\n\n",&keybuf[2]);

//    printf("\n");

   if(InitHostBFC(0x25)<0) { printf("Error Init BFC mode!\n\a"); return -1;};
   if((SendBFC_Cmd(0x25,0x20,keybuf,keylen+1)==0)&&(ibfc.data[0]==0x11))
    {
      if(ibfc.data[1]<31)
      {
        printf("Key Send - Ok! Waitings %i sec...\n\n",ibfc.data[1]);
        Sleep(ibfc.data[1]*1000+33);
        if (TstOpenBFC()==0) return 0;
        else return 1;
      }
      else
      {
        printf("Key Error or bad 5121 or 5123 blocks EEP!\n\n\a");
        return 21;
      };

    }
    else 
    {
      printf("Key Send - Error!\n\n\a");
      return 22;
    };
    return 23;
}
