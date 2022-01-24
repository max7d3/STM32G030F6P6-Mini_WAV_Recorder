#ifndef _BUTTONS_H_
#define _BUTTONS_H_

typedef enum
{
    BTN_OK,
    BTN_UP,
    BTN_DOWN,
    /* BTN_BACK,*/
    BTN_COUNT

}buttons_names_t;

typedef enum
{
    BTN_STATE_NOT_PRESSED,
    BTN_STATE_PRESSED,
    BTN_STATE_LONG_PRESSED

}buttons_sates_t;

typedef struct 
{
    uint8_t debounce:1;
    uint8_t pressed:1;
    uint8_t now_pressed:1;
    uint8_t long_pressed:1;
    uint8_t long_press_timer:4;
    
}button_status_t;

volatile button_status_t BUTTON_STATUS[3];

void BTN_Init(void);
buttons_sates_t BTN_GetState(buttons_names_t button);
void BTN_ClearState(buttons_names_t button);


gpio_state_t _BTN_CheckState(buttons_names_t button);
void _BTN_EnableIT(buttons_names_t button);

#endif /* _BUTTONS_H_ */