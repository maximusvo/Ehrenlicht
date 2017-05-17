#ifndef __CLED2_H__
#define __CLED2_H__

typedef struct 
{
	uint8_t r, g, b;
} rgbr_t;

//#define LED_NUM_X	7
//#define LED_NUM_Y	6
#define LEDR_NUM		 60

class CLED2
{
//variables
public:

protected:

private:
	rgbr_t dataR[LEDR_NUM];

//functions
public:
	CLED2();
	~CLED2();
	
	void update();
	
	void setLED_RGB(uint8_t y, rgbr_t col);
	void setLED_RGB(uint8_t y, uint8_t r, uint8_t g, uint8_t b);
	void setLED_HSV(uint8_t y, uint8_t hue, uint8_t sat, uint8_t val);
	void setLED_HSV(rgbr_t *rgb, uint8_t hue, uint8_t sat, uint8_t val);
	
	rgbr_t* getLED(uint8_t y);
	
	void clear();
	
	uint8_t isBusy();
	
protected:

private:
	CLED2( const CLED2 &c );
	CLED2& operator=( const CLED2 &c );

}; //CLED2

#endif //__CLED2_H__
