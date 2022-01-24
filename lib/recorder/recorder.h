#ifndef _RECORDER_H_
#define _RECORDER_H_

#define SAMPLES_BUF_SIZE 512
#define WAV_HEADER_SIZE 44

char rec_filename[32];

uint8_t recording_in_progress;
uint8_t recording_paused;

uint8_t gain;

uint32_t samples_avg;
uint16_t last_buf_avg;

uint32_t byte_pos;


volatile uint8_t current_buf;
volatile uint8_t samples_ready;

volatile uint16_t i2s_samples[4];

volatile int16_t samples_buf_0[SAMPLES_BUF_SIZE];
volatile int16_t samples_buf_1[SAMPLES_BUF_SIZE];


void REC_Init(void);
int16_t REC_ProcessSample(uint16_t *samples);
void REC_ProcessDMA(void);
void REC_ProcessMain(void);

void REC_Start(void);
void REC_End(void);

#endif /* _RECORDER_H_ */