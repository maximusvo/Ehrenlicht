/*
 * HSV_ws2812.c
 *
 * Created: 30.05.2016 12:22:20
 *  Author: Maximilian Voigt
 */ 

#include "HSV_ws2812.h"
#include "light_ws2812.h"

struct cRGB led[rgb_led_num];

void sendLED_HSV ()
{
	ws2812_setleds(led, rgb_led_num);
}

void setLED_HSV(uint8_t y, uint8_t hue, uint8_t sat, uint8_t val){
	uint16_t subhue;
	uint32_t y1, y2, y3;
	uint8_t r, g, b;
	
	if(sat == 0)
	{
		r = val;
		g = val;
		b = val;
		led[y].r=r;
		led[y].g=g;
		led[y].b=b;
		return;
	}
	
	if(hue > 251)
	{
		hue = 251;
	}
	
	subhue = (uint16_t)(hue % 42) * 255 / 41;
	hue /= 42;
	
	y1 = (uint32_t)(val * (255 - sat)) / 255;
	y2 = (uint32_t)(val * (255 - (sat * subhue) / 255)) / 255;
	y3 = (uint32_t)(val * (255 - (sat * (255 - subhue)) / 255)) / 255;
	
	switch(hue)
	{
		case 0:
			r = val;
			g = y3;
			b = y1;
		break;
		
		case 1:
			r = y2;
			g = val;
			b = y1;
		break;
		
		case 2:
			r = y1;
			g = val;
			b = y3;
		break;
		
		case 3:
			r = y1;
			g = y2;
			b = val;
		break;
		
		case 4:
			r = y3;
			g = y1;
			b = val;
		break;
		
		default:
			r = val;
			g = y1;
			b = y2;
		break;
	}
	
	led[y].r=r;
	led[y].g=g;
	led[y].b=b;
}