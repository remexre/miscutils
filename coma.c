#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

void parse_args(int* argc, char*** argv, bool* daemonize);
void usage(void);

int main(int argc, char** argv) {
	bool daemonize;
	parse_args(&argc, &argv, &daemonize);

	if(daemonize) {
		if(daemon(0, 1) != 0)
			perror("daemon(3)");
		printf("%jd\n", (intmax_t) getpid());
	}
	while(1) {
		pause();
		perror("pause(3p)");
	}
}

void parse_args(int* argc, char*** argv, bool* daemonize) {
	int opt;
	while((opt = getopt(*argc, *argv, "D")) != -1) {
		switch(opt) {
		case 'D':
			*daemonize = true;
			break;
		default:
			usage();
		}
	}

	if(*argc == optind) {
		*argc -= optind;
		*argv += optind;
	} else {
		usage();
	}
}

void usage(void) {
	fprintf(stderr, "USAGE\n\t./coma [FLAGS]\n\nSUMMARY\n\tSleeps forever.\n\n"
			"FLAGS\n\t-D  Daemonize after starting. Will print the PID before"
			" forking off.\n\t-h  Prints this help message.\n");
	exit(EXIT_FAILURE);
}
