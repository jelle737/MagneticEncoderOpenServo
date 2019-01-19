// storage.h
//
// Software SPI for OpenServo
//
// Copyright (C) 2018 Jelle L
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

#ifndef __STORAGE_H__
#define __STORAGE_H__

void storage_registers_defaults(void);
void storage_init(void);
void storage_update(uint16_t position);

extern uint16_t reg_stor_value;

#endif
