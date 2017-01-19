#ifndef _SPIRAM_H_
#define _SPIRAM_H_

#define SPIRAMSIZE (128*1024) //for a 23LC1024 chip


//Define this to use the SPI RAM in QSPI mode. This mode theoretically improves
//the bandwith to the chip four-fold, but it needs all 4 SDIO pins connected. It's
//disabled here because not everyone using the MP3 example will have those pins 
//connected and the overall speed increase on the MP3 example is negligable.
//#define SPIRAM_QIO


void spiRamInit();
void spiRamRead(int addr, char *buff, int len);
void spiRamWrite(int addr, char *buff, int len);
int spiRamTest();

#endif