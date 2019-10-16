/**
 * SSD1306xLED - Library for the SSD1306 based OLED/PLED 128x64 displays
 *
 * @author Neven Boyanov
 *
 * This is part of the Tinusaur/SSD1306xLED project.
 *
 * Copyright (c) 2018 Neven Boyanov, The Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Retain in your source code the link http://tinusaur.org to the Tinusaur project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

// ============================================================================

// NOTE: About F_CPU - it should be set in either (1) Makefile; or (2) in the IDE.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "tinyavrlib/cpufreq.h"

#include "ssd1306xled/font6x8.h"
#include "ssd1306xled/font8x16.h"
#include "ssd1306xled/ssd1306xled.h"
#include "ssd1306xled/ssd1306xledtx.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                 ATtiny85
//               +----------+   (-)--GND--
//       (RST)---+ PB5  Vcc +---(+)--VCC--
// ---[OWOWOD]---+ PB3  PB2 +---[TWI/SCL]-
// --------------+ PB4  PB1 +-------------
// --------(-)---+ GND  PB0 +---[TWI/SDA]-
//               +----------+
//              Tinusaur Board
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// NOTE: If you want to reassign the SCL and SDA pins and the I2C address
// do that in the library source code and recompile it so it will take affect.

// ----------------------------------------------------------------------------

#define TESTING_DELAY 500

volatile  uint8_t lap_counter;
volatile  uint8_t timer_sec;
volatile  uint8_t timer_min;
volatile  uint8_t timer_hr;
volatile  uint8_t pb2_button_pressed;
volatile  uint8_t pb2_long_press;
volatile  uint8_t pb3_button_pressed;
volatile  uint8_t pb3_long_press;
volatile  uint8_t flip;
volatile  uint16_t ticks;

// ISR(PCINT0_vect)
// {
//  if( (bit_is_clear(PINB, PB2)) || (bit_is_clear(PINB, PB3)) )
//   _delay_ms(50);
//  else
//    exit(0);
//
//  if(bit_is_clear(PINB, PB2))
//    lap_counter++;
//
//  if(bit_is_clear(PINB, PB3))
//    lap_counter=0;
//}


void my_sleep() {

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT2);                   // Use PB3 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT2);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
    } // sleep

ISR(PCINT0_vect) 
  { }

ISR(TIM0_COMPA_vect) {
    ticks++;
    if(ticks > 500)
    {
     ticks = 0;
     if(flip)
     {
      flip = 0;
      ssd1306tx_stringxy(ssd1306xled_font8x16data, 32,6, "long");
     }
    else
     {
      flip = 1;
      ssd1306tx_stringxy(ssd1306xled_font8x16data, 32,6, "    ");
     }
    }
}


int main(void) {

// ---- CPU Frequency Setup ----
#if F_CPU == 1000000UL
#pragma message "F_CPU=1MHZ"
	CLKPR_SET(CLKPR_1MHZ);
#elif F_CPU == 8000000UL
#pragma message "F_CPU=8MHZ"
	CLKPR_SET(CLKPR_8MHZ);
#else
#pragma message "F_CPU=????"
#error "CPU frequency should be either 1 MHz or 8 MHz"
#endif

        char printstr[10];

	// ---- Initialization ----
	_delay_ms(40);	// Small delay might be necessary if ssd1306_init is the first operation in the application.
	ssd1306_init();
	ssd1306tx_init(ssd1306xled_font6x8data, ' ');
        ssd1306_clear();

        // CLKPR = 1<<CLKPCE;
        // CLKPR = 4<<CLKPS0;

        TCCR0A = 2<<WGM00;            // CTC mode; count up to OCR0A
        TCCR0B = 0<<WGM02 | 2<<CS00;  // Divide by 8 = 62500Hz
        OCR0A = 249;                  // Divide by 250 -> 250Hz
        TIMSK = TIMSK | 1<<OCIE0A;    // Enable compare match interrupt

        DDRB = 0b00100011;
        // PORTB &= 1<<PB4;

	// ---- Main Loop ----
	for (;;) {

                 if(bit_is_clear(PINB, PB2) && (!pb2_button_pressed) )
                  {
                   pb2_button_pressed = 1;
                   _delay_ms(100);
                   pb2_long_press++;
                  }
                 else if((bit_is_clear(PINB, PB2)) && pb2_button_pressed)
                  {
                   pb2_long_press++;
                   if(pb2_long_press > 8)
                    {
                    }
                  }
                 else if((!bit_is_clear(PINB, PB2)) && (pb2_button_pressed))
                  {
                   if(pb2_long_press < 3)
                    lap_counter++;
                   pb2_button_pressed = 0;
                   pb2_long_press = 0;
                  } 

                 if(bit_is_clear(PINB, PB3) && (!pb3_button_pressed) )
                  {
                   pb3_button_pressed = 1;
                   _delay_ms(100);
                   pb3_long_press++;
                  }
                 else if((bit_is_clear(PINB, PB3)) && pb3_button_pressed)
                  {
                   pb3_long_press++;
                   if(pb3_long_press > 8)
                    {
                     ssd1306_off();
                     my_sleep();
                     ssd1306_init();
                     //ssd1306tx_stringxy(ssd1306xled_font8x16data, 32,6, "long");
                     //_delay_ms(1000);
                     //ssd1306_clear();
                    }
                  }
                 else if((!bit_is_clear(PINB, PB3)) && (pb3_button_pressed))
                  {
                   if(pb3_long_press < 3)
                    lap_counter = 0;
                   pb3_button_pressed = 0;
                   pb3_long_press = 0;
                  }                    

                sprintf(printstr," - %d - ",lap_counter);
		ssd1306tx_stringxy(ssd1306xled_font8x16data, 32, 4, printstr);
                _delay_ms(100);
	}

	return 0; // Return the mandatory result value. It is "0" for success.
}



// ============================================================================
