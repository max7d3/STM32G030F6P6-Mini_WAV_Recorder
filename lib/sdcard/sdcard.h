#ifndef _SDCARD_H_
#define _SDCARD_H_

#include "fatfs.h"

uint8_t sdcard_mounted;


FATFS FatFs; 	//Fatfs handle
FIL fil; 		//File handle
FRESULT fres;   //Result after operations

FRESULT SDCARD_Mount(void);

FRESULT SDCARD_GetFreeSpace(uint32_t *free_mb, uint32_t *card_size_mb);

#endif /* _SDCARD_H_ */