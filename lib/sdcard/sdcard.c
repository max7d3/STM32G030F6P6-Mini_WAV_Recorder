#include "stm32g0xx.h"

#include "sdcard.h"

/*
*   Just for holding all fs related things as globals but out of main
*/

FRESULT SDCARD_Mount(void)
{
    return f_mount(&FatFs, "", 1);
}

FRESULT SDCARD_GetFreeSpace(uint32_t *free_mb, uint32_t *card_size_mb)
{

    FRESULT res = FR_OK;

    FATFS* getFreeFs;

    DWORD free_clusters, free_sectors, total_sectors;

    res = f_getfree("", &free_clusters, &getFreeFs);

    if (res == FR_OK)
    {
        total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
        free_sectors = free_clusters * getFreeFs->csize;

        *free_mb = (free_sectors / 2)/1024;
        *card_size_mb = (total_sectors / 2)/1024;
    }
    return res;
}