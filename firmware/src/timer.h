#pragma once

unsigned long millis_();
unsigned long millis();
unsigned long millis_rtc_();
unsigned long millis_rtc();
unsigned long micros();
void yield();

void timer_init();

void delay(unsigned long ms);

extern volatile uint8_t tc_flag;
