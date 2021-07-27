#pragma once

unsigned long millis_();
unsigned long millis();
unsigned long millis_rtc_();
unsigned long millis_rtc();
unsigned long micros();
void yield();

void timer_init();

extern volatile uint8_t tc_flag;
