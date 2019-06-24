// swspi.c
//
// Software SPI for OpenServo
//
// Copyright (C) 2009-2010  Darius Rad <alpha@area49.net>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include "swspi.h"

//Check CRC on transmission enable '1', disable '0'
#define CRCCHECK 1

// SS is D6 - PD6
#define _SS_PORTx  PORTD
#define _SS_DDRx  DDRD
#define _SS_PINx  PIND
#define _SS_BIT  (1<<6)

// SCL is D13 - PB5
#define _SCL_PORTx  PORTB
#define _SCL_DDRx  DDRB
#define _SCL_PINx  PINB
#define _SCL_BIT  (1<<5)

// MISO is D12 - PB4
#define _MISO_PORTx  PORTB
#define _MISO_DDRx  DDRB
#define _MISO_PINx  PINB
#define _MISO_BIT  (1<<4)

// MOSI is D11 - PB3
#define _MOSI_PORTx  PORTB
#define _MOSI_DDRx  DDRB
#define _MOSI_PINx  PINB
#define _MOSI_BIT  (1<<3)


static void _MOSI_low(void)
{
    _MOSI_PORTx &= ~_MOSI_BIT;
}

static void _MOSI_high(void)
{
    _MOSI_PORTx |= _MOSI_BIT;
}

static bool _MISO_read(void)
{
    return _MISO_PINx & _MISO_BIT ? true : false;
}

static void _SCL_low(void){
    _SCL_PORTx &= ~_SCL_BIT;
}

static void _SCL_high(void){
    _SCL_PORTx |= _SCL_BIT;
}

static void _SS_low(void){
   _SS_PORTx &= ~_SS_BIT;
}

static void _SS_high(void){
    _SS_PORTx |= _SS_BIT;
}

static void _start(void)
{
    _SS_low();
}

static void _stop(void)
{
    _SS_high();
}

static void _sendbit(bool data)
{
    _SCL_high();

    if (data)
    {
        _MOSI_high();
    }
    else
    {
        _MOSI_low();
    }
    asm("nop\n");
    _SCL_low();
}

static bool _recvbit(void)
{
    _SCL_high();
    asm("nop\n");
    _SCL_low();

    return _MISO_read();
}

static void _sendbyte(uint8_t data)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        _sendbit(data & 0x80 ? true : false);
        data <<= 1;
    }
}


static uint8_t _recvbyte()
{
    unsigned char data = 0;
    int i;

    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= _recvbit() ? 1 : 0;
    }
    return data;
}

void swspi_init(void)
{
    // set SCL output
    _SCL_DDRx |= _SCL_BIT;
    _SCL_low();

    // set SS output
    _SS_DDRx |= _SS_BIT;
    _SS_high();

    // set MOSI output
    _MOSI_DDRx |= _MOSI_BIT;
    _MOSI_high();

    // set MISO input
    _MISO_DDRx &= ~_MISO_BIT;
    _MISO_PORTx |= _MISO_BIT; //set true enable internal pullup (or turn pin high)

}

#if CRCCHECK
unsigned char CRC8(unsigned char *message, unsigned char Bytelength)
{
    //“crc” defined as the 8-bits that will be generated through the message till the
    //final crc is generated. In the example above this are the blue lines out of the
    //XOR operation.
    unsigned char crc;
    //“Byteidx” is a counter to compare the bytes used for the CRC calculation
    unsigned char Byteidx, Bitidx;
    //Initially the CRC remainder has to be set with the original seed (0xFF for the TLE5012B).
    crc = 0xFF;
    //For all the bytes of the message.
    for(Byteidx=0; Byteidx<Bytelength; Byteidx++)
    {
        //“crc” is calculated as the XOR operation from the previous “crc” and the “message”.
        //“^” is the XOR operator.
        crc ^= message[Byteidx];
        //For each bit position in a 8-bit word
        for(Bitidx=0; Bitidx<8; Bitidx++)
        {
            //If the MSB of the “crc” is 1(with the &0x80 mask we get the MSB of the crc).
            if((crc&0x80)!=0)
            {
                //“crc” advances on position (“crc” is moved left 1 bit: the MSB is deleted since it
                //will be cancelled out with the first one of the generator polynomial and a new bit
                //from the “message” is taken as LSB.)
                crc <<=1;
                //“crc” is calculated as the XOR operation from the previous “crc” and the generator
                //polynomial (0x1D for TLE5012B). Be aware that here the x8 bit is not taken since
                //the MSB of the “crc” already has been deleted in the previous step.
                crc ^= 0x1D;
            }
            //In case the crc MSB is 0.
            else
            //“crc” advances one position (this step is to ensure that the XOR operation is only
            //done when the generator polynomial is aligned with a MSB of the message that is “1”.
            crc <<= 1;
        }
    }
    //Return the inverted “crc” remainder(“~” is the invertion operator). An alternative
    //to the “~” operator would be a XOR operation between “crc” and a 0xFF polynomial.
    return(~crc);
}
#endif

