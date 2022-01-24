#ifndef _REC_TASK_H
#define _REC_TASK_H


#include "menu.h"

extern menu_status_t menu_status;

uint8_t *rec_icons_data[3];

typedef struct
{
    uint8_t once:1;
    uint8_t once_time:1;

    uint8_t selected_option:2;
    uint8_t prev_selected_option:2;

    uint8_t frames_passed;
    uint8_t animation_var;
    uint8_t animation_in_progress:1;

    uint8_t gain_menu_active:1;

}rec_task_status_t;

typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

}rec_time_t;

rec_time_t rec_time;

rec_task_status_t rec_status;

uint8_t prev_second;

uint16_t max_avg;
uint16_t prev_avg;
uint8_t avgs[63];


void TASK_Record(void);

#endif /* _REC_TASK_ H*/