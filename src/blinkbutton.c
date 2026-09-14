#include "stm32f4xx.h"
#include <stdbool.h>
#define green_pin 0
#define red_pin 14
#define blue_pin 7
#define butt_pin 13
#define led_port GPIOB
#define butt_port GPIOC
bool butt_lock = false;
bool cur_but;
bool prev_but = false;
unsigned debounce_delay = 50;
bool but_out;
int cur_led = 0;
uint32_t last_interrupt_time = 0;

void delay(volatile uint32_t count){
    while(count--);
}

void debounce(bool btn) {
    unsigned LastDebounce;
    cur_but = btn;
    if (cur_but != prev_but) {
        LastDebounce = millis();
    }
    if ((millis() - LastDebounce) > debounce_delay) {
        if ((butt_lock == true) && (cur_but == false)) {
            butt_lock = false;
        }
        else {
            butt_lock = true;
        }
        prev_but = cur_but;
    }
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

void Systick_Handler(void) {
    if (!butt_lock && but_out) {
        cur_led += 1;
        if (cur_led == 0) {
            led_port->ODR |= (1<<green_pin);
            led_port->ODR &= ~(1<<blue_pin);
            led_port->ODR &= ~(1<<red_pin);
        }
        else if (cur_led == 1) {
            led_port->ODR &= ~(1<<green_pin);
            led_port->ODR |= (1<<blue_pin);
            led_port->ODR &= ~(1<<red_pin);
        }
        else if (cur_led == 2) {
            led_port->ODR &= ~(1<<green_pin);
            led_port->ODR &= ~(1<<blue_pin);
            led_port->ODR |= (1<<red_pin);
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1<<butt_pin)) {
        uint32_t current_time = SysTick->VAL;
        if (current_time - last_interrupt_time > 50) {
            but_out = !but_out;
            last_interrupt_time = current_time;
        }
        EXTI->PR |= (1<<butt_pin);
    } 
}

int main(void){
    register_enable();
    
    
}