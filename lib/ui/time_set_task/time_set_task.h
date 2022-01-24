#ifndef _TIME_SET_TASK_H_
#define _TIME_SET_TASK_H_


#include "menu.h"

extern menu_status_t menu_status;

typedef enum
{
    SET_HOURS,
    SET_MINUTES,
    SET_SECONDS,
    
}time_selected_option_t;

typedef struct
{
    time_selected_option_t selected_option;

    uint8_t once:1;

    uint8_t changing_time:1;

    uint8_t setting_hours:1;
    uint8_t setting_minutes:1;
    uint8_t setting_seconds:1;

}set_time_status_t;

set_time_status_t set_time_status;

rtc_time_t time;

void TASK_SetTime(void);

#endif /* _TIME_SET_TASK_H_ */