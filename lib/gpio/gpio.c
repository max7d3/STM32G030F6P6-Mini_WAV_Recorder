#include "stm32g0xx.h"


#include "gpio.h"


/*
*   Easy GPIO config almost like HAL
*/

static void _PortCfg(GPIO_TypeDef *gpio_port, gpio_pin_t pin, gpio_mode_t mode, gpio_speed_t speed)
{
    switch (mode)
    {
        case GPIO_MODE_INPUT:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            break;

        case GPIO_MODE_INPUT_PULLUP:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 1 << (pin * 2);
            break;

        case GPIO_MODE_INPUT_PULLDOWN:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 2 << (pin * 2);
            break;

        case GPIO_MODE_OUTPUT:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 1 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            gpio_port->OTYPER = (gpio_port->OTYPER & ~(1 << pin)) | 0 << pin;
            break;

        case GPIO_MODE_OUTPUT_OPEN_DRAIN:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 1 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            gpio_port->OTYPER = (gpio_port->OTYPER & ~(1 << pin)) | 1 << pin;
            break;

        case GPIO_MODE_ALTERNATE:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 2 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            gpio_port->OTYPER = (gpio_port->OTYPER & ~(1 << pin)) | 0 << pin;
            break;

        case GPIO_MODE_ALTERNATE_OPEN_DRAIN:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 2 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            gpio_port->OTYPER = (gpio_port->OTYPER & ~(1 << pin)) | 1 << pin;
            break;

        case GPIO_MODE_ALTERNATE_OPEN_DRAIN_PULLUP:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 2 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 1 << (pin * 2);
            gpio_port->OTYPER = (gpio_port->OTYPER & ~(1 << pin)) | 1 << pin;
            break;

        case GPIO_MODE_ANALOG:
            gpio_port->MODER = (gpio_port->MODER & ~(0b11 << (pin*2))) | 3 << (pin * 2);
            gpio_port->PUPDR = (gpio_port->PUPDR & ~(0b11 << (pin*2))) | 0 << (pin * 2);
            gpio_port->OTYPER = (gpio_port->OTYPER & ~(1 << pin)) | 0 << pin;
            break;
        }

    gpio_port->OSPEEDR |= speed << (pin * 2);

}

// Set pointer to correct register
static GPIO_TypeDef *_GPIO_SwitchPort(gpio_port_t port)
{
    switch (port)
    {
    case GPIO_PORTA:
        return GPIOA;

    case GPIO_PORTB:
        return GPIOB;

    default:
        return 0;
    
    }

}


// Configure GPIO Pin

void GPIO_Config(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode, gpio_speed_t speed)
{
    GPIO_TypeDef *GPIOx = _GPIO_SwitchPort(port); // GPIO Register type

    _PortCfg(GPIOx, pin, mode, speed);

}

// Enable GPIOs clock
void GPIO_Init(void)
{
    RCC->IOPENR = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
}

// Set GPIO pin state
void GPIO_SetState(gpio_port_t port, gpio_pin_t pin, gpio_state_t state)
{
    GPIO_TypeDef *GPIOx = _GPIO_SwitchPort(port);

    if(state == LOW)
    {
        GPIOx->BSRR = (1 << pin) << 16;
    }
    else
    {
        GPIOx->BSRR = (1 << pin);
    }
    
}

gpio_state_t GPIO_GetState(gpio_port_t port, gpio_pin_t pin)
{
    GPIO_TypeDef *GPIOx = _GPIO_SwitchPort(port);

    // volatile temp = ((GPIOx->IDR >> pin) & 1);

    // temp = temp;

    return ((GPIOx->IDR >> pin) & 1);

}