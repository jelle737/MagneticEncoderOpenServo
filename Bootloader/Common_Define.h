#ifndef __COMMON_DEFINE_H
#define __COMMON_DEFINE_H

#include "stdint.h"

#define INTVECT_PAGE_ADDRESS      	0x000	// The location of the start of the interrupt vector table address




// The flash memory page size
#define PAGE_SIZE 256     

// Page 120, the start of bootloader section
#define BOOT_PAGE_ADDRESS 0x7800  

// 16KB of flash divided by pages of size 128 bytes
#define TOTAL_NO_OF_PAGES  128   

// The number of pages being used for bootloader code
#define BOOTLOADER_PAGES          	(TOTAL_NO_OF_PAGES - BOOT_PAGE_ADDRESS/PAGE_SIZE)	

// For bounds check during page write/erase operation to protect the bootloader code from being corrupted
#define LAST_PAGE_NO_TO_BE_ERASED 	(TOTAL_NO_OF_PAGES - BOOTLOADER_PAGES)	



#define SELFPROGEN     SPMEN

#define MAX__APP_ADDR BOOT_PAGE_ADDRESS     // Maximum Application Address



/*****************************************************************************/
/*****************************************************************************/

#define EEMEM_ADDR_AVERSION 		0xFF	// The address in EEPROM where application revision identifier will be stored
#define BVERSION 			0x10	// This bootloader revision identifier


#define CSTARTUP_ADDRESS            0x800
#define BOOT_FLAG                   0x2A

#define SWUART_PORT_REG                 PORTB
#define SWUART_PIN_REG                  PINB
#define SWUART_PIN                      PB0    

#endif