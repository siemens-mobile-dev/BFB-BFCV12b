#ifndef _xCRC_H_
#define _xCRC_H_

typedef union
{
  struct {
   unsigned char l;
   unsigned char h;
  } b;
#ifdef _WIN32
   unsigned short w;
#else
   unsigned int w;
#endif
} ucrc;

extern ucrc crc;

void UpdateCRC(unsigned char c);


#endif _xCRC_H_












