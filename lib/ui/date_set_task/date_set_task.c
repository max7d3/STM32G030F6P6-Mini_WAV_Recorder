#include "stm32g0xx.h"

#include "string.h"

#include "common.h"


#include "gpio.h"
#include "buttons.h"


#include "ssd1306.h"
#include "BW_GFX.h"

#include "rtc.h"
#include "date_set_task.h"

/*
*   Basically copy of time_set_task but there is still some memory left so there's not need to optimize it.
*/



static void _ProcessNavigation(void)
{
    if (BTN_GetState(BTN_UP) > BTN_STATE_NOT_PRESSED)
    {
        BTN_ClearState(BTN_UP);

        if (set_date_status.changing_date == 1)
        {
            switch (set_date_status.selected_option)
            {
            case SET_DAYS:
                
                date.day++;
                if (date.day > 31)
                {
                    date.day = 1;
                }
                
                break;

            case SET_MONTHS:
                
                date.month++;
                if(date.month > 12)
                {
                    date.month = 1;
                }

                break;

            case SET_YEARS:
                
                date.year++;
                if(date.year > 99)
                {
                    date.year = 0;
                }
                break;
            }
        }   
    }
    else if(BTN_GetState(BTN_DOWN) > BTN_STATE_NOT_PRESSED)    
    {
        BTN_ClearState(BTN_DOWN);

        if (set_date_status.changing_date == 1)
        {
            switch (set_date_status.selected_option)
            {
            case SET_DAYS:

                date.day--;
                if (date.day == 0)
                {
                    date.day = 31;
                }
                
                break;

            case SET_MONTHS:

                date.month--;
                if (date.month == 0)
                {
                    date.month = 12;
                }

                break;

            case SET_YEARS:

                date.year--;
                if (date.year == 255)
                {
                    date.year = 99;
                }

                break;
            }
        }

    }
    else if (BTN_GetState(BTN_OK) == BTN_STATE_PRESSED)
    {
        BTN_ClearState(BTN_OK);

        if (set_date_status.changing_date == 1)
        {
            set_date_status.selected_option++;

            if (set_date_status.selected_option > SET_YEARS)
            {
                set_date_status.selected_option = 0;
                set_date_status.changing_date = 0;
                set_date_status.setting_year = 0;

                rtc_time_t time;

                RTC_GetTime(&time);

                time.day = date.day;
                time.month = date.month;
                time.year = date.year;

                RTC_SetTime(time.hour, time.minute, time.second, time.year, time.month, time.day);

            }

            switch (set_date_status.selected_option)
            {
                case SET_DAYS:
                    break;
                case SET_MONTHS:

                    set_date_status.setting_days = 0;
                    set_date_status.setting_months = 1;
                    break;

                case SET_YEARS:
                    set_date_status.setting_months = 0;
                    set_date_status.setting_year = 1;
                    break;
                }
            
        }
        else
        {
            set_date_status.changing_date = 1;

            set_date_status.setting_days = 1;
        }
        
    }
    else if (BTN_GetState(BTN_OK) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        set_date_status.changing_date = 0;
        set_date_status.selected_option = 0;
        set_date_status.setting_days = 0;
        set_date_status.setting_months = 0;
        set_date_status.setting_year = 0;
        menu_status.task_active = 0;
    }  
}


void TASK_SetDate(void)
{
    char temp[3];
    memset(temp, 0, 3);

    if (set_date_status.changing_date == 0)
    {
        RTC_GetTime(&date);
    }

    if (set_date_status.changing_date == 1 && set_date_status.once == 0)
    {
        set_date_status.once = 1;
    }
    
    F_print00(date.day, temp);

    if (set_date_status.setting_days == 0)
    {
        GFX_Print(16, 8, temp, WHITE, BLACK, 2);
    }
    else
    {
        GFX_Print(16, 8, temp, BLACK, WHITE, 2);
        GFX_DrawFiledRectangle(16, 6, 24, 2, WHITE);
        GFX_DrawFiledRectangle(14, 6, 2, 18, WHITE);
    }

    GFX_Print(cursor_x, 8, ".", WHITE, BLACK, 2);

    F_print00(date.month, temp);

    if (set_date_status.setting_months == 0)
    {
        GFX_Print(cursor_x, 8, temp, WHITE, BLACK, 2);
    }
    else
    {
        GFX_Print(cursor_x, 8, temp, BLACK, WHITE, 2);
        GFX_DrawFiledRectangle(16 + 24 + 12, 6, 24, 2, WHITE);
        GFX_DrawFiledRectangle(14 + 24 + 12, 6, 2, 18, WHITE);
    }

    F_print00(date.year, temp);

    GFX_Print(cursor_x, 8, ".", WHITE, BLACK, 2);

    if (set_date_status.setting_year == 0)
    {
        GFX_Print(cursor_x, 8, temp, WHITE, BLACK, 2);
    }
    else
    {
        GFX_Print(cursor_x, 8, temp, BLACK, WHITE, 2);
        GFX_DrawFiledRectangle(16 + 24 + 24 + 12 + 12, 6, 24, 2, WHITE);
        GFX_DrawFiledRectangle(14 + 24 + 24 + 12 + 12, 6, 2, 18, WHITE);
    }


    

    _ProcessNavigation();    
}