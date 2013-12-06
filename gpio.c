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
#include "gpio.h"

pin_t pin_new (int fd, int num, int mode)
{
	pin_t pin = malloc (sizeof(struct pin));

	if (pin != NULL) {
		pin->fd = fd;
		pin->num = num;
		pin->mode = mode;

		if (ioctl (pin->fd, GPIO_GET, &pin->reg) < 0 ||
				gpio_mode (pin, mode)) {
			free (pin);
			pin = NULL;
		}
	}

	return pin;
}

void pin_free (pin_t* pin)
{
	if (*pin != NULL) {
		free (*pin);
		*pin = NULL;
	}
}

int gpio_mode (pin_t pin, int mode)
{
   if (mode)
	pin->reg |= GPIO_DIR(pin->num);
   else
	pin->reg &= ~GPIO_DIR(pin->num);

   pin->reg &= ~GPIO_VALUE(pin->num);

   if (ioctl (pin->fd, GPIO_SET, &pin->reg) < 0)
	   return -1;

   return 0;
}

int gpio_set (pin_t pin, int value)
{
   if (value)
       pin->reg |= GPIO_VALUE(pin->num);
   else
       pin->reg &= ~GPIO_VALUE(pin->num);

   if(ioctl (pin->fd, GPIO_SET, &pin->reg) < 0)
	   return -1;

   return 0;
}

int gpio_get (pin_t pin)
{
	if(ioctl (pin->fd, GPIO_GET, &pin->reg) < 0)
		return -1;

	return !!(GPIO_VALUE(pin->num) & pin->reg);
}
