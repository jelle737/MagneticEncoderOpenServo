// swspi.h
//
// Software SPI for OpenServo, header file
//
// Copyright (C) 2009-2010  Jelle Lecomte
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


#ifndef __SWSPI_H__
#define __SWSPI_H__


#include <stdbool.h>
#include <stdint.h>

void swspi_init(void);

void swspi_save(void);

uint8_t swspi_read(uint16_t addr, void * data);

uint8_t swspi_write(uint16_t addr, const void * data);


#endif
