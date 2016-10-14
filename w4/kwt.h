/* Include these
 * #include <stdint.h>
 * #include <stdlib.h>
 * #include <nit/list.h>
 * #include <nit/hset.h>
 * #include <nit/hmap.h>
 * #include <nit/bimap.h>
 */

typedef struct bound Bound;

struct bound {
	Nit_entry_list *point;
	Bound *super;
};

typedef struct {
	Nit_dlist list;
	Bound *bound;
	int goal;
} Bound_list;

typedef struct {
	const char *name;
        Nit_hset *knows;
        Nit_hset *wants;
} Cog;

typedef struct {
	Cog *person;

	int *goals;
	size_t goal_num;

	Nit_bimap *funcs;
	Nit_hset *stuff;

	int bound_max;
	int steps;
	Bound_list *bounds;
} Wi;

Cog *
cog_new(const char *name);

void
run(Wi *wi);