void swspi_save(void)
{
    _start();
    asm("nop\n");
    _stop();
    
}

uint8_t swspi_read(uint16_t addr, void * data)
{
    uint16_t * ptr = data;
    uint8_t count = 0;

#if CRCCHECK
    uint16_t safety_word;
#endif
    
    _start();
    
    /*
    TLE5012B:
    RW [15] 0:Write 1:Read
    Lock [14..11] 4-bit Lock Value 0000: Def for 0x00 0x04 0x14:15 0x20 0x10
        1010: 0x05:11
    UPD [10] Update-Register Access 0:Acces current values 1: access to values in update buffer
    ADDR [9..4] 6-bit address
    ND [3..0] 4-bit number of data words (if bit set to 0000 no safety word is provided) (reads x registery recursively)

    AVAL (02H) ANG_VAL[14:0] angle value
    ASPD (03H) ANG_SPD[14:0] angle speed
    AREV (04H) REVOL[8:0] counter clocl +1, clockwise -1

    update 'update buffer', CS low 1µS

    real time:
    1_0000_0_000010_0001 = 8021

    real time without crc:
    1_0000_0_000010_0000 = 8020

    update bufer:
    1_0000_1_000010_0001 = 8421 AVAL
    1_0000_1_000100_0001 = 8441 AREV

    */

    _sendbyte((uint8_t) (0x00ff & (addr >> 8)));
    _sendbyte((uint8_t) (0x00ff & addr));

    while (count < (0x000f & addr))
    {
        *ptr = _recvbyte();
        *ptr <<= 8;
        *ptr++ |= _recvbyte();
        count++;
    }
    
#if CRCCHECK
    ptr = &safety_word;
#endif

    while (count <= (0x000f & addr))
    {
        *ptr = _recvbyte();
        *ptr <<= 8;
        *ptr++ |= _recvbyte();
        count++;
    }

#if CRCCHECK
    uint16_t *ptr2 = data;
    uint8_t message[4];
    message[0]=addr >> 8;
    message[1]=addr;
    message[2]=ptr2[0] >> 8;
    message[3]=ptr2[0];
#endif
    
    _stop();
    
#if CRCCHECK
    return ((safety_word&0x00ff) == CRC8(message, 4));
#else
    return true;
#endif
}
















uint8_t swspi_write(uint8_t addr, const void * data)
{
    uint8_t * ptr = data;
    int count = 0;

#if CRCCHECK
//    uint8_t safety_word[2];
    uint16_t safety_word;
#endif

    _start();

    _sendbyte((uint8_t) (0x00ff && (addr >> 8)));
    _sendbyte((uint8_t) (0x00ff && addr));

    while (count < 1)
    {
        _sendbyte(*ptr++);
        _sendbyte(*ptr++);
        count++;
    }
    
#if CRCCHECK
//    ptr = safety_word;
    ptr = (uint8_t*)&safety_word;
#endif

    while (count <= 0x000f && addr)
    {
        *ptr++ = _recvbyte();
        *ptr++ = _recvbyte();
        count++;
    }

#if CRCCHECK
    uint16_t *ptr2 = data;
    uint32_t message = (addr << 16 ) + *ptr2;
#endif
    
    _stop();

#if CRCCHECK
//    return ((uint8_t)(safety_word&&0x00ff) == CRC8(message, 4));
    _sendbyte((uint8_t) CRC8(message, 4));
    return true;
#else
    return true;
#endif

}
