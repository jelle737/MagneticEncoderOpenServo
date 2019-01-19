#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "pwm.h"
#include "pid.h"
#include "pulsectl.h"
#include "swspi.h"
#include "timer.h"
#include "enc.h"
#include "storage.h"

#define EEPROM_VERSION 1

int16_t pwm;
int16_t position;

void setup() {
    if(eeprom_read_byte(0) !=  EEPROM_VERSION){
        storage_registers_defaults();
        eeprom_write_byte(0, EEPROM_VERSION);
    }
    pwm_init();
    storage_init();
    swspi_init();
    enc_init();
    pulse_control_init(enc_get_position_value());
    pid_init(reg_seek_position);
    timer_init();
    sei();
    pwm_enable();
}

void loop() {
    pulse_control_update();
  
    // Get the new position value.
    position = (int16_t) enc_get_position_value(); //0.2048..4096.y for x.0°..360°.y
  
    // Call the PID algorithm module to get a new PWM value.
    pwm = pid_position_to_pwm(position);
  
    // Update the servo movement as indicated by the PWM value.
    // Sanity checks are performed against the position value.
    pwm_update(position, pwm);
  
    // Store position just in case of power loss
    storage_update(position);
}
