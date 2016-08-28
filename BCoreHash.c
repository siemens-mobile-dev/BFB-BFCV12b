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

   fprintf(out, "Calculation New BCore Hash keys for Siemens GSM telephones.\n\t(c) All people of the Earth and PV`\n\n");
   if ((in = fopen("key.bin", "rb")) == NULL)
   {
     fprintf(stderr, "Cannot open input key.bin file!\n\a");
     return 2;
   }
   if((i = fread(buffer, 1, 8 , in)) != 8)
   {
     fprintf(stderr, "Cannot read key.bin file!\n\a");
     return 3;
   }
   fclose(in);

   for(i=0;i<8;i++) buffer[i+8]=buffer[i]^buffer[i+3];
   fprintf(out, "Key:\n");
   for (i = 0; i < 16; i++) fprintf(out, "%02X", buffer[i]);

   MD5Init(&md5c);
   MD5Update(&md5c, buffer, (unsigned) 16 );
   MD5Final(signature, &md5c);
   fprintf(out, "\n\nBOOTKEY:\n");
   for (i = 0; i < sizeof signature; i++) fprintf(out, "%02X ", signature[i]);
   MD5Init(&md5c);
   MD5Update(&md5c, signature, (unsigned) 16 );
   MD5Final(csig, &md5c);
   fprintf(out, "\nNEWHASH:\n");
   for (i = 0; i < sizeof csig; i++) fprintf(out, "%02X ", csig[i]);
   if ((in = fopen("NewHash.bin", "wb")) == NULL)
   {
     fprintf(stderr, "Cannot open NewHash.bin file!\n");
     return 2;
   }
   if((i = fwrite(csig, 1, 16 , in)) != 16)
   {
     fprintf(stderr, "Cannot write NewHash.bin file!\n");
     return 3;
   }
   fclose(in);
   fprintf(out, "\nSaved in NewHash.bin.\n");

   MD5Init(&md5c);
   MD5Update(&md5c, buffer, (unsigned) 16 );
   MD5Final(csig, &md5c);
   fprintf(out,"\nEEP5122:\n");
   for (i = 4; i < 8; i++) fprintf(out, "%02X ", buffer[i]);
   fprintf(out,"58 00\nFlashSerialNumber:\n");
   for (i = 0; i < 4; i++) fprintf(out, "%02X", buffer[i]);
   fprintf(out,"\nServicePassword:\n'S%08u'\n",*(uint32*)(&buffer[i]));
   fprintf(out,"FullOpenPassword:\n'X%08u'\n\n",*(uint32*)(&buffer[i]));
   fcloseall();
   return 0;
}
