# MagneticEncoderOpenServo
A programmable servo capable of turning a defined 1 turn 360° (or more)

## Programmer

Code to run on an arduino nano to allow Arduino IDE to program the servo microcontroller with special bootloader over it's servo plug using 1 signal wire.
Softserial 1-wire code is based on the optiboot sofserial code with lots of trail and tweaks, and requires some special settings to compile within the arduino IDE.
The programmer uses it's regular RX and TX lines to communicate over usb with the computer/avrdude. It relays this data to PB0 (pin 8) on the microcontroller to interact with the servo (or µcontroller with custom bootloader).

A little bit more on it's development struggles can be read here: https://forum.arduino.cc/index.php?topic=625311.0

## Compile and install programmer

It should compile within arduino IDE with minor settings change within the IDE to allow for compilation.
The code has to be burned straight on the programmer chip, overwriting the bootloader on that chip as to not trigger that bootloader when uploading code to the servo µcontroller.
That code could look something like:
```
avrdude -c USBasp -p m328p  -v -e -U lock:w:0x3F:m -U flash:w:programmer.hex -U lock:w:0x0F:m
```

## Open Source

This fork is software that is **open source** and is available free of charge without warranty to all users.

The license is GPL3.