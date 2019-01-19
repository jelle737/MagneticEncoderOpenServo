/*
    Copyright (c) 2006 Michael P. Thompson <mpthompson@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*https://stackoverflow.com/questions/46573550/atmel-arduino-isrtimer0-ovf-vect-wont-compile-first-defined-in-vector */
#define _DISABLE_ARDUINO_TIMER0_INTERRUPT_HANDLER_
#include <wiring.c>

#include "timer.h"

uint16_t timer;

void timer_init(){
    // Reset timer 0 count values.
    TCNT0 = 0;

    // Set timer/counter0 control register A.
    TCCR0A = (0<<COM0A1) | (0<<COM0A0) |                    // Disconnect OCOA.
             (0<<COM0B1) | (0<<COM0B0) |                    // Disconnect OCOB.
             (0<<WGM01) | (0<<WGM00);                       // Mode 0 - normal operation.

    // Set timer/counter0 control register B.
    TCCR0B = (0<<FOC0A) | (0<<FOC0B) |                      // No force output compare A or B.
             (0<<WGM02) |                                   // Mode 2 - clear timer on compare match.
             (1<<CS02) | (0<<CS01) | (1<<CS00);							// Clk/1024 prescaler -- a 16/1024MHz clock rate.

    // Set the timer/counter0 interrupt masks.
    TIMSK0 = (0<<OCIE0A) |                                  // No interrupt on compare match A.
             (0<<OCIE0B) |                                  // No interrupt on compare match B.
             (1<<TOIE0);                                    // Interrupt on overflow.
             
    // Clear any pending overflow interrupt.
    TIFR0 = (1<<TOV0);

    // set time to 0 for start;
    timer = 0;
}

void timer_set(uint16_t value)
{
    // Set the timer to the specified value.
    timer = value;
}

uint16_t timer_get(void)
{
    // Get the timer value.
    return timer;
}

uint16_t timer_delta(uint16_t time_stamp)
{
    //calculate the delta time, delta is maintained on overflow of reg_time
    return timer - time_stamp;
}

void timer_increment(void)
{
    // Increment the timer value.
    ++timer;
}


ISR(TIMER0_OVF_vect)
// Handles timer/counter0 overflow interrupt.
{
    // Increment the timer.
    timer_increment();
}
