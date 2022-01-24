#include "stm32g0xx.h"

#include "stdlib.h"
#include "string.h"

#include "common.h"

#include "ssd1306.h"
#include "BW_GFX.h"
#include "gpio.h"
#include "adc.h"
#include "buttons.h"
#include "pwr.h"

#include "sdcard.h"
#include "diskio.h"
#include "ff_gen_drv.h"
#include "user_diskio.h"
#include "fatfs.h"
#include "recorder.h"

#include "menu.h"
#include "icons.h"


#include "rec_task/rec_task.h"

#include "rtc.h"
#include "time_set_task/time_set_task.h"
#include "date_set_task/date_set_task.h"

/*
*   All UI handling
*/


const menu_t menu [] = 
{
/*****************************************************************/

    {/* MENU with IDX 0 */

        4, /*Options count*/

        ICON_MENU, /*Type */
        
        { 
            {   /* Option name*/
                {"Record"},
                /*Option callbacks*/
                &_MENU_Task, NULL,
                /*Option info*/
                {1, RECORD_TASK, 0, (uint8_t*)&mic_icon},
            },

            {   /* Option name*/
                {"Settings"},
                /*Option callbacks*/
                &_MENU_Change, NULL,
                /*Option info*/
                {1, SETTINGS_MENU, 0, (uint8_t*)&gear_icon},
            },

            {   /* Option name*/
                {"Info"},
                /*Option callbacks*/
                &_MENU_Task, NULL,
                /*Option info*/
                {1, INFO_TASK, 0, (uint8_t*)&info_icon},
            },

            {   /* Option name*/
                {"Power"},
                /*Option callbacks*/
                &PWR_EnterSleep, NULL,
                /*Option info*/
                {1, 0, 0, (uint8_t*)&power_icon},
            },

        }
    
    },/* MENU with IDX 0 */

/*****************************************************************/

/*****************************************************************/

    {/* MENU with IDX 1 */

        4, /*Options count*/

        TEXT_MENU, /*Type */
        
        { 
            {   /* Option name*/
                {"Set Time"},
                /*Option callbacks*/
                &_MENU_Task, NULL,
                /*Option info*/
                {1, TIME_SET_TASK, 0, (uint8_t*)&mic_icon},
            },

            {   /* Option name*/
                {"Set Date"},
                /*Option callbacks*/
                &_MENU_Task, NULL,
                /*Option info*/
                {1, DATE_SET_TASK, 0, (uint8_t*)&power_icon},
            },

            {   /* Option name*/
                {"SD Init"},
                /*Option callbacks*/
                &_MENU_Task, NULL,
                /*Option info*/
                {1, SD_INIT_TASK, 0, (uint8_t*)&gear_icon},
            },

            {   /* Option name*/
                {"SD Info"},
                /*Option callbacks*/
                &_MENU_Task, NULL,
                /*Option info*/
                {1, SD_INFO_TASK, 0, (uint8_t*)&info_icon},
            },

        }
    
    },/* MENU with IDX 1 */

/*****************************************************************/
};

//menu_status_t menu_status = {0, 0, 0, 0, 8};


