/*
 * animation.h
 *
 * Created: 12.06.2016 17:57:53
 * Author: Maximilian Voigt
 */ 


#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <avr/interrupt.h>
#include "HSV_ws2812.h"

void animation_step(uint8_t payload []);
void animation_init();
void animation_clear();

#endif /* ANIMATION_H_ */