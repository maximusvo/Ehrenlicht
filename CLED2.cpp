//ANPASSEN!!!!!!! Ehrenlicht

#include <avr/io.h>
#include <string.h>
#include "CLED2.h"
#include "CLED.h"
#include "CWS2812V2.h"

CWS2812V2 ws2812V2;

// default constructor
CLED2::CLED2()
{
	memset(this->data, 0, sizeof(rgb_t) * LED_NUM);
} //CLED2

// default destructor
CLED2::~CLED2()
{
} //~CLED2

void CLED2::update()
{
	ws2812V2.input((uint8_t*)this->data, LED_NUM * 3);
	ws2812V2.transfer();
}

void CLED2::setLED_RGB(uint8_t y, rgb_t col)
{
	this->setLED_RGB(y, col.r, col.g, col.b);
}

// definiert Aufbau der Matrix und Farbe der Matrixelemente
void CLED2::setLED_RGB(uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
	if(y > LED_NUM)
	{
		return;
	}

	else		
	{
		this->data[y].r = r;
		this->data[y].g = g;
		this->data[y].b = b;
	}
}

void CLED2::setLED_HSV(uint8_t y, uint8_t hue, uint8_t sat, uint8_t val)
{
	uint16_t subhue;
	uint32_t y1, y2, y3;
	rgb_t rgb;
	
	if(y > LED_NUM)
	{
		return;
	}
	
	if(sat == 0)
	{
		rgb.r = val;
		rgb.g = val;
		rgb.b = val;
		this->setLED_RGB(y, rgb);
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
			rgb.r = val;
			rgb.g = y3;
			rgb.b = y1;
		break;
		
		case 1:
			rgb.r = y2;
			rgb.g = val;
			rgb.b = y1;
		break;
		
		case 2:
			rgb.r = y1;
			rgb.g = val;
			rgb.b = y3;
		break;
		
		case 3:
			rgb.r = y1;
			rgb.g = y2;
			rgb.b = val;
		break;
		
		case 4:
			rgb.r = y3;
			rgb.g = y1;
			rgb.b = val;
		break;
		
		default:
			rgb.r = val;
			rgb.g = y1;
			rgb.b = y2;
		break;
	}
	
	this->setLED_RGB(y, rgb);
}

rgb_t *CLED2::getLED(uint8_t y)
{
	if(y > LED_NUM)
	{
		return NULL;
	}

	return &this->data[y];
}

uint8_t CLED2::isBusy()
{
	return ws2812V2.isBusy();
}

void CLED2::clear()
{
	memset(this->data, 0, sizeof(rgb_t) * LED_NUM);
}
