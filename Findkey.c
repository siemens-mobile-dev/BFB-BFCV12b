/*

  Calculation of keys for telephones Siemens.
  (c) PV` PapuaSoft & PapuaHard

*/


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "md5.h"
#include "md5.c"

#define key ((uint32*)(&buffer[4]))[0]


/*  Main program  */

int main(void)
{
 unsigned char buffer[16], signature[16], csig[16];
 struct MD5Context md5c;
 int i, docheck = 0;
    FILE *in = stdin, *out = stdout;

    fprintf(out, "Calculation of keys for Siemens GSM telephones.\n (c) All people of the Earth and PV`\n\n");
        if ((in = fopen("hash.bin", "rb")) == NULL)
   {
     fprintf(stderr, "Cannot open input hash.bin file!\n");
     return 2;
   }
   if((i = fread(csig, 1, sizeof csig, in)) != sizeof csig)
   {
     fprintf(stderr, "Cannot read hash.bin file!\n");
     return 3;
   }
   fclose(in);

   if ((in = fopen("key.bin", "rb")) == NULL)
   {
     fprintf(stderr, "Cannot open input key.bin file!\n");
     return 2;
   }
   if((i = fread(buffer, 1, 8 , in)) != 8)
   {
     fprintf(stderr, "Cannot read key.bin file!\n");
     return 3;
   }
   fclose(in);

   for(i=0;i<8;i++) buffer[i+8]=buffer[i]^buffer[i+3];
   fprintf(out, "StartKey:\n");
   for (i = 0; i < 16; i++) fprintf(out, "%02X", buffer[i]);

   fprintf(out, "\nHASH:\n");
   for (i = 0; i < sizeof csig; i++) fprintf(out, "%02X", csig[i]);

   //docheck = 0;
   //key = 0;
   fprintf(out, "\nKEY:\n");
   fprintf(out, "%08lX\r", key);
  do
   {
     for(i=0;i<8;i++) buffer[i+8]=buffer[i]^buffer[i+3];
     MD5Init(&md5c);
     MD5Update(&md5c, buffer, (unsigned) 16 );
     MD5Final(signature, &md5c);
     MD5Init(&md5c);
     MD5Update(&md5c, signature, (unsigned) 16 );
     MD5Final(signature, &md5c);
     docheck = 0;
     for (i = 0; i < sizeof signature; i++)
     {
       if(signature[i] != csig[i])
       {
         docheck = 1;
         key++;
         break;
       };
     };
     if(key==99999999) fprintf(out, "\rWarning! Key is not retrieved!\nInsecure data or BootCore was re-recorded!\n\a");
     if(((key)&0xfffff)==0)
     {
       fprintf(out, "%08u\r", key);
           if(((key)&0xffffff)==0)
           {
         if ((in = fopen("savekey.bin", "wb")) == NULL)
                 {
           fprintf(stderr, "Cannot open savekey.bin file!\n");
           return 2;
                 };
         if((i = fwrite(buffer, 1, 16 , in)) != 16)
                 {
           fprintf(stderr, "Cannot write savekey.bin file!\n");
           return 3;
                 };
         fclose(in);
       }
     };
   }while((docheck!=0)&&(key!=0));

   if(docheck!=0)
   {
     fprintf(out, "\rKey is not retrieved!\nInsecure data or BootCore was re-recorded!\n\a");
     return 1;
   }
   for (i = 0; i < 16; i++) fprintf(out, "%02X", buffer[i]);

       if ((in = fopen("key.bin", "wb")) == NULL)
       {
         fprintf(stderr, "Cannot open savekey.bin file!\n");
         return 2;
       }
       if((i = fwrite(buffer, 1, 16 , in)) != 16)
       {
         fprintf(stderr, "Cannot write key.bin file!\n");
         return 3;
       }
       fclose(in);

   MD5Init(&md5c);
   MD5Update(&md5c, buffer, (unsigned) 16 );
   MD5Final(csig, &md5c);

   fprintf(out, "\n\nBOOTKEY:\n");

   for (i = 0; i < sizeof csig; i++) fprintf(out, "%02X ", csig[i]);

   fprintf(out,"\nEEP5122:\n");
   for (i = 4; i < 8; i++) fprintf(out, "%02X ", buffer[i]);
   fprintf(out,"58 00\nFlashSerialNumber:\n");
   for (i = 0; i < 4; i++) fprintf(out, "%02X", buffer[i]);
   fprintf(out,"\nServicePassword:\n'S%08u'\n",*(uint32*)(&buffer[i]));
   fprintf(out,"FullOpenPassword:\n'X%08u'\n\a",*(uint32*)(&buffer[i]));

       if ((in = fopen("bootkey.bin", "wb")) == NULL)
       {
         fprintf(stderr, "Cannot open bootkey.bin file!\n");
         return 2;
       }
       if((i = fwrite(csig, 1, 16 , in)) != 16)
       {
         fprintf(stderr, "Cannot write bootkey.bin file!\n");
         return 3;
       }
       fclose(in);


   fcloseall();
   return docheck;
}
