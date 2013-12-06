/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MORSE_H
#define MORSE_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "gpio.h"

/* for use with nanoseconds/milliseconds */
#define MILLION		1000000L
#define BILLION		1000000000L

/* default morse timing in nanoseconds */
#define MORSE_UNIT	(100 * MILLION)	/* 100ms */
#define MORSE_TOLERANCE	(100 * MILLION)	/* 80ms */

/* largest size of morse code */
#define MAX_MORSE_CODE	8

/* dit-dit-dit dah-dah-dah dit-dit-dit */
#define DIT	'.'
#define DAH	'-'

/* morse char type */
typedef struct morse {
	char    ascii;
	char*   morse;
} *morsechar_t;

/* get morse code from input pin */
extern void get_morse (pin_t pin);

/* lookup up morse code in ...- format */
extern morsechar_t morse_lookup (const char* code);

/* lookup ascii char in morse_table */
extern morsechar_t ascii_to_morse (char ascii);

/* output string on pin */
extern void morse_output(const char* str, pin_t pin);

#endif /* MORSE_H */
