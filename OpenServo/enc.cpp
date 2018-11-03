// enc.c
//
// OpenEncoder driver for OpenServo
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


#include <stdint.h>
#include "enc.h"
#include "config.h"
#include "swspi.h"
#include "registers.h"

struct serial_interface_unit_s
{
    uint16_t angle;
    uint16_t revol;
};

static bool _reg_read_serial_interface_unit(struct serial_interface_unit_s * siu)
{
    uint16_t data;

    swspi_save();
    //bool valid = swspi_read(0x8021, &data);
    bool valid = swspi_read(0x8421, &data);
    siu->angle = data & 0x7fff; //max 32768
    valid &= swspi_read(0x8441, &data);
    siu->revol = data & 0x01ff; //max 512 -> -256 - 255

    return valid;
}

void enc_init(void){
    registers_write_word(REG_OFFSET_HI, REG_OFFSET_LO, 0<<11);
    uint16_t position = enc_get_position_value();
    position >>= 6;
    uint16_t prev_position = registers_read_word(REG_STOR_HI, REG_STOR_LO);
    /*if(((prev_position-position+5)%32)<=10){
        uint16_t offset = ((prev_position+5-position)/32)<<11;
        registers_write_word(REG_OFFSET_HI, REG_OFFSET_LO, offset);
    }else{
        registers_write_word(REG_OFFSET_HI, REG_OFFSET_LO, 1<<11);
    }*/
    uint16_t offset = ((prev_position+16-position)/32)<<11;
    registers_write_word(REG_OFFSET_HI, REG_OFFSET_LO, offset);
}

uint16_t enc_get_position_value(void) // (Revol+1)*2048+angle/16 -> 0.2048..4096.y for x.0°..360°.y
{
    struct serial_interface_unit_s siu;
    unsigned char *raw = (void*)&siu;
    uint16_t ret = registers_read_word(REG_OFFSET_HI, REG_OFFSET_LO);
    ret += _reg_read_serial_interface_unit(&siu) ? (((siu.revol)<<11)|((siu.angle>>4)&0x1fff)) : 0xFFFF;
    //ret = _reg_read_serial_interface_unit(&siu) ? ((siu.angle>>4)&0x1fff) : 0xFFFF;
    //ret = _reg_read_serial_interface_unit(&siu) ? ((siu.angle)&0x7fff) : 0xFFFF;


 /*   registers_write_byte(REG_ENCODER_RAW_0, *raw++);
    registers_write_byte(REG_ENCODER_RAW_1, *raw++);
    registers_write_byte(REG_ENCODER_RAW_2, *raw++);
    registers_write_byte(REG_ENCODER_RAW_3, *raw);*/
    return ret;
}
