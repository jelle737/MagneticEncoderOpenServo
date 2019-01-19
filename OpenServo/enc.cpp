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
#include "swspi.h"
#include "storage.h"

uint16_t offset;

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
    //measure position without offset
    offset = 0<<11;
    uint16_t position = enc_get_position_value();
    position >>= 6;
    //check current position against previous position and calculate offset
    uint16_t prev_position = reg_stor_value;
    offset = ((prev_position+16-position)/32)<<11;
}

uint16_t enc_get_position_value(void) // (Revol+1)*2048+angle/16 -> 0.2048..4096.y for x.0°..360°.y
{
    struct serial_interface_unit_s siu;
    uint16_t ret = offset;
    ret += _reg_read_serial_interface_unit(&siu) ? (((siu.revol)<<11)|((siu.angle>>4)&0x1fff)) : 0xFFFF;
    return ret;
}
