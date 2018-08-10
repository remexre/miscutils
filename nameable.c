#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void parse_args(int* argc, char*** argv, bool* daemonize, bool* is_child);
char* make_name(int argc, char** argv);
void usage(void);

int main(int argc, char** argv) {
	bool daemonize = false;
	// Technically, "child" is incorrect, but execee sounds weird.
	bool is_child = false;
	char* me = argv[0];
	parse_args(&argc, &argv, &daemonize, &is_child);

	if(is_child) {
		if(daemonize) {
			if(daemon(0, 1) != 0)
				perror("daemon(3)");
			printf("%jd\n", (intmax_t) getpid());
		}
		while(1) {
			pause();
			perror("pause(3p)");
		}
	} else {
		char* name = make_name(argc, argv);
		char* child_argv[4] = { name, "-C", "-D", NULL };
		child_argv[daemonize ? 3 : 2] = NULL;
		execvp(me, child_argv);
	}
}

void parse_args(int* argc, char*** argv, bool* daemonize, bool* is_child) {
	int opt;
	while((opt = getopt(*argc, *argv, "CD")) != -1) {
		switch(opt) {
		case 'C':
			*is_child = true;
			break;
		case 'D':
			*daemonize = true;
			break;
		default:
			usage();
		}
	}

	if(*is_child || (*argc > optind)) {
		*argc -= optind;
		*argv += optind;
	} else {
		usage();
	}
}

char* make_name(int argc, char** argv) {
	size_t name_len = argc - 1;
	size_t* lens = malloc(argc * sizeof(size_t));
	for(int i = 0; i < argc; i++) {
		size_t len = strlen(argv[i]);
		name_len += len;
		lens[i] = len;
	}

	char* name = malloc(name_len + 1);
	size_t j = 0;
	for(int i = 0; i < argc; i++) {
		size_t len = lens[i];
		strncpy(&name[j], argv[i], len);
		j += len;
		name[j++] = ' ';
	}
	assert(j != 0);
	name[j - 1] = '\0';
	assert(j - 1 == name_len);

	return name;
}

void usage(void) {
	fprintf(stderr, "USAGE\n\t./nameable [FLAGS] NAME...\n\nSUMMARY\n\tThis"
			" process' name will appear to be NAME.\n\nFLAGS\n\t-D  Daemonize"
			" after starting. Will print the PID before forking off.\n"
			"\t-h  Prints this help message.\n");
	exit(EXIT_FAILURE);
}
