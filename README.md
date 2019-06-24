# MagneticEncoderOpenServo
A programmable servo capable of turning a defined 1 turn 360° (or more)

Video on youtube:

[![1 turn 360° servo](http://img.youtube.com/vi/oHWwntjVp9A/0.jpg)](http://www.youtube.com/watch?v=oHWwntjVp9A "1 turn 360° servo")

* A programmable servo, based on ATMEGA328P, TLE5012B, MC34931 or DRV8837, using the arduino enviroment
* It's a defined travel servo, capable of going beyond the limits that a potentiometer has, without adding extra gears. It uses a magnet encoder to read it's position instead of a potentiometer.

More information and images are to be found [on this rc groups thread](https://www.rcgroups.com/forums/showthread.php?3154439-1-turn-360%C2%B0-servo "1 turn 360° servo").

## Software

The software programmed as is has a travel of 376° between the control pulses of 1000µs and 2000µs. The pulses can be wider but *as is* it is limited between around 800µs and 2200µs. providing a travel of more then 450°.

The software can be reprogrammed to do the following:
* Become a continious rotating servo.
* Be a multiple turn servo, like a 10 turn sail winch servo.
* Be current limited, the hardware is capable but not software implemented.
* Have a speed limited movement.
* Be a digital classic servo with a travel of just 90° or 180°.
* Be programmed as time lapse very slow turning servo since there are 2^15 steps of precision.
* Be programmed to accept commands instead of a PWM signal, or have different interaction interfaces.

Dificulty of implementing varies on the topic.

Possible drawbacks:
* Since it uses a magnet, it might be interfered with by an external magnet.
* If the shaft is turned >180° degrees while unpowered the servo will skip a turn, it should not be turned >90° when powered down to prevent for this unwanted behaviour.
* Since there is no endstop there is a software implementation to catch where in a turn it was depowered. This uses EEPROM which has a 100000 write cycle. In my software implementation every 1/8th of a circle the position is saved. This is done in a ring memory containing 256 bytes and requires the old value to be erased (written with 0) and the next position to be written with the new position value. This means technically after 1600000 turns the atmega chips EEPROM might stop working correctly. If you succeed in breaking the EEPROM there are software solutions to prolong the atmega328p's life.

Possible improvements:
* In a fast movement the servo has a tendency to overshoot it's target value. At this moment a P&D is implemented in the PID, but I feel like the I could help out to not overshoot the target and have a more gracefull reach to the target value.
* Code could be improved to integrated safety features like current limiting.
* Code could be cleaned up.

## Hardware

The required parts next the pcb's and it's components are:
* A donor servo for it's housing, gears, motor and shaft of the potentiometer.
* A Diametrically Magnetised round magnet, hard to find can possibly be substitued by a cube magnet.
* Basic components of the hardware pcb are: ATMEGA328P, TLE5012B, MC34931 or DRV8837

## Open Source

This fork is software that is **open source** and is available free of charge without warranty to all users.

The license is GPL3.

## Project/Fork History

Based on OpenServo: https://github.com/ginge/OpenServo