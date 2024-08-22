
#ifndef __CRC_H__
#define __CRC_H__

#include "data_type.h"

extern unsigned char MODBUS_CRC[2];


extern unsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen);
extern void InvertUint8(unsigned char *DesBuf, unsigned char *SrcBuf);
extern void InvertUint16(unsigned short *DesBuf, unsigned short *SrcBuf);


#endif

