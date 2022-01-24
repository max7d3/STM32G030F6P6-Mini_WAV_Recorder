#ifndef _GPIO_H_
#define _GPIO_H_

typedef enum
{
    LOW,
    HIGH

}gpio_state_t;


// Ports enum
typedef enum
{
    GPIO_PORTA,
    GPIO_PORTB,
    GPIO_PORTC,

} gpio_port_t;

// Pins enum
typedef enum
{
    GPIO_PIN0,
    GPIO_PIN1,
    GPIO_PIN2,
    GPIO_PIN3,
    GPIO_PIN4,
    GPIO_PIN5,
    GPIO_PIN6,
    GPIO_PIN7,
    GPIO_PIN8,
    GPIO_PIN9,
    GPIO_PIN10,
    GPIO_PIN11,
    GPIO_PIN12,
    GPIO_PIN13,
    GPIO_PIN14,
    GPIO_PIN15

} gpio_pin_t;

// GPIO modes enum
typedef enum
{
    GPIO_MODE_INPUT,
    GPIO_MODE_INPUT_PULLUP,
    GPIO_MODE_INPUT_PULLDOWN,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_OUTPUT_OPEN_DRAIN,
    GPIO_MODE_ALTERNATE,
    GPIO_MODE_ALTERNATE_OPEN_DRAIN,
    GPIO_MODE_ALTERNATE_OPEN_DRAIN_PULLUP,
    GPIO_MODE_ANALOG

} gpio_mode_t;

typedef enum
{
    GPIO_SPEED_LOW,
    GPIO_SPEED_MEDIUM,
    GPIO_SPEED_HIGH,
    GPIO_SPEED_VERY_HIGH,

} gpio_speed_t;

void GPIO_Config(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode, gpio_speed_t speed);
void GPIO_Init(void);
void GPIO_SetState(gpio_port_t port, gpio_pin_t pin, gpio_state_t state);
gpio_state_t GPIO_GetState(gpio_port_t port, gpio_pin_t pin);


#endif /* _GPIO_H_ */