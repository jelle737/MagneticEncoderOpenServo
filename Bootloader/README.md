# MagneticEncoderOpenServo
A programmable servo capable of turning a defined 1 turn 360° (or more)

## Bootloader

Minimal changes to the original bootloader to provide the functionality of softserial programming over 1 wire.

Some notheworty changes:
* 'output' and 'input' pin_def changes to same pin PB0 (pin 8).
* Usage of softserial, was already implemented.
* Change pinstate between tristate and output.
* Switch the reset flag with input state, high state is likely program state (reset flag), low state is likely pwm input and goes directly to program (no reset flag).
* If input high (likely to progam device) boot delay is 4 second.

## Compile & install bootloader

The bootloader can be compiled with the following command line: 
```
make LED_START_FLASHES=0 SOFT_UART=1 atmega328
```
The 4 seconds boot delay in case of a high signal can be changed in the code.

Upload the bootloader hex using a usbasp device with the following command:
Erase chip, unlock bootloader section and set fuses for clock and bootloader size
```
avrdude -c USBasp -p m328p  -v -e -U lock:w:0x3F:m -U efuse:w:0xFD:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m
```
Program chip and lock bootloader section:
```
avrdude -c USBasp -p m328p  -v -e -U flash:w:bootloader.hex -U lock:w:0x0F:m
```

## Open Source

This fork is software that is **open source** and is available free of charge without warranty to all users.

The license is GPL3.

## Project/Fork History

Based on optiboot bootloader (by WestfW): https://github.com/Optiboot/optiboot