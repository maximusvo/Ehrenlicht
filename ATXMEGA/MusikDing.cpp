/*
TCC1 - System timer
TCC0 - Sampling

DMA1 - ADC
DMA2 - ADC

ADCA
ADCB
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "adc.h"
#include "CFFT.h"
#include "animation.h"
#include "spi.h"
#include "wl_module.h"
#include "nRF24L01.h"

CFFT fft;

volatile uint32_t systick;

#define FLAG_FRAME		0x01
#define FLAG_100HZ		0x02
#define FLAG_DOSAMPLE	0x04
#define FLAG_FFTDONE	0x08
volatile uint8_t flags, adc_state;

#ifdef __APS_DEBUG__
uint32_t aps_tick = 0;
uint8_t _fps = 0, _sps = 0, __fps = 0, __sps = 0;
#endif

// seed for random
unsigned short get_seed() {
	unsigned short seed = 0;
	unsigned short *p = (unsigned short*)(RAMEND + 1);
	extern unsigned short __heap_start;
	
	while(p >= &__heap_start + 1)
		seed ^= *(--p);
	
	return seed;
}

int main(void)
{
	// ext. Clock, activate PLL
	OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	OSC.CTRL |= OSC_XOSCEN_bm;
	while(!(OSC.STATUS & OSC_XOSCRDY_bm));
	
	OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | 2;
	OSC.CTRL |= OSC_PLLEN_bm;
	while(!(OSC.STATUS & OSC_PLLRDY_bm));
	
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_PLL_gc;
	OSC.CTRL &= ~OSC_RC2MEN_bm;
	
	// Systemtimer	alle 1 ms
	systick = 0;
	TCC1.CTRLA = TC_CLKSEL_DIV1_gc;
	TCC1.INTCTRLA = TC_OVFINTLVL_MED_gc;
	TCC1.PER = 31999;
	
	DMA.CTRL = DMA_ENABLE_bm;
	
	_delay_ms(100);
	PORTA.DIRSET |= PIN2_bm;
	
	wl_module_init();						// initialise nRF24L01+ Module
	anim_init();
	adc_init();
	srand(get_seed());

	PORTA.OUT = PIN2_bm;					// give a sign
	_delay_ms(100);							// wait for nRF24L01+ Module
	PORTA.OUT = ~(PIN2_bm);
	
	wl_module_tx_config(wl_module_TX_NR_0);	// Config Module
	
	flags = 0;
	adc_state = ADC_STATE_IDLE;
	
	PMIC.CTRL |= (PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm);
	sei();
	
    while(1)
    {	
		// sampling started
		if(adc_state == ADC_STATE_SAMPLING)
		{
			adc_check();
		}
		
		// every 10 ms
		if(flags & FLAG_100HZ)
		{		
			// TODO: get inputs 
			
			flags &= ~FLAG_100HZ;
		}
		
		// calculate new picture
		else if(flags & FLAG_FRAME)
		{
#ifdef __APS_DEBUG__
			_fps++;
#endif
			anim_frame();
			flags &= ~FLAG_FRAME;
		}
		
		// neue Daten einlesen
		else if((adc_state == ADC_STATE_IDLE) && (flags & FLAG_DOSAMPLE))
		{
			adc_startSampling();
			
			flags &= ~FLAG_DOSAMPLE;
		}
		
		// start calculation
		else if(adc_state == ADC_STATE_SAMPLING_DONE)
		{	
			fft.doFFT();
			adc_state = ADC_STATE_WAIT;
		}
		
		 // calculation
		 else if(adc_state == ADC_STATE_WAIT)
		 {
			if(!fft.doStep())
			{
				flags |= FLAG_FFTDONE;
				adc_state = ADC_STATE_IDLE;
			}
		 }
		 
		 // fft done -> work with it
		 else if(flags & FLAG_FFTDONE)
		 {
#ifdef __APS_DEBUG__
			_sps++;
#endif
			anim_inputData(fft.getLeft(), fft.getRight()); 
			flags &= ~FLAG_FFTDONE;
		 }
		
		else
		{
			// TODO: Sachen mit niedriger Priorität, z.B. Ausgabe auf ein Display
		}
		
#ifdef __APS_DEBUG__
		if(systick >= aps_tick)
		{
			aps_tick = systick + 1000;
			
			__fps = _fps;
			__sps = _sps;
			
			_fps = 0;
			_sps = 0;
		}
#endif
    }
}

#define TIME_FRAME		16 //33
#define TIME_100HZ		10
#define TIME_SAMPLE		50

ISR(TCC1_OVF_vect)
{
	static uint8_t cnt_frame = TIME_FRAME, cnt_100hz = TIME_100HZ;
	static uint16_t cnt_capture = TIME_SAMPLE;
	
	cnt_frame--;
	if(!cnt_frame)
	{
		PORTF.OUTTGL = PIN0_bm;
		flags |= FLAG_FRAME;
		cnt_frame = TIME_FRAME;
	}
	
	cnt_100hz--;
	if(!cnt_100hz)
	{
		flags |= FLAG_100HZ;
		cnt_100hz = TIME_100HZ;
	}
	
	cnt_capture--;
	if(!cnt_capture)
	{
		flags |= FLAG_DOSAMPLE;
		cnt_capture = TIME_SAMPLE;
	}
	
	systick++;
}

ISR(PORTA_INT0_vect)
{
	uint8_t status;   
    
    // Read wl_module status 
    wl_module_CSN_lo;										// Pull down chip select
    status = spi_fast_shift(NOP);							// Read status register
    wl_module_CSN_hi;										// Pull up chip select
		
	if (status & (1<<TX_DS))								// IRQ: Package has been sent
	{
		wl_module_config_register(STATUSREG, (1<<TX_DS));	// Clear Interrupt Bit
		PTX=0;
	}
		
	if (status & (1<<MAX_RT))								// IRQ: Package has not been sent, send again
	{
		wl_module_config_register(STATUSREG, (1<<MAX_RT));	// Clear Interrupt Bit
		wl_module_CE_hi;									// Start transmission
		_delay_us(10);								
		wl_module_CE_lo;
	}
		
	if (status & (1<<TX_FULL))								// TX_FIFO Full <-- this is not an IRQ
	{
		wl_module_CSN_lo;									// Pull down chip select
		spi_fast_shift(FLUSH_TX);							// Flush TX-FIFO
		wl_module_CSN_hi;									// Pull up chip select
	}
}