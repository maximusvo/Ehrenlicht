/*
 * animation.c
 *
 * Created: 12.06.2016 17:57:38
 *  Author: Maximilian Voigt
 */
#include <avr/interrupt.h>
#include "HSV_ws2812.h"
#include "animation.h"
#include "usart.h"
#include "wl_module.h"

#define lamp_size     10
#define MIN(X,Y) (X > Y)?Y:X

volatile struct
{
  uint8_t y_full_soll, hue, sat, val, val_remain;
  uint8_t y_full_ist;
} mode_10;

volatile struct
{
  uint16_t y_val_soll[lamp_size], y_val_ist[lamp_size], y_col_soll[lamp_size], y_col_ist[lamp_size];
  uint8_t temp_data[wl_module_PAYLOAD];
} mode_11;

void animation_init()
{
  animation_clear();
}

void animation_clear()
{
  mode_10.y_full_ist = 0;

  for(uint8_t y = 0; y <= rgb_led_num; y++)
  {
    setLED_HSV(y, 0, 0, 0);
  }
  cli();
  sendLED_HSV ();
  sei();
}

void animation_step(uint8_t payload []) {
  if(payload [1] == 10) //Volume unit mode
  {
    uint8_t match = 0;
    mode_10.y_full_soll =   payload [2];
    mode_10.hue =       payload [3];
    mode_10.sat =       payload [4];
    mode_10.val =       payload [5];
    mode_10.val_remain =    payload [6];

    if(mode_10.y_full_soll <= rgb_led_num)
    {
      if(mode_10.y_full_soll > mode_10.y_full_ist)
      {
        mode_10.y_full_ist = mode_10.y_full_soll;
      }

      else if(mode_10.y_full_soll < mode_10.y_full_ist)
      {
        mode_10.y_full_ist=mode_10.y_full_ist-2;
      }

      for(uint8_t y = 0; y <= rgb_led_num; y++)
      {
        setLED_HSV(y, 0, 0, 0);
      }
            /*
      if(mode_10.y_full_ist < rgb_led_num)
      {
        for(uint8_t y = mode_10.y_full_ist; y <= mode_10.y_full_ist+5; y++)
        {
          setLED_HSV(y, mode_10.hue, mode_10.sat, mode_10.val_remain);
        }
      }*/
      if(mode_10.y_full_ist > 5) {
        while(mode_10.y_full_ist > match) {
          match = match + 6;
        }
      }

      for(uint8_t y = 0; y < match; y++) {
        setLED_HSV(y, mode_10.hue, mode_10.sat, mode_10.val);
      }

      cli();
      sendLED_HSV ();
      sei();
      //payload[0]=mode_10.y_full_ist;
      //uart_Send_Int_Array (payload, 1);
    } else {
      for(uint8_t y = 0; y <= rgb_led_num; y++) {
        setLED_HSV(y, 0, 0, 0);
      }
      cli();
      sendLED_HSV ();
      sei();
    }
  }

  if(payload [1] == 11) //Spectrum mode
  {
    uint8_t dots = 0;
    uint8_t color = payload[12];
    uint8_t addC = color;

    for(uint8_t i = 0; i < lamp_size; i++) {
      mode_11.y_val_soll[i] = payload [i+2];
    }

    for(uint8_t i=0; i <= lamp_size; i++) {

      if (mode_11.y_val_soll[i] > mode_11.y_val_ist[i]) {
        mode_11.y_val_ist[i] = mode_11.y_col_ist[i] = MIN(255, mode_11.y_val_soll[i]*5);
      } else if (mode_11.y_val_ist[i] > 2) {
        mode_11.y_val_ist[i] = mode_11.y_val_ist[i] - 2;
      } else if (mode_11.y_val_ist[i] > 0) {
        mode_11.y_val_ist[i] = mode_11.y_val_ist[i] - 1;
      }

      if(mode_11.y_col_ist[i] >= 1) {
        mode_11.y_col_ist[i] = mode_11.y_col_ist[i] - 1;
      }
    }

    for(uint8_t i = 2; i < lamp_size; i++) {
      for(uint8_t j = dots; j < dots + 6; j++) {
          uint8_t val0 = MIN(255, 40 + mode_11.y_col_ist[0]);
          uint8_t val1 = MIN(255, 40 + mode_11.y_col_ist[1]);
          setLED_HSV(j, color - addC, val0, mode_11.y_val_ist[0]);
          setLED_HSV(j, color - addC, val1, mode_11.y_val_ist[1]);
          setLED_HSV(j, color - addC, 255 - mode_11.y_col_ist[i], mode_11.y_val_ist[i]);
      }
      dots = dots + 6;
      addC = addC - 14;
    }

    cli();
    sendLED_HSV ();
    sei();
    //uart_Send_Int_Array (mode_11.y_val_ist, lamp_size);
  }
}
