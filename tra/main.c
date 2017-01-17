#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <t4.h>
#include <stdio.h>

#define NIT_SHORT_NAMES
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>
#include <nit/radix.h>

int
main(int argc, char *argv[])
{
	Nit_radix *radix = radix_new(NULL);
	Nit_radix_iter iter;
	T4_out out;
	T4_in in;
	T4_dat dat = T4_DAT_INIT;
	char *str;

	(void) argc;
	(void) argv;

	radix_iter_init(&iter, radix);

	radix_insert(radix, "a", 1, "hello!\n");
	radix_insert(radix, "abcd", 4, "bye!\n");

	if (!t4_term_set()) {
		fprintf(stderr, "Could not set up terminal for t4\n");
		return 1;
	}

	if (!t4_out_init(&out, "./pipe", 0, 1)) {
		fprintf(stderr, "Could not initialize t4_out\n");
		return 1;
	}

	t4_in_init(&in, 0);

	radix_iter_init(&iter, radix);

	while (1) {
		switch (t4_in(&in, &dat)) {
		case T4_IN_NONE:
		case T4_IN_WAIT:
			break;
		case T4_IN_ERR:
			t4_dat_dispose(&dat);
			t4_out_dispose(&out);
			fprintf(stderr, "Error in t4_in\n");
			return 1;
		case T4_IN_STUFF:
			if (radix_iter_move(&iter, dat.str, dat.len))
				goto end;

			if (str = radix_iter_get(&iter)) {
				t4_dat_set(&dat, str);
				t4_out(&out, &dat, NULL);
			}
		}
	}

end:
	t4_dat_dispose(&dat);
	t4_out_dispose(&out);
	printf("\n");
	return 0;
}


