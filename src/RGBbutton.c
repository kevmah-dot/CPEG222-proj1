#include "stm32f4xx.h"
#include <stdbool.h>
#define green_pin 0
#define red_pin 14
#define blue_pin 7
#define butt_pin 13
#define led_port GPIOB
#define butt_port GPIOC

bool butt_lock = false;
unsigned debounce_delay = 50;
int cur_led = 0;
uint32_t last_interrupt_time = 0;
volatile uint32_t ms = 0;

void register_enable(void){
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;//led enable
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;//button enable
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //clock enable

    led_port->MODER &= ~(0x3 << (green_pin * 2));
    led_port->MODER |= (0x1 << (green_pin * 2));//set green

    led_port->MODER &= ~(0x3 << (blue_pin * 2));
    led_port->MODER |= (0x1 << (blue_pin * 2));//set blue

    led_port->MODER &= ~(0x3 << (red_pin * 2));
    led_port->MODER |= (0x1 << (red_pin * 2));//set red
}

void SysTick_Handler(void) {
    ms++;
}
/*
void EXTI15_10_IRQHandler(void) {
    bool raw_butt = (GPIOC->IDR & (1<<butt_pin)) != 0;
    if (raw_butt) {
        if (ms - last_interrupt_time > debounce_delay) {
            last_interrupt_time = ms;
            if (!butt_lock) {
                butt_lock = true;
                cur_led = (cur_led+1)%3;
                if (cur_led == 1) {
                    led_port->ODR |= (1<<green_pin);
                    led_port->ODR &= ~(1<<blue_pin);
                    led_port->ODR &= ~(1<<red_pin);
                }
                else if (cur_led == 2) {
                    led_port->ODR &= ~(1<<green_pin);
                    led_port->ODR |= (1<<blue_pin);
                    led_port->ODR &= ~(1<<red_pin);
                }
                else if (cur_led == 0) {
                    led_port->ODR &= ~(1<<green_pin);
                    led_port->ODR &= ~(1<<blue_pin);
                    led_port->ODR |= (1<<red_pin);
                }
            }
        }
    }
    if (!raw_butt){
        butt_lock = false;
    }
    EXTI->PR |= (1<<butt_pin);
}
*/

int main(void){
    register_enable();
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    EXTI->IMR |= (1 << butt_pin);
    EXTI->RTSR |= (1 << butt_pin);
    EXTI->FTSR |= (1<<butt_pin);

    led_port->ODR &= ~(1<<green_pin);
    led_port->ODR &= ~(1<<blue_pin);
    led_port->ODR |= (1<<red_pin);

    NVIC_EnableIRQ(EXTI15_10_IRQn);
    SysTick_Config(SystemCoreClock/1000);
    while(1) {
        bool raw_butt = (GPIOC->IDR & (1<<butt_pin)) != 0;
        if (raw_butt) {
            if (ms- last_interrupt_time > debounce_delay) {
                last_interrupt_time = ms;
                if (!butt_lock) {
                    butt_lock = true;
                    cur_led = (cur_led + 1)%3;
                    if (cur_led == 0) {
                        led_port->ODR |= (1<<red_pin);
                        led_port->ODR &= ~(1<<green_pin);
                        led_port->ODR &= ~(1<<blue_pin);
                    }
                    if (cur_led == 1) {
                        led_port->ODR |= (1<<green_pin);
                        led_port->ODR &= ~(1<<red_pin);
                        led_port->ODR &= ~(1<<blue_pin);
                    }
                    if (cur_led == 2) {
                        led_port->ODR |= (1<<blue_pin);
                        led_port->ODR &= ~(1<<red_pin);
                        led_port->ODR &= ~(1<<green_pin);
                    }
                }
            }
        }
        if (!raw_butt) {
            butt_lock = false;
        }
    }
    return(0);
}