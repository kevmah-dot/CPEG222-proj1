#include "stm32f4xx.h"
#include <stdbool.h>
#define green_pin 0
#define red_pin 14
#define blue_pin 7
#define butt_pin 13
#define led_port GPIOB
#define butt_port GPIOC
bool butt_lock = false;

void delay(volatile uint32_t count){
    while(count--);
}

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

int main(void){
    register_enable();
    
    
}