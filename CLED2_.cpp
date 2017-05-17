//ANPASSEN!!!!!!! Ehrenlicht

#include <avr/io.h>
#include <string.h>
#include "CLED2.h"
#include "CWS2812V2.h"

CWS2812V2 ws2812V2;

// default constructor
CLED2::CLED2()
{
	memset(this->dataR, 0, sizeof(rgbr_t) * LEDR_NUM);
} //CLED

// default destructor
CLED2::~CLED2()
{
} //~CLED

void CLED2::update()
{
	ws2812V2.input((uint8_t*)this->dataR, LEDR_NUM * 3);
	ws2812V2.transfer();
}

void CLED2::setLED_RGB(uint8_t y, rgbr_t col)
{
	this->setLED_RGB(y, col.r, col.g, col.b);
}

// definiert Aufbau der Matrix und Farbe der Matrixelemente
void CLED2::setLED_RGB(uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
	if(y > LEDR_NUM)
	{
		return;
	}

	else		
	{
		this->dataR[y].r = r;
		this->dataR[y].g = g;
		this->dataR[y].b = b;
	}
}

void CLED2::setLED_HSV(uint8_t y, uint8_t hue, uint8_t sat, uint8_t val)
{
	uint16_t subhue;
	uint32_t y1, y2, y3;
	rgbr_t rgbr;
	
	if(y > LEDR_NUM)
	{
		return;
	}
	
	if(sat == 0)
	{
		rgbr.r = val;
		rgbr.g = val;
		rgbr.b = val;
		this->setLED_RGB(y, rgbr);
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
			rgbr.r = val;
			rgbr.g = y3;
			rgbr.b = y1;
		break;
		
		case 1:
			rgbr.r = y2;
			rgbr.g = val;
			rgbr.b = y1;
		break;
		
		case 2:
			rgbr.r = y1;
			rgbr.g = val;
			rgbr.b = y3;
		break;
		
		case 3:
			rgbr.r = y1;
			rgbr.g = y2;
			rgbr.b = val;
		break;
		
		case 4:
			rgbr.r = y3;
			rgbr.g = y1;
			rgbr.b = val;
		break;
		
		default:
			rgbr.r = val;
			rgbr.g = y1;
			rgbr.b = y2;
		break;
	}
	
	this->setLED_RGB(y, rgbr);
}

rgbr_t *CLED2::getLED(uint8_t y)
{
	if(y > LEDR_NUM)
	{
		return NULL;
	}

	return &this->dataR[y];
}

uint8_t CLED2::isBusy()
{
	return ws2812V2.isBusy();
}

void CLED2::clear()
{
	memset(this->dataR, 0, sizeof(rgbr_t) * LEDR_NUM);
}
