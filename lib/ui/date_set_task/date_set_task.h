#ifndef _DATE_SET_TASK_H_
#define _DATE_SET_TASK_H_

#include "menu.h"

extern menu_status_t menu_status;

typedef enum
{
    SET_DAYS,
    SET_MONTHS,
    SET_YEARS,
    
}date_selected_option_t;

typedef struct
{
    date_selected_option_t selected_option;

    uint8_t once:1;

    uint8_t changing_date:1;

    uint8_t setting_days:1;
    uint8_t setting_months:1;
    uint8_t setting_year:1;

}set_date_status_t;

set_date_status_t set_date_status;

rtc_time_t date;


void TASK_SetDate(void);


#endif /* _DATE_SET_TASK_H_ */