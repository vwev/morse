#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "morse.h"

#define DEFAULT_DEVICE		"/dev/ttyUSB0"
#define DEFAULT_OUTPUT_PIN	0
#define DEFAULT_INPUT_PIN	1
#define DEFAULT_MODE		GPIO_OUTPUT

static pin_t pin = NULL;

void sigtrap (int signum)
{
	if (pin->mode == GPIO_OUTPUT)
		gpio_set(pin, GPIO_LOW);

	putchar ('\n');

	if (signum != SIGSTOP)
		exit (EXIT_FAILURE);
}

int main (int argc, char** argv)
{
	char* dev = DEFAULT_DEVICE;
	int mode = DEFAULT_MODE;
	int num = -1;
	int opt;
	int fd;
	char line[128];

	signal(SIGINT, sigtrap);
	signal(SIGTERM, sigtrap);
	signal(SIGQUIT, sigtrap);
	signal(SIGSTOP, sigtrap);

	while ((opt = getopt (argc, argv, "d:p:oi")) != -1)
		switch (opt) {
		case 'd':
			dev = optarg;
			break;
		case 'p':
			num = atoi(optarg);
			break;
		case 'o':
			mode = GPIO_OUTPUT;
			break;
		case 'i':
			mode = GPIO_INPUT;
			break;
		default: break;
		}

	if (num < 0)
		num = (mode == GPIO_OUTPUT) ? DEFAULT_OUTPUT_PIN
			: DEFAULT_INPUT_PIN;

	fd = open (dev, O_RDWR);
	if (fd < 0) {
		perror (dev);
		exit (EXIT_FAILURE);
	}

	pin = pin_new (fd, num, mode);
	if (pin == NULL) {
		fprintf (stderr, "could not allocate pin #%d\n", num);
		exit (EXIT_FAILURE);
	}

	if (pin->mode == GPIO_INPUT) {
		get_morse (pin);
	} else /* GPIO_OUTPUT */ {
		if (optind >= argc || (argv[optind][0] == '-' &&
					argv[optind][1] == 0)) {
			while (fgets (line, sizeof(line), stdin) != NULL)
				morse_output (line, pin);
		} else {
			for (; optind < argc; optind++) {
				morse_output (argv[optind], pin);

				if (argc != optind + 1)
					morse_output (" ", pin);
			}

		}

		putchar ('\n');
	}

	pin_free (&pin);
	close (fd);

	return 0;
}
