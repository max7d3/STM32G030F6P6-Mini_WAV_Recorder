#ifndef _MENU_H_ 
#define _MENU_H_

#define MENU_MAX_OPTION_TEXT_LEN 16
#define MENU_MAX_OPTIONS_COUNT 4

#define MENU_OPTION_V_OFFSET 8

typedef enum
{
  MAIN_MENU,
  SETTINGS_MENU,

}menus_t;

typedef enum
{
  INFO_TASK,
  SD_INIT_TASK,
  SD_INFO_TASK,
  RECORD_TASK,
  TIME_SET_TASK,
  DATE_SET_TASK,

}tasks_t;

typedef enum
{
  TEXT_MENU,
  ICON_MENU

}menu_type_t;

typedef struct 
{
  uint8_t has_next_menu:1;
  uint8_t next_menu_id:5;

  uint8_t has_special_render:1;

  uint8_t *icon_data;
  
}menu_option_info_t;


typedef struct
{
    const char text[MENU_MAX_OPTION_TEXT_LEN];  
    void (*clicked_callback)(int); // Function to call when option is clicked
    void (*rendered_callback)(int); // Function to call when option is rendered e.g for showing value
    menu_option_info_t info;

}menu_option_t;


typedef struct 
{
  uint8_t options_count;

  menu_type_t type;

  menu_option_t option[MENU_MAX_OPTIONS_COUNT];

}menu_t;

typedef struct
{
  uint8_t selected_option;
  uint8_t prev_selected_option;
  uint8_t current_menu;

  uint8_t prev_menu;
  uint8_t prev_menu_selected_option;

  uint8_t btn_ok_press:1;
  uint8_t btn_back_press:1;

  
  uint8_t frames_passed;
  uint8_t animation_var;
  uint8_t animation_in_progress:1;

  uint8_t task_active:1;
  tasks_t task_id;

}menu_status_t;



menu_status_t menu_status;

const menu_t menu[2];


void MENU_Draw(const menu_t *_menu);
void _MENU_Change(int id);
void _MENU_Task(int id);


void TASK_Info(void);
void TASK_SDInit(void);
void TASK_SDInfo(void);

// void TASK_Record(void);

#endif /* _MENU_H_ */