static void _MENU_ProcessNavigation(const menu_t *_menu)
{

    if (BTN_GetState(BTN_UP) == BTN_STATE_PRESSED)
    {
        BTN_ClearState(BTN_UP);

        if(menu_status.animation_in_progress == 0)
        {
            menu_status.selected_option--;
            if (menu_status.selected_option > _menu->options_count - 1)
            {
                menu_status.selected_option = 0;
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

        if(menu_status.animation_in_progress == 0)
        {
            menu_status.selected_option++;
            
            if (menu_status.selected_option > _menu->options_count - 1)
            {
                menu_status.selected_option = _menu->options_count - 1;
            }
        }
    }
    else if (BTN_GetState(BTN_DOWN) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_DOWN);
        show_battery_status = 1;
    }
    else if (BTN_GetState(BTN_OK) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        menu_status.btn_back_press = 1;
    }
    else if (BTN_GetState(BTN_OK) == BTN_STATE_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        menu_status.btn_ok_press = 1;
    }

}

static void _MENU_ProcessBackOk(const menu_t *_menu)
{
    if (menu_status.btn_ok_press == 1)
    {
        if (_menu->option[menu_status.selected_option].clicked_callback != NULL)
        {
            _menu->option[menu_status.selected_option].clicked_callback(_menu->option[menu_status.selected_option].info.next_menu_id);
        }

        menu_status.btn_ok_press = 0;
        
    }

    if (menu_status.btn_back_press == 1)
    {
        if (menu_status.current_menu != menu_status.prev_menu)
        {
            menu_status.current_menu = menu_status.prev_menu;
            menu_status.selected_option = menu_status.prev_menu_selected_option;
        }
        
        menu_status.btn_back_press = 0; 
    }    
}

static void _MENU_TextSelectorDraw(char *buf, char *text)
{
    strcat(buf, "\xAF\xAF ");
    strcat(buf, text);
    strcat(buf, " \xAE\xAE");
}

// Function for drawing text based menu
static void _MENU_DrawTextMenu(const menu_t *_menu)
{
    for (uint8_t i = 0; i < _menu->options_count; i++)
    {
        if (i == menu_status.selected_option)
        {
            char temp_name[MENU_MAX_OPTION_TEXT_LEN + 6];

            //Clear temp buffer to avoid errors
            memset(temp_name,0,MENU_MAX_OPTION_TEXT_LEN + 6);

            _MENU_TextSelectorDraw(temp_name, (char*)_menu->option[i].text);

            GFX_PrintCentered((MENU_OPTION_V_OFFSET*i), temp_name, WHITE, BLACK, 1);

        }
        else
        {
            GFX_PrintCentered((MENU_OPTION_V_OFFSET*i), (char*)_menu->option[i].text, WHITE, BLACK, 1);
        }
        
        
    }

}

static void _MENU_IconSelectorDraw(void)
{
    switch (menu_status.selected_option)
    {
        case 0:
            GFX_DrawRectangle(0,0,32,32,WHITE);
            break;
        case 1:
            GFX_DrawRectangle(32,0,32,32,WHITE);
            break;
        case 2:
            GFX_DrawRectangle(64,0,32,32,WHITE);
            break;
        case 3:
            GFX_DrawRectangle(96,0,32,32,WHITE);
            break;
        default:
            break;
    }
}

// Function to not repeat same code for diffrent directions
static void _MENU_IconAnimationEnd(void)
{
    menu_status.animation_var = 0;
    menu_status.animation_in_progress = 0;
    menu_status.frames_passed = 0;
    menu_status.prev_selected_option = menu_status.selected_option;

    _MENU_IconSelectorDraw();
}



// Function for drawing icon based menu
static void _MENU_DrawIconMenu(const menu_t *_menu)
{
    // Draw icons
    for (uint8_t i = 0; i < _menu->options_count; i++)
    {
        ssd1306_bitmap(1 + (32 * i), 1, _menu->option[i].info.icon_data, 30, 30, WHITE);
    }

    // Option change ---->
    if (menu_status.selected_option > menu_status.prev_selected_option)
    {
        menu_status.animation_in_progress = 1;

        if (menu_status.frames_passed < 8)
        {
            for (uint8_t i = 1; i < _menu->options_count; i++)
            {
                if (i == menu_status.selected_option)
                {
                    if (menu_status.frames_passed < 4)
                    {
                        GFX_DrawRectangle((i-1) * 32, 0, 32 + menu_status.animation_var, 32, WHITE);
                        menu_status.animation_var += 8;
                        if (menu_status.frames_passed == 3) menu_status.animation_var = 8;
                        
                    }
                    else
                    {
                        GFX_DrawRectangle(((i-1) * 32) + menu_status.animation_var, 0, 32 + 32 - menu_status.animation_var, 32, WHITE);
                        menu_status.animation_var += 8;
                    }
                }
                
            }

            menu_status.frames_passed++;
            
        }
        else
        {
            _MENU_IconAnimationEnd();
        }
        
    }
    // Option change <----
    else if(menu_status.selected_option < menu_status.prev_selected_option)
    {
        menu_status.animation_in_progress = 1;

        if (menu_status.frames_passed < 8)
        {
            for (uint8_t i = 0; i < _menu->options_count - 1; i++)
            {
                if (i == menu_status.selected_option)
                {
                    if (menu_status.frames_passed < 4)
                    {

                        GFX_DrawRectangle((32*(i+1)) - menu_status.animation_var, 0, 32 + menu_status.animation_var, 32, WHITE);
                        menu_status.animation_var += 8;

                        if (menu_status.frames_passed == 3) menu_status.animation_var = 8;
                        
                    }
                    else
                    {
                        GFX_DrawRectangle((32*(i+1)) - 32, 0, 32+32-menu_status.animation_var, 32, WHITE);
                        menu_status.animation_var += 8;
                    }                    
                }

            }

            menu_status.frames_passed++;

        }
        else
        {
            _MENU_IconAnimationEnd();
        }

    }
    else
    {
        _MENU_IconSelectorDraw();
    }
    
}

void MENU_Draw(const menu_t *_menu)
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           

    if (menu_status.task_active != 1)
    {
        _MENU_ProcessNavigation(_menu);

        if (_menu->type == TEXT_MENU)
        {
            _MENU_DrawTextMenu(_menu);
        }
        else
        {
            _MENU_DrawIconMenu(_menu);
        }

        _MENU_ProcessBackOk(_menu);
    }
    else
    {
        switch (menu_status.task_id)
        {
        case INFO_TASK:
            TASK_Info();
            break;

        case SD_INIT_TASK:
            TASK_SDInit();
            break;

        case SD_INFO_TASK:
            TASK_SDInfo();
            break;

        case RECORD_TASK:
            TASK_Record();
            break;

        case TIME_SET_TASK:
            TASK_SetTime();
            break;

        case DATE_SET_TASK:
            TASK_SetDate();
        
        default:
            break;
        }
    }

    if (show_battery_status == 1)
    {
        PWR_ShowBatStatus();
    }
    

}

