#ifndef __CLED_H__
#define __CLED_H__

#define LED_NUM		 60
#define DATA_SIZE	 6

class CLED
{
//variables
public:

protected:

private:
	uint8_t data[DATA_SIZE];

//functions
public:
	CLED();
	~CLED();
	
	void update();

	void setLED_HSV(uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, uint8_t valNext);
	
	void clear();
	
	uint8_t isBusy();
	
protected:

private:
	CLED( const CLED &c );
	CLED& operator=( const CLED &c );

}; //CLED

#endif //__CLED_H__
