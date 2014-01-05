/** @file    main.c
 *  @author  Lukas Zurschmiede <lukas@ranta.ch>
 *  @email   <lukas@ranta.ch>
 *  @version 0.0.1
 *  @date    2014-01-05
 *  @see     https://balau82.wordpress.com/2010/12/16/using-newlib-in-arm-bare-metal-programs/
 *  @brief   This file contains some really lowlevel functions from syscalls.c so we are
 *           able to use printf, puts, etc. to test the code in a qemu emulated environment:
 *           $ qemu-system-arm  -cpu cortex-m3 -nographic -monitor null -serial null -semihosting -kernel IMAGE.elf
 * 
 *  Copyright (C) 2013-2014 @em Lukas @em Zurschmiede <lukas@ranta.ch>
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "inc/config.h"
#include <sys/stat.h>
#include <errno.h>

enum {
	UART_FR_RXFE = 0x10,
	UART_FR_TXFF = 0x20,
	UART0_ADDR = 0x10009000,
};

#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
#define UART_FR(baseaddr) (*(((unsigned int *)(baseaddr))+6))

int _close(int file) { return -1; }

int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file) { return 1; }

int _lseek(int file, int ptr, int dir) { return 0; }

int _open(const char *name, int flags, int mode) { return -1; }

int _read(int file, char *ptr, int len) {
	int todo;
	if(len == 0)
		return 0;
	while(UART_FR(UART0_ADDR) & UART_FR_RXFE);
	*ptr++ = UART_DR(UART0_ADDR);
	for(todo = 1; todo < len; todo++) {
		if(UART_FR(UART0_ADDR) & UART_FR_RXFE) {
			break;
		}
		*ptr++ = UART_DR(UART0_ADDR);
	}
	return todo;
}

caddr_t _sbrk(int incr) {
	extern char _ebss; // Defined by the linker
	static char *heap_end;
	char *prev_heap_end;
	if (heap_end == 0)
	{
		heap_end = &_ebss;
	}
	prev_heap_end = heap_end;
	char * stack = (char*) __get_MSP();
	if (heap_end + incr > stack)
	{
		errno = ENOMEM;
		return (caddr_t) -1;
	}
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len) {
	int todo;
	
	for (todo = 0; todo < len; todo++) {
		UART_DR(UART0_ADDR) = *ptr++;
	}
	return len;
}