#include "stm32g0xx.h"
#include "stdlib.h"

#include "common.h"
#include "rtc.h"

#include "sdcard.h"
#include "fatfs.h"

#include "recorder.h"
#include "i2s.h"

/*
* Everything related to getting samples and writing it to file
*/

UINT bw;

uint8_t gain = 2;

void REC_Init(void)
{
    I2S_StartDMA((uint16_t *)i2s_samples, 2);
}

// Process 24 bit sample to 16 bit
int16_t REC_ProcessSample(uint16_t *samples)
{
    int32_t full_sample = (((int32_t)samples[0] << 16) | samples[1]);
    uint8_t sign = samples[0] >> 15;
    
    full_sample = full_sample << gain;
    int16_t sample = (sign << 15) ;

    int16_t temp_sample = 0x7FFF & (full_sample >> 16);

    samples_avg += abs(sample | temp_sample);

    return sample | temp_sample;

}

// Recorder function requied to be execute in DMA interrupt
void REC_ProcessDMA(void)
{
    static uint16_t i;

    if (current_buf == 0)
    {
        if (i < SAMPLES_BUF_SIZE)
        {
            samples_buf_0[i] = REC_ProcessSample((uint16_t *)i2s_samples);
            i++;
        }
        else
        {
            i = 0;
            current_buf = 1;
            samples_ready = 1;
            last_buf_avg = samples_avg / SAMPLES_BUF_SIZE;
            samples_avg = 0;
        }
        
    }
    else
    {
        if (i < SAMPLES_BUF_SIZE)
        {
            samples_buf_1[i] = REC_ProcessSample((uint16_t*)i2s_samples);
            i++;
        }
        else
        {
            i = 0;
            current_buf = 0;
            samples_ready = 1;
            last_buf_avg = samples_avg / SAMPLES_BUF_SIZE;
            samples_avg = 0; 
        }
        
    }
}

void REC_ProcessMain(void)
{
    if (samples_ready && recording_paused == 0)
    {
        if (current_buf == 0)
        {
            f_write(&fil, (int16_t*)samples_buf_1, SAMPLES_BUF_SIZE * 2, &bw);
        }
        else
        {
            f_write(&fil, (int16_t*)samples_buf_0, SAMPLES_BUF_SIZE * 2, &bw);
        }

        samples_ready = 0;
        byte_pos += 1024;
        
    }
}

void REC_Start(void)
{
    rtc_time_t start_time;

    RTC_GetTime(&start_time);


    rec_filename[0] = 'R';
    rec_filename[1] = 'E';
    rec_filename[2] = 'C';

    rec_filename[3] = '_';

    rec_filename[4] = '0' + start_time.hour / 10;
    rec_filename[5] = '0' + start_time.hour % 10;

    rec_filename[6] = '0' + start_time.minute / 10;
    rec_filename[7] = '0' + start_time.minute % 10;

    rec_filename[8] = '0' + start_time.second / 10;
    rec_filename[9] = '0' + start_time.second % 10;

    rec_filename[10] = '_';

    rec_filename[11] = '2';
    rec_filename[12] = '0';

    rec_filename[13] = '0' + start_time.year / 10;
    rec_filename[14] = '0' + start_time.year % 10;

    rec_filename[15] = '0' + start_time.month / 10;
    rec_filename[16] = '0' + start_time.month % 10;

    rec_filename[17] = '0' + start_time.day / 10;
    rec_filename[18] = '0' + start_time.day % 10;

    rec_filename[19] = '.';
    rec_filename[20] = 'W';
    rec_filename[21] = 'A';
    rec_filename[22] = 'V';

    f_open(&fil, rec_filename, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

}

void REC_End(void)
{
    char wav_header[WAV_HEADER_SIZE] = 
    {
        'R', 'I', 'F', 'F',
        0, 0, 0, 0,
        'W', 'A', 'V', 'E',
        'f', 'm', 't', ' ',
        16, 0, 0, 0,
        1, 0,
        1, 0,
        0x80, 0x3E, 0, 0, // Sample rate 16000
        0, 0x7D, 0, 0, // Byte rate 32000
        1,0,
        16,0,
        'd', 'a', 't', 'a',
        0,0,0,0,
    };

    // Data section size in Little-Endian
    wav_header[40] = (byte_pos >> 0);
    wav_header[41] = (byte_pos >> 8);
    wav_header[42] = (byte_pos >> 16);
    wav_header[43] = (byte_pos >> 24);

    // Overall file size in Little-Endian
    wav_header[4] = ((byte_pos + WAV_HEADER_SIZE) >> 0);
    wav_header[5] = ((byte_pos + WAV_HEADER_SIZE) >> 8);
    wav_header[6] = ((byte_pos + WAV_HEADER_SIZE) >> 16);
    wav_header[7] = ((byte_pos + WAV_HEADER_SIZE) >> 24);

    f_close(&fil);

    f_open(&fil, rec_filename, FA_READ | FA_WRITE);

    f_lseek(&fil,0);

    f_write(&fil, wav_header, WAV_HEADER_SIZE, &bw);

    f_close(&fil);
}