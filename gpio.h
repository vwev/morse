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
#ifndef GPIO_H
#define GPIO_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>

/* LOW/HIGH for pin output */
#define GPIO_LOW	0
#define GPIO_HIGH	1

/* pin mode */
#define GPIO_INPUT	0
#define GPIO_OUTPUT	1

/* ioctl to write a GPIO register */
#define GPIO_SET     _IOW('v', 0xac, int)
/* ioctl to read a GPIO register */
#define GPIO_GET     _IOR('v', 0xac, int)

/* macro function to set direction bit of a gpio pin */
#define GPIO_DIR(gpio)     ((uint8_t)(1 << ((gpio) + 4)))
/* macro function to set the bit of a gpio pin */
#define GPIO_VALUE(gpio)   ((uint8_t)(1 << ((gpio) + 6)))

/* used to store pin information */
typedef struct pin {
	int fd;		/* Device file descriptor */
	int num;	/* pin number(pin0, pin1, ...) */
	int mode;	/* pin mode(GPIO_INPUT / GPIO_OUTPUT) */
	int reg;	/* the internal PL2303 register value */
} *pin_t;

/* allocate a new pin object, set it to mode */
extern pin_t pin_new (int fd, int num, int mode);

/* free a pin object */
extern void pin_free (pin_t* pin);

/* set pin mode to GPIO_INPUT/GPIO_OUTPUT */
extern int gpio_mode (pin_t pin, int mode);

/* set output pin value to low(0) or high(1) */
extern int gpio_set (pin_t pin, int value);

/* get input pin value */
extern int gpio_get (pin_t pin);

#endif /* GPIO_H */
