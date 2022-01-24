#include "stm32g0xx.h"
#include "string.h"

#include "common.h"

#include "rtc.h"
#include "pwr.h"


#include "fatfs.h"
#include "sdcard.h"

#include "ssd1306.h"
#include "BW_GFX.h"

#include "recorder.h"
#include "gpio.h"
#include "buttons.h"

#include "icons.h" 
#include "rec_task.h"

/*
*   Recording screen UI
*/


uint8_t *rec_icons_data[3] = {(uint8_t*)&mini_settings_icon, (uint8_t*)&pause_icon, (uint8_t*)&stop_icon};


static void _ProcessNavigation(void)
{
    if (BTN_GetState(BTN_UP) == BTN_STATE_PRESSED)
    {
        BTN_ClearState(BTN_UP);

        if (rec_status.gain_menu_active == 1)
        {
            gain++;
            if (gain > 8)
            {
                gain = 8;
            }
        }
        else if(rec_status.animation_in_progress == 0)
        {
            rec_status.selected_option--;
            if (rec_status.selected_option > 3 - 1)
            {
                rec_status.selected_option = 0;
            }
        }
        
    }
    else if (BTN_GetState(BTN_UP) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_UP);
        show_battery_status = 1;
    }
    else if (BTN_GetState(BTN_DOWN) == BTN_STATE_PRESSED)    
    {
        BTN_ClearState(BTN_DOWN);

        if (rec_status.gain_menu_active == 1)
        {
            gain--;
            if (gain == 255)
            {
                gain = 0;
            }
        }
        else if(rec_status.animation_in_progress == 0)
        {
            rec_status.selected_option++;
            
            if (rec_status.selected_option > 3 - 1)
            {
                rec_status.selected_option = 3 - 1;
            }
        }
    }
    else if (BTN_GetState(BTN_DOWN) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_DOWN);
        show_battery_status = 1;
    }

    if (BTN_GetState(BTN_OK) == BTN_STATE_PRESSED)
    {
        BTN_ClearState(BTN_OK);

        switch (rec_status.selected_option)
        {
            case 0:

                if (rec_status.gain_menu_active == 0)
                {
                    rec_status.gain_menu_active = 1;
                }
                else
                {
                    rec_status.gain_menu_active = 0;
                }
                
                break;

            case 1:

                if (recording_paused == 1)
                {
                    recording_paused = 0;
                    rec_icons_data[1] = (uint8_t*)&pause_icon;
                }
                else
                {
                    recording_paused = 1;
                    rec_icons_data[1] = (uint8_t*)&play_icon;
                }
                break;

            case 2:
                    rec_status.once = 0;
                    rec_status.once_time = 0;
                    recording_in_progress = 0;
                    REC_End();
                    menu_status.task_active = 0;
                break;
        
            default:
                break;
        }
        
    }
    else if (BTN_GetState(BTN_OK) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_OK);

        if(rec_status.selected_option == 0)
        {
            if (rec_status.gain_menu_active == 1)
            {
                rec_status.gain_menu_active = 0;
            }
        }
        // Clear buttons status
        BTN_ClearState(BTN_UP);
        BTN_ClearState(BTN_DOWN);
    }  
}

static void _DrawChart(void)
{
    GFX_DrawRectangle(0,0, 63, 31, WHITE);

    for (uint8_t i = 0; i < 63; i++)
    {
        GFX_DrawLine(i, 15, i, 15 - avgs[i], WHITE);
        GFX_DrawLine(i, 15, i, 15 + avgs[i], WHITE);
    }

    if (recording_paused == 0)
    {
        for (uint8_t i = 0; i < 62; i++)
        {
            avgs[i] = avgs [i+1];
        }
    }

    switch (gain)
    {
    case 0:
        avgs[62] = map(last_buf_avg, 0, 400, 0, 15);
        break;

    case 1:
        avgs[62] = map(last_buf_avg, 0, 800, 0, 15);
        break;

    case 2:
        avgs[62] = map(last_buf_avg, 0, 1600, 0, 15);
        break;

    case 3:
        avgs[62] = map(last_buf_avg, 0, 2000, 0, 15);
        break;

    case 4:
        avgs[62] = map(last_buf_avg, 0, 4000, 0, 15);
        break;

    case 5:
        avgs[62] = map(last_buf_avg, 0, 10000, 0, 15);
        break;

    case 6:
        avgs[62] = map(last_buf_avg, 0, 14000, 0, 15);
        break;
    
    default:
        avgs[62] = map(last_buf_avg, 0, 16000, 0, 15);
        break;
    }

    if (avgs[62] > 15)
    {
        avgs[62] = 15;
    }

}

static void _DrawSelector(void)
{
    switch (rec_status.selected_option)
    {
    case 0:
        GFX_DrawRectangle(65, 12, 19, 19, WHITE);
        break;
    case 1:
        GFX_DrawRectangle(86, 12, 19, 19, WHITE);
        break;
    case 2:
        GFX_DrawRectangle(107, 12, 19, 19, WHITE);
        break;
    
    default:
        break;
    }
}

