//ANPASSEN!!!!!!! Ehrenlicht

#include <avr/io.h>
#include <string.h>
#include "CLED.h"
#include "CWS2812.h"
//#include "CWS2812V2.h"

CWS2812 ws2812;
//CWS2812V2 ws2812V2;

// default constructor
CLED::CLED()
{
	memset(this->data, 0, sizeof(rgb_t) * LED_NUM);
} //CLED

// default destructor
CLED::~CLED()
{
} //~CLED

void CLED::update()
{
	ws2812.input((uint8_t*)this->data);
	ws2812.transfer();
}

// definiert Aufbau der Matrix und Farbe der Matrixelemente

void CLED::setLED_HSV(uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, uint8_t valNext)
{
	if(y > LED_NUM)
	{
		return;
	}

		else		
	{
		this->data[0] = y;
		this->data[1] = hue;
		this->data[2] = sat;
		this->data[3] = val;
		this->date[4] = valNext;
	}
	
}

rgb_t *CLED::getLED(uint8_t y)
{
	if(y > LED_NUM)
	{
		return NULL;
	}

	return &this->data.y;
}

uint8_t CLED::isBusy()
{
	return ws2812.isBusy();
}

void CLED::clear()
{
	memset(this->data, 0, sizeof(rgb_t) * LED_NUM); //datensatzgröße auf 6bit anpassen
}
