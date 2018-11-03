#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "watchdog.h"
#include "config.h"
#include "registers.h"
#include "eeprom.h"
#include "pwm.h"
#include "pid.h"
#include "pulsectl.h"
#include "swspi.h"
#include "timer.h"
#include "enc.h"
#include "storage.h"






static void config_pin_defaults(void)
// Configure pins to their default states to conform to recommendation that all
// AVR MCU pins have a defined level.  We do this by configuring unused pins
// as inputs and enabling the internal pull-ups.
{
    // Configure unused port B pins as inputs (0/false) and enable internal pull-up resistor (1/true).
   /* DDRB = (0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) |
           (0<<DDB3) | (1<<DDB2) | (1<<DDB1) | (0<<DDB0);   //  |PWMx|PWMy|PWM pluse in
    PORTB = (1<<PORTB7) | (1<<PORTB6) | (1<<PORTB5) | (1<<PORTB4) |
            (1<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (1<<PORTB0);

    // Configure unused port C pins as inputs and enable internal pull-up resistor.
    DDRC = (0<<DDC6) | (0<<DDC5) | (0<<DDC4) |
           (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
    PORTC = (1<<PORTC6) | (1<<PORTC5) | (1<<PORTC4) |
            (1<<PORTC3) | (1<<PORTC2) | (1<<PORTC1) | (1<<PORTC0);

    // Configure port D pins as inputs and enable internal pull-up resistor.
    DDRD = (0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) |
           (1<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0); //  HBRIDGE ENABLE PIN| | | 
    PORTD = (1<<PORTD7) | (1<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) |
            (0<<PORTD3) | (1<<PORTD2) | (1<<PORTD1) | (1<<PORTD0);*/
}

//uint16_t i;
int16_t pwm;
int16_t position;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600); //**//
  //Serial.println("");
  //Serial.print("A"); //**//
  //delay(100);
//  i=0;
  
  config_pin_defaults();
  //Serial.print("B");
  //delay(100);
  // Initialize the watchdog module.
  watchdog_init();
  //Serial.print("C");
  //delay(100);
  // First, initialize registers that control servo operation.
  registers_init();
  //Serial.print("D");
  //delay(100);
  // Initialize the PWM module.
  pwm_init();
  //Serial.print("E");
  //delay(100);
  // Initialize the PID algorithm module.
  pid_init();
  //Serial.print("F");
  //delay(100);
  storage_init();

  swspi_init();

  enc_init();
  
  enc_get_position_value();
  
  registers_write_word(REG_SEEK_POSITION_HI, REG_SEEK_POSITION_LO, enc_get_position_value());
  pulse_control_init();
  //Serial.print("G");
  //delay(100);
  
  //Serial.print("H");
  //delay(100);
  // Finally initialize the timer.
  timer_set(0);
  //Serial.print("I");
  //delay(100);
  // Enable interrupts.
  sei();
  //Serial.print("J");
  //delay(100);
  // Set the initial seek position and velocity.
  
  //registers_write_word(REG_SEEK_VELOCITY_HI, REG_SEEK_VELOCITY_LO, 0);
  //Serial.print("K");
  //delay(100);
  // XXX Enable PWM and writing.  I do this for now to make development and
  // XXX tuning a bit easier.  Constantly manually setting these values to
  // XXX turn the servo on and write the gain values get's to be a pain.
  pwm_enable();
  //Serial.print("L");
  //delay(100);
  registers_write_enable();
  //Serial.print("M");
  //delay(100);

  //uint16_t seek_high = 0;
  //uint16_t seek_low = 0;
  // This is the main processing loop for the servo.  It basically looks
  // for new position, power or TWI commands to be processed.
/*  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(6, OUTPUT);*/
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  if(i%100==0){
    Serial.print("hi ");
    Serial.print(i);
    Serial.print(" ");
    int16_t seek_position = (int16_t) registers_read_word(REG_SEEK_POSITION_HI, REG_SEEK_POSITION_LO);
    Serial.print(seek_position);
    Serial.print(" ");
    Serial.print(position);
    Serial.print(" ");
    Serial.println(pwm);
    delay(100);
  }
  i++;*/
  /*
  digitalWrite(13, HIGH);
  delay(100);  
  digitalWrite(12, HIGH);
  delay(100);  
  digitalWrite(11, HIGH);
  delay(100);  
  digitalWrite(6, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);  
  digitalWrite(12, LOW);
  delay(100);  
  digitalWrite(11, LOW);
  delay(100);  
  digitalWrite(6, LOW);
  delay(100);*/
  // Give pulse control a chance to update the seek position.
  pulse_control_update();

  // Get the new position value.
  position = (int16_t) enc_get_position_value(); //0.2048..4096.y for x.0°..360°.y

  if (position >= 0)  {
    // Call the PID algorithm module to get a new PWM value.
    pwm = pid_position_to_pwm(position);
  
    // Update the servo movement as indicated by the PWM value.
    // Sanity checks are performed against the position value.
    pwm_update(position, pwm);

    // Store position just in case of power loss
    storage_update(position);
  }
}
