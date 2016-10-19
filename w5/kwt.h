/* Include these
 * #include <stdint.h>
 * #include <stdlib.h>
 * #include <pthread.h>
 * #include <sys/select.h>
 * #include <sys/socket.h>
 * #include <sys/un.h>
 * #include <nit/list.h>
 * #include <nit/hset.h>
 * #include <nit/hmap.h>
 * #include <nit/bimap.h>
 * #include <nit/socket.h>
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
	Nit_joint *in;

	int goal;
	int asked;
	time_t timer;
	double s_span;
	int bored;
	double a_span;
	Bound_list *list;
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
cog_new(const char *name, Nit_joint *in, double s_span, double a_span);

void
run(Wi *wi);
