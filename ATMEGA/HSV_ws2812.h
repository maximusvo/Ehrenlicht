/*
 * HSV_ws2812.h
 *
 * Created: 30.05.2016 12:23:30
 *  Author: Maximilian Voigt
 */ 


#ifndef HSV_WS2812_H_
#define HSV_WS2812_H_

#include <avr/io.h>
#include "light_ws2812.h"

#define rgb_led_num 60

void setLED_HSV(uint8_t y, uint8_t hue, uint8_t sat, uint8_t val);
void sendLED_HSV ();


#endif /* HSV_WS2812_H_ */