static void _AnimationEnd(void)
{
    rec_status.animation_var = 0;
    rec_status.animation_in_progress = 0;
    rec_status.frames_passed = 0;
    rec_status.prev_selected_option = rec_status.selected_option;

    _DrawSelector();  
}

static void _DrawMenu(void)
{
    // Draw icons
    for (uint8_t i = 0; i < 3; i++)
    {
        ssd1306_bitmap(66 + (21 * i), 13, rec_icons_data[i], 17, 17, WHITE);
    }

    // Option change ---->
    if (rec_status.selected_option > rec_status.prev_selected_option)
    {
        rec_status.animation_in_progress = 1;

        if (rec_status.frames_passed < 8)
        {
            for (uint8_t i = 1; i < 3; i++)
            {
                if (i == rec_status.selected_option)
                {
                    if (rec_status.frames_passed < 4)
                    {
                        GFX_DrawRectangle(65 +((i-1) * 21), 12, 20 + rec_status.animation_var, 19, WHITE);
                        rec_status.animation_var += 5;
                        if (rec_status.frames_passed == 3) rec_status.animation_var = 5;
                        
                    }
                    else
                    {
                        GFX_DrawRectangle(65 + (((i-1) * 21)) + rec_status.animation_var, 12, 20 + 20 - rec_status.animation_var, 19, WHITE);
                        rec_status.animation_var += 5;
                    }
                }
                
            }

            rec_status.frames_passed++;
            
        }
        else
        {
            _AnimationEnd();
        }
        
    }
    // Option change <----
    else if(rec_status.selected_option < rec_status.prev_selected_option)
    {
        rec_status.animation_in_progress = 1;

        if (rec_status.frames_passed < 8)
        {
            for (uint8_t i = 0; i < 3 - 1; i++)
            {
                if (i == rec_status.selected_option)
                {
                    if (rec_status.frames_passed < 4)
                    {

                        GFX_DrawRectangle(65 + (21*(i+1)) - rec_status.animation_var, 12, 20 + rec_status.animation_var, 19, WHITE);
                        rec_status.animation_var += 5;

                        if (rec_status.frames_passed == 3) rec_status.animation_var = 5;
                        
                    }
                    else
                    {
                        GFX_DrawRectangle(65 + (21*(i+1)) - 21, 12, 20+20-rec_status.animation_var, 19, WHITE);
                        rec_status.animation_var += 5;
                    }                    
                }

            }

            rec_status.frames_passed++;

        }
        else
        {
            _AnimationEnd();
        }

    }
    else
    {
        _DrawSelector();
    }

}

void _DrawTime(void)
{
    char temp[3];
    rtc_time_t current_time;

    memset(temp, 0, 3);

    if(recording_paused == 0)
    {
        if(rec_status.once_time == 1)
            {
            RTC_GetTime(&current_time);

            if (current_time.second != prev_second)
            {
                prev_second = current_time.second;
                rec_time.seconds++;
            }
        }
        else
        {
            RTC_GetTime(&current_time);
            prev_second = current_time.second;
            rec_status.once_time = 1;
            
        }
        

    }

    if (rec_time.seconds > 59)
    {
        rec_time.minutes++;
        rec_time.seconds = 0;
    }

    if (rec_time.minutes > 59)
    {
        rec_time.hours++;
        rec_time.minutes = 0;
    }

    GFX_DrawRectangle(62, 0, 66, 11, WHITE);

    F_print00(rec_time.hours, temp);
    GFX_Print(63 + (66 - (8 * 6)) / 2 , 2, temp, WHITE, BLACK, 1);

    GFX_Print(cursor_x, cursor_y, ":", WHITE, BLACK, 1);

    F_print00(rec_time.minutes, temp);
    GFX_Print(cursor_x, cursor_y, temp, WHITE, BLACK, 1);

    GFX_Print(cursor_x, cursor_y, ":", WHITE, BLACK, 1);

    F_print00(rec_time.seconds, temp);
    GFX_Print(cursor_x, cursor_y, temp, WHITE, BLACK, 1);
    
}

void _DrawGainMenu(void)
{
    char temp[2] = {('0' + gain)};

    GFX_DrawFiledRectangle(41, 6, 46, 20, BLACK);
    GFX_DrawRectangle(41, 6, 46, 20, WHITE);

    GFX_PrintCentered(8, "GAIN", WHITE, BLACK, 1);

    GFX_PrintCentered(16, temp, WHITE, BLACK, 1);
    
}


void TASK_Record(void)
{
    if (rec_status.once == 0)
    {

        for (uint8_t i = 0; i < 63; i++)
        {
            avgs[i] = 0;
        }

        rec_time.seconds = 0;
        rec_time.minutes = 0;
        rec_time.hours = 0;

        rec_status.selected_option = 0;
        rec_status.prev_selected_option = 0;

        recording_paused = 0;
        rec_icons_data[1] = (uint8_t*)&pause_icon;
        recording_in_progress = 1;
        REC_Start();//(&fil, "REC.BIN", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
        rec_status.once = 1;
    }

 
    _DrawChart();

    _DrawTime();

    _DrawMenu();

    if (rec_status.gain_menu_active == 1)
    {
        _DrawGainMenu();
    }

    _ProcessNavigation();
    
}