void _MENU_Change(int id)
{
    _MENU_IconAnimationEnd();
    menu_status.prev_menu = menu_status.current_menu;
    menu_status.prev_menu_selected_option = menu_status.selected_option;
    menu_status.current_menu = id;
    menu_status.selected_option = 0;
}

void _MENU_Task(int id)
{
    //_MENU_IconAnimationEnd();
    menu_status.animation_var = 0;
    menu_status.animation_in_progress = 0;
    menu_status.frames_passed = 0;
    menu_status.task_active = 1;
    menu_status.task_id = id;
}


/*
  _____  _    ____  _  __    _____ _   _ _   _  ____ _____ ___ ___  _   _ ____  
 |_   _|/ \  / ___|| |/ /   |  ___| | | | \ | |/ ___|_   _|_ _/ _ \| \ | / ___| 
   | | / _ \ \___ \| ' /    | |_  | | | |  \| | |     | |  | | | | |  \| \___ \ 
   | |/ ___ \ ___) | . \    |  _| | |_| | |\  | |___  | |  | | |_| | |\  |___) |
   |_/_/   \_\____/|_|\_\   |_|    \___/|_| \_|\____| |_| |___\___/|_| \_|____/ 
                                                                                
*/


void TASK_Info(void)
{
    GFX_PrintCentered(0, "DEVICE INFO", WHITE, BLACK, 1);
    GFX_Print(0,8, "Firmware: 1.0", WHITE, BLACK, 1);
    GFX_Print(0,16, "Microphone type: I2S", WHITE, BLACK, 1);
    GFX_Print(0,24, "Created by max7d3", WHITE, BLACK, 1);


    if (BTN_GetState(BTN_OK) == BTN_STATE_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        menu_status.task_active = 0;

        // Clear buttons status
        BTN_ClearState(BTN_UP);
        BTN_ClearState(BTN_DOWN);
    }
    else if (BTN_GetState(BTN_OK) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        menu_status.task_active = 0;
        // Clear buttons status
        BTN_ClearState(BTN_UP);
        BTN_ClearState(BTN_DOWN);
    }  
}


void TASK_SDInit(void)
{

     if(SDCARD_Mount() == FR_OK)
     {
         sdcard_mounted = 1;
     }
     else
     {
         // try to restart the SD
         sdcard_mounted = 0;
         disk_deinitialize(0);

         if(SDCARD_Mount() == FR_OK)
         {
             sdcard_mounted = 1;
         }
     }
        
  
    if (menu_status.frames_passed < 30)
    {
        if (sdcard_mounted == 1)
        {
            GFX_PrintCentered(12, "SD CARD INIT OK", WHITE, BLACK, 1);
        }
        else
        {
            GFX_PrintCentered(12, "SD CARD INIT ERROR", WHITE, BLACK, 1);
        }

        menu_status.frames_passed++;
        
    }
    else
    {
        menu_status.frames_passed = 0;
        menu_status.task_active = 0;
    }
    
}

void TASK_SDInfo(void)
{
    if (sdcard_mounted == 0)
    {
        GFX_PrintCentered(12, "SD CARD NOT INSERTED", WHITE, BLACK, 1);
    }
    else
    {
        uint32_t free_mb, total_mb, free_percent;
        SDCARD_GetFreeSpace(&free_mb, &total_mb);

        GFX_Print(0,0, "Free space: ", WHITE, BLACK, 1);
        GFX_PrintInt(cursor_x, cursor_y, free_mb, WHITE, BLACK, 1);
        GFX_Print(cursor_x, cursor_y, " MB", WHITE, BLACK, 1);

        GFX_Print(0, 9, "Total space: ", WHITE, BLACK, 1);
        GFX_PrintInt(cursor_x, cursor_y, total_mb, WHITE, BLACK, 1);
        GFX_Print(cursor_x, cursor_y, " MB", WHITE, BLACK, 1);

        free_percent = (free_mb*100) / total_mb;
        free_percent = 100 - free_percent;

        GFX_DrawRectangle(10, 20, 102, 12, WHITE);
        GFX_DrawFiledRectangle(12, 22, free_percent, 8, WHITE);

    }
    
    if (BTN_GetState(BTN_OK) == BTN_STATE_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        menu_status.task_active = 0;
        // Clear buttons status
        BTN_ClearState(BTN_UP);
        BTN_ClearState(BTN_DOWN);
    }
    else if (BTN_GetState(BTN_OK) == BTN_STATE_LONG_PRESSED)
    {
        BTN_ClearState(BTN_OK);
        menu_status.task_active = 0;
        // Clear buttons status
        BTN_ClearState(BTN_UP);
        BTN_ClearState(BTN_DOWN);
    }  

}
