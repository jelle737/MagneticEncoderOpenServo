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
#include <avr/interrupt.h>
#include <avr/io.h>

#include "pwm.h"

// Pwm frequency divider value.
#define DEFAULT_PWM_FREQ_DIVIDER        0x0040

bool reg_pwm_pid_enabled;

// Determine the top value for timer/counter1 from the frequency divider.
#define PWM_TOP_VALUE(div)      ((uint16_t) div << 4) - 1;

// Determines the compare value associated with the duty cycle for timer/counter1.
#define PWM_OCRN_VALUE(div,pwm) (uint16_t) (((uint32_t) pwm * (((uint32_t) div << 4) - 1)) / 255)

// Flags that indicate PWM output in A and B direction.
static uint8_t pwm_a;
static uint8_t pwm_b;

static void pwm_dir_a(uint8_t pwm_duty)
// Send PWM signal for rotation with the indicated pwm ratio (0 - 255).
// This function is meant to be called only by pwm_update.
{
    // Determine the duty cycle value for the timer.
    uint16_t duty_cycle = PWM_OCRN_VALUE(DEFAULT_PWM_FREQ_DIVIDER, pwm_duty);

    // Disable interrupts.
    cli();

    // Do we need to reconfigure PWM output for direction A?
    if (!pwm_a)
    { // Yes...

        // Set EN (PD3) to low.
        PORTD &= ~(1<<PD3);

        // Disable PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) output.
        // NOTE: Actually PWM_A should already be disabled...
        TCCR1A = 0;

        // Make sure PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) are low.
        PORTB &= ~((1<<PB1) | (1<<PB2));

        // Enable PWM_A (PB1/OC1A)  output.
        TCCR1A |= (1<<COM1A1);

        // Set EN (PD3) to high.
        PORTD |= (1<<PD3);

        // Reset the B direction flag.
        pwm_b = 0;
    }

    // Update the A direction flag.  A non-zero value keeps us from
    // recofiguring the PWM output A when it is already configured.
    pwm_a = pwm_duty;

    // Update the PWM duty cycle.
    OCR1A = duty_cycle;
    OCR1B = 0;

    // Restore interrupts.
    sei();
}

static void pwm_dir_b(uint8_t pwm_duty)
// Send PWM signal for rotation with the indicated pwm ratio (0 - 255).
// This function is meant to be called only by pwm_update.
{
    // Determine the duty cycle value for the timer.
    uint16_t duty_cycle = PWM_OCRN_VALUE(DEFAULT_PWM_FREQ_DIVIDER, pwm_duty);

    // Disable interrupts.
    cli();

    // Do we need to reconfigure PWM output for direction B?
    if (!pwm_b)
    { // Yes...

        // Set EN (PD3) to low.
        PORTD &= ~(1<<PD3);

        // Disable PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) output.
        // NOTE: Actually PWM_B should already be disabled...
        TCCR1A = 0;

        // Make sure PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) are low.
        PORTB &= ~((1<<PB1) | (1<<PB2));

        // Enable PWM_B (PB2/OC1B) output.
        TCCR1A = (1<<COM1B1);

        // Set EN (PD3) to high.
        PORTD |= (1<<PD3);

        // Reset the A direction flag.
        pwm_a = 0;
    }

    // Update the B direction flag.  A non-zero value keeps us from
    // recofiguring the PWM output B when it is already configured.
    pwm_b = pwm_duty;

    // Update the PWM duty cycle.
    OCR1A = 0;
    OCR1B = duty_cycle;

    // Restore interrupts.
    sei();
}

void pwm_init(void)
// Initialize the PWM module for controlling a DC motor.
{
    // Set EN (PD3) to low.
    PORTD &= ~(1<<PD3);

    // Enable PD3 as outputs.
    DDRD |= (1<<DDD3);

    // Set PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) are low.
    PORTB &= ~((1<<PB1) | (1<<PB2));

    // Enable PB1/OC1A and PB2/OC1B as outputs.
    DDRB |= ((1<<DDB1) | (1<<DDB2));

    // Reset the timer1 configuration.
    TCNT1 = 0;
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1C = 0;
    TIMSK1 = 0;

    // Set timer top value.
    ICR1 = PWM_TOP_VALUE(DEFAULT_PWM_FREQ_DIVIDER);

    // Set the PWM duty cycle to zero.
    OCR1A = 0;
    OCR1B = 0;

    // Configure timer 1 for PWM, Phase and Frequency Correct operation, but leave outputs disabled.
    TCCR1A = (0<<COM1A1) | (0<<COM1A0) |                    // Disable OC1A output.
             (0<<COM1B1) | (0<<COM1B0) |                    // Disable OC1B output.
             (0<<WGM11) | (0<<WGM10);                       // PWM, Phase and Frequency Correct, TOP = ICR1
             
    TCCR1B = (0<<ICNC1) | (0<<ICES1) |                      // Input on ICP1 disabled.
             (1<<WGM13) | (0<<WGM12) |                      // PWM, Phase and Frequency Correct, TOP = ICR1
             (0<<CS12) | (0<<CS11) | (1<<CS10);             // No prescaling.
}


void pwm_update(uint16_t position, int16_t pwm)
// Update the PWM signal being sent to the motor.  The PWM value should be
// a signed integer in the range of -255 to -1 for clockwise movement,
// 1 to 255 for counter-clockwise movement or zero to stop all movement.
// This function provides a sanity check against the servo position and
// will prevent the servo from being driven past a minimum and maximum
// position.
{
    uint8_t pwm_width;

    // Disable clockwise movements when position is below the minimum position.
    if ((position < MIN_POSITION) && (pwm < 0)) pwm = 0;

    // Disable counter-clockwise movements when position is above the maximum position.
    if ((position > MAX_POSITION) && (pwm > 0)) pwm = 0;

    // Determine if PWM is disabled in the registers.
    if (!reg_pwm_pid_enabled) pwm = 0;

    // Determine direction of servo movement or stop.
    if (pwm < 0)
    {
        // Less than zero. Turn clockwise.

        // Get the PWM width from the PWM value.
        pwm_width = (uint8_t) -pwm;

        // Turn clockwise.
        pwm_dir_a(pwm_width);
    }
    else if (pwm > 0)
    {
        // More than zero. Turn counter-clockwise.
        // Get the PWM width from the PWM value.
        pwm_width = (uint8_t) pwm;

        // Turn counter-clockwise.
        pwm_dir_b(pwm_width);
    }
    else
    {
        // Stop all PWM activity to the motor.
        pwm_stop();
    }
}


void pwm_stop(void)
// Stop all PWM signals to the motor.
{
    // Disable interrupts.
    cli();

    // Are we moving in the A or B direction?
    if (pwm_a || pwm_b)
    {
        // Disable PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) output.
        TCCR1A = 0;

        // Make sure that PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) are held low.
        PORTB &= ~((1<<PB1) | (1<<PB2));

        // Hold EN (PD3) low.
        PORTD &= ~(1<<PD3);

        // Reset the A and B direction flags.
        pwm_a = 0;
        pwm_b = 0;
    }

    // Set the PWM duty cycle to zero.
    OCR1A = 0;
    OCR1B = 0;

    // Restore interrupts.
    sei();
}

void pwm_enable(void)
{
    // Enable PWM to the servo motor and PID calculations.
    reg_pwm_pid_enabled = true;
}


void pwm_disable(void)
{
    // Disable PWM to the servo motor and prevent PID windup;
    reg_pwm_pid_enabled = false;

    // Stop now!
    pwm_stop();
}
