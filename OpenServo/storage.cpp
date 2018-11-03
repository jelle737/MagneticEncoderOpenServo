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

//max value = MAX_POSITION            (4710)

#include <inttypes.h>
//#include <string.h>
//#include <avr/io.h>
#include <avr/eeprom.h>

//#include "openservo.h"
//#include "config.h"
//#include "eeprom.h"
#include "registers.h"

#define MIN_INDEX 0x100
#define MAX_INDEX 0x200

void storage_registers_defaults(void){
    //initialize all registers
    for(int i=MIN_INDEX; i<MAX_INDEX; i++){
        if(eeprom_read_byte(i)!=0){
            eeprom_update_byte(i, 0);
        }
    }
    //write current value to first/a register
    eeprom_write_byte(MIN_INDEX, 32);
}

void storage_init(void){
    uint16_t index = MIN_INDEX;
    //find index find first non empty value
    while(eeprom_read_byte(index)==0){
        index++;
    }

    //uint8_t position = EEPROM.read(index);
    registers_write_word(REG_STOR_INDEX_HI, REG_STOR_INDEX_LO, index);
    registers_write_word(REG_STOR_HI, REG_STOR_LO, eeprom_read_byte(index));
}

//push back uint16_t position 6 times

void storage_update(uint16_t position){
    position >>= 6;
    if(position < registers_read_word(REG_STOR_HI, REG_STOR_LO)-1 || position > registers_read_word(REG_STOR_HI, REG_STOR_LO)+1 ){
        uint16_t index = registers_read_word(REG_STOR_INDEX_HI, REG_STOR_INDEX_LO);
        eeprom_write_byte(index++, 0);
        if(index==MAX_INDEX)index=MIN_INDEX;
        eeprom_write_byte(index, (uint8_t) position);
        registers_write_word(REG_STOR_INDEX_HI, REG_STOR_INDEX_LO, index);
        registers_write_word(REG_STOR_HI, REG_STOR_LO, (uint8_t) position);
    }
}

/*uint16_t storage_read(void){
    //read value on index
    registers_read_word(REG_STOR_HI, REG_STOR_LO);
}*/

void storage_write(uint16_t position){
    //clear current value

    //write new value
    registers_write_word(REG_STOR_HI, REG_STOR_LO, position);
}
