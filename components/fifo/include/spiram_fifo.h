#ifndef _SPIRAM_FIFO_H_
#define _SPIRAM_FIFO_H_

int  spiRamFifoInit();
void  spiRamFifoRead(char *buff, int len);
void  spiRamFifoWrite(const char *buff, int len);
int  spiRamFifoFill();
int  spiRamFifoFree();
long  spiRamGetOverrunCt();
long  spiRamGetUnderrunCt();

void spiRamFifoReset();
int spiRamFifoLen();

#endif
