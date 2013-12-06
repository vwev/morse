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
#include "morse.h"

static const struct timespec dit = {0, MORSE_UNIT};
static const struct timespec dah = {0, MORSE_UNIT * 3};
static const struct timespec word = {0, MORSE_UNIT * 7};
static const struct timespec input_poll = {0, MORSE_UNIT / 8};

static struct morse morse_table[] = {
	{'A',".-"}, {'B',"-..."}, {'C',"-.-."}, {'D',"-.."}, {'E',"."},
	{'F',"..-."}, {'G',"--."}, {'H',"...."}, {'I',".."}, {'J',".---"},
	{'K',"-.-"}, {'L',".-.."}, {'M',"--"}, {'N',"-."}, {'O',"---"},
	{'P',".--."}, {'Q',"--.-"}, {'R',".-."}, {'S',"..."}, {'T',"-"},
	{'U',"..-"}, {'V',"...-"}, {'W',".--"}, {'X',"-..-"}, {'Y',"-.--"},
	{'Z',"--.."},
	{'0',"-----"},{'1',".----"},{'2',"..---"},{'3',"...--"},{'4',"....-"},
	{'5',"....."},{'6',"-...."},{'7',"--..."},{'8',"---.."},{'9',"----."},
	{'.',".-.-.-"}, {',',"--..--"}, {'?',"..--.."},
	{'\'',".----."}, {'!',"-.-.--"}, {'/',"-..-."}, {'@',".--.-."},
	{'(',"-.--."}, {')',"-.--.-"}, {'&',".-..."}, {':',"---..."},
	{';',"-.-.-."}, {'=',"-...-"}, {'+',".-.-."}, {'-',"-....-"},
	{'_',"..--.-"}, {'"',".-..-."}, {'$',"...-..-"}
};

static size_t morse_table_len = sizeof(morse_table) / sizeof(struct morse);

/* TODO: add break in gpio_get loops */
static char get_bit (pin_t pin)
{
	struct timespec start, end;
	double accum;

	clock_gettime (CLOCK_REALTIME, &start);

	while (gpio_get (pin))
		nanosleep (&input_poll, NULL);

	clock_gettime (CLOCK_REALTIME, &end);

	accum = ((end.tv_sec - start.tv_sec) * 1E9) +
		(end.tv_nsec - start.tv_nsec);

	return (accum < (dah.tv_nsec - MORSE_TOLERANCE)) ? DIT : DAH;
}

static morsechar_t get_code (pin_t pin, double *accum)
{
	struct timespec start, end;
	char code[MAX_MORSE_CODE];
	int i = 0;

	const double max = dah.tv_nsec - MORSE_TOLERANCE / 2;

	do {
		code[i++] = get_bit (pin);

		clock_gettime (CLOCK_REALTIME, &start);

		while (!gpio_get (pin))
			nanosleep (&input_poll, NULL);

		clock_gettime (CLOCK_REALTIME, &end);

		*accum = ((end.tv_sec - start.tv_sec) * 1E9) +
			(end.tv_nsec - start.tv_nsec);
	} while (*accum <= max && i < MAX_MORSE_CODE);

	code[i] = 0;

	return (i == MAX_MORSE_CODE) ? NULL : morse_lookup (code);
}

void get_morse (pin_t pin)
{
	morsechar_t c;
	double accum;

	/* wait for for no pin input */
	while (gpio_get (pin))
		nanosleep(&input_poll, NULL);

	/* wait for pin input(initialize for get_bit) */
	while (!gpio_get (pin))
		nanosleep(&input_poll, NULL);

	for (;;) {
		c = get_code (pin, &accum);
		if (c != NULL)
			putchar (c->ascii);

		if (accum > word.tv_nsec)
			putchar (' ');

		fflush (stdout);
	}
}

morsechar_t morse_lookup (const char *morse)
{
	/* TODO: reimplement log n */
	int i = 0;

	for (i = 0; i < morse_table_len; i++)
	if (strcmp (morse, morse_table[i].morse) == 0)
		return &morse_table[i];

	return NULL;
}

morsechar_t ascii_to_morse (char ascii)
{
	size_t index = 0;

	if (ascii >= 65 && ascii <= 90)
		index = ascii - 65; // UPPER
	else if (ascii >= 97 && ascii <= 122)
		index = ascii - 97; // lower
	else
		// others
		for (index = 25; index < morse_table_len; index++)
		if (ascii == morse_table[index].ascii)
			break;

	return index < morse_table_len ? &morse_table[index] : NULL;
}

static void morse_output_char (morsechar_t code, pin_t pin)
{
	if (code != NULL) {
		char* ptr = code->morse;

		for (; *ptr != 0; ptr++) {
			gpio_set (pin, GPIO_HIGH);

			putchar(*ptr);
			fflush(stdout);

			if(*ptr == DIT)
				nanosleep (&dit, NULL);
			else if(*ptr == DAH)
				nanosleep (&dah, NULL);

			gpio_set (pin, GPIO_LOW);

			if (*(ptr + 1))
				nanosleep (&dit, NULL);
		}
	}
}

void morse_output (const char *ptr, pin_t pin)
{
	for (; *ptr != 0; ptr++) {
		if (*ptr == ' ') {
			printf ("   ");
			fflush (stdout);
			nanosleep (&word, NULL);
		} else if (*ptr == '\n') {
			putchar ('\n');
		} else {
			morse_output_char (ascii_to_morse (*ptr), pin);
		}

		if (*(ptr + 1)) {
			putchar (' ');
			fflush (stdout);
			nanosleep (&dah, NULL);
		}
	}
}
