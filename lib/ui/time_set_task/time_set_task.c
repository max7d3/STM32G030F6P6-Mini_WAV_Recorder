#include "stm32g0xx.h"
#include "string.h"

#include "common.h"


#include "gpio.h"
#include "buttons.h"


#include "ssd1306.h"
#include "BW_GFX.h"

#include "rtc.h"
#include "time_set_task.h"

/*
*   Basically copy of date_set_task but there is still some memory left so there's not need to optimize it.
*/


static void _ProcessNavigation(void)
{
    if (BTN_GetState(BTN_UP) > BTN_STATE_NOT_PRESSED)
    {
        BTN_ClearState(BTN_UP);

        if (set_time_status.changing_time == 1)
        {
            switch (set_time_status.selected_option)
            {
            case SET_HOURS:
                
                time.hour++;
                if (time.hour > 23)
                {
                    time.hour = 0;
                }
                
                break;

            case SET_MINUTES:
                
                time.minute++;
                if(time.minute > 59)
                {
                    time.minute = 0;
                }

                break;

            case SET_SECONDS:
                
                time.second++;
                if(time.second > 59)
                {
                    time.second = 0;
                }
                break;
            }
        }   
    }
    else if(BTN_GetState(BTN_DOWN) > BTN_STATE_NOT_PRESSED)    
    {
        BTN_ClearState(BTN_DOWN);

        if (set_time_status.changing_time == 1)
        {
            switch (set_time_status.selected_option)
            {
            case SET_HOURS:

                time.hour--;
                if (time.hour == 255)
                {
                    time.hour = 23;
                }
                
                break;

            case SET_MINUTES:

                time.minute--;
                if (time.minute == 255)
                {
                    time.minute = 59;
                }

                break;

            case SET_SECONDS:

                time.second--;
                if (time.second == 255)
                {
                    time.second = 59;
                }

                break;
            }
        }

    }
    else if(BTN_GetState(BTN_OK) == BTN_STATE_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        if (set_time_status.changing_time == 1)
        {
            set_time_status.selected_option++;

            if (set_time_status.selected_option > SET_SECONDS)
            {
                set_time_status.selected_option = 0;
                set_time_status.changing_time = 0;
                set_time_status.setting_seconds = 0;

                RTC_SetTime(time.hour, time.minute, time.second, time.year, time.month, time.day);

            }

            switch (set_time_status.selected_option)
            {
                case SET_HOURS:
                    break;
                case SET_MINUTES:

                    set_time_status.setting_hours = 0;
                    set_time_status.setting_minutes = 1;
                    break;

                case SET_SECONDS:
                    set_time_status.setting_minutes = 0;
                    set_time_status.setting_seconds = 1;
                    break;
                }
            
        }
        else
        {
            set_time_status.changing_time = 1;

            set_time_status.setting_hours = 1;
        }
        
    }
    else if (BTN_GetState(BTN_OK) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        set_time_status.changing_time = 0;
        set_time_status.selected_option = 0;
        set_time_status.setting_hours = 0;
        set_time_status.setting_minutes = 0;
        set_time_status.setting_seconds = 0;
        menu_status.task_active = 0;
    }  
}






void TASK_SetTime(void)
{
    char temp[3];
    memset(temp, 0, 3);

    if (set_time_status.changing_time == 0)
    {
        RTC_GetTime(&time);
    }

    if (set_time_status.changing_time == 1 && set_time_status.once == 0)
    {
        set_time_status.once = 1;
    }
    
    F_print00(time.hour, temp);

    if (set_time_status.setting_hours == 0)
    {
        GFX_Print(16, 8, temp, WHITE, BLACK, 2);
    }
    else
    {
        GFX_Print(16, 8, temp, BLACK, WHITE, 2);
        GFX_DrawFiledRectangle(16, 6, 24, 2, WHITE);
        GFX_DrawFiledRectangle(14, 6, 2, 18, WHITE);
    }

    GFX_Print(cursor_x, 8, ":", WHITE, BLACK, 2);

    F_print00(time.minute, temp);

    if (set_time_status.setting_minutes == 0)
    {
        GFX_Print(cursor_x, 8, temp, WHITE, BLACK, 2);
    }
    else
    {
        GFX_Print(cursor_x, 8, temp, BLACK, WHITE, 2);
        GFX_DrawFiledRectangle(16 + 24 + 12, 6, 24, 2, WHITE);
        GFX_DrawFiledRectangle(14 + 24 + 12, 6, 2, 18, WHITE);
    }

    F_print00(time.second, temp);

    GFX_Print(cursor_x, 8, ":", WHITE, BLACK, 2);

    if (set_time_status.setting_seconds == 0)
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