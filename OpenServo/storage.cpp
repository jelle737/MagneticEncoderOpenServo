// storage.cpp
//
// Software SPI for OpenServo
//
// Copyright (C) 2018  Jelle L
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

#include <inttypes.h>
#include <avr/eeprom.h>

#define MIN_INDEX 0x100
#define MAX_INDEX 0x200
uint16_t index;
uint16_t reg_stor_value;

void storage_registers_defaults(void){
    //initialize all registers
    for(int i=MIN_INDEX; i<MAX_INDEX; i++){
        if(eeprom_read_byte(i)!=0){
            eeprom_update_byte(i, 0);
        }
    }
    //write good first value to register, should only happen once in lifetime of servo
    eeprom_write_byte(MIN_INDEX, 32);
}

void storage_init(void){
    index = MIN_INDEX;
    //find index find first non empty value
    while(eeprom_read_byte(index)==0){
        index++;
    }
    
    reg_stor_value = eeprom_read_byte(index);
}

void storage_update(uint16_t position){
    position >>= 6;
    if(position < reg_stor_value-1 || position > reg_stor_value+1 ){
        eeprom_write_byte(index++, 0);
        if(index>=MAX_INDEX)index=MIN_INDEX;
        eeprom_write_byte(index, (uint8_t) position);
        reg_stor_value = position;
    }
}
