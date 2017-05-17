/*
USART C1 & DMA channel 0
PE5: CLK
PE6: RX
PE7: TX
*/
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "CWS2812V2.h"

// default constructor
CWS2812V2::CWS2812V2()
{
	// USART 1 PORT E
	// ports setzen, TX invertieren, Master SPI
	PORTE.DIRSET = PIN5_bm | PIN7_bm;
	PORTE.DIRCLR = PIN6_bm;
	
	PORTE.PIN7CTRL = PORT_INVEN_bm;
	
	USARTE1.CTRLA = 0;
	USARTE1.CTRLB = USART_TXEN_bm;
	USARTE1.CTRLC = USART_CMODE_MSPI_gc;
	USARTE1.BAUDCTRLA = 3;					// 16MHz: 1					4
	USARTE1.BAUDCTRLB = (-1 << 4);			// 16MHz: (-3 << 4);		0
	
	// DMA channel 3
	DMA.CH3.ADDRCTRL = DMA_CH_SRCRELOAD_TRANSACTION_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_NONE_gc | DMA_CH_DESTDIR_FIXED_gc;
	DMA.CH3.TRIGSRC = DMA_CH_TRIGSRC_USARTE1_DRE_gc;
	DMA.CH3.CTRLA = DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_1BYTE_gc;
	
	DMA.CH3.DESTADDR0 = ((uint16_t)&USARTE1.DATA) & 0xFF;
	DMA.CH3.DESTADDR1 = ((uint16_t)&USARTE1.DATA >> 8) & 0xFF;
	DMA.CH3.DESTADDR2 = 0;
	
	memset(this->data, WS2812_LOW, WS2812_DATASIZE);
} //CWS2812V2

// default destructor
CWS2812V2::~CWS2812V2()
{
} //~CWS2812V2

void CWS2812V2::transfer()
{
	DMA.CH3.SRCADDR0 = ((uint16_t)this->data) & 0xFF;
	DMA.CH3.SRCADDR1 = ((uint16_t)this->data >> 8) & 0xFF;
	DMA.CH3.SRCADDR2 = 0;
	
	DMA.CH3.TRFCNT = WS2812_DATASIZE;
	DMA.CH3.CTRLA |= DMA_CH_ENABLE_bm;
}

void CWS2812V2::input(uint8_t *led_data, uint16_t len)
{
	uint8_t i, b;
	uint16_t dpos;
	
	dpos = 0;
	
	for(i = 0; i < len; i++)
	{
		for(b = 0x80; b; b >>= 1)
		{	
			if(led_data[i] & b)
			{
				this->data[dpos] = WS2812_HIGH;
			}
			else
			{
				this->data[dpos] = WS2812_LOW;
			}
			
			dpos++;
		}
	}
}

uint8_t CWS2812V2::isBusy()
{	
	if((DMA.CH3.CTRLB & DMA_CH_CHBUSY_bm) || (DMA.CH3.CTRLA & DMA_CH_ENABLE_bm))
	{
		return 1;
	}
	
	return 0;
}