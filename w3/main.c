#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NIT_SHORT_NAMES
#include <nit/palloc.h>
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>
#include <nit/bimap.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))
#define LIST_INC(LIST) LIST = LIST_NEXT(LIST)
#define DLIST_JOIN(FRONT, BACK)			\
	do {					\
		LIST_CONS(FRONT, BACK);		\
		DLIST_RCONS(BACK, FRONT);	\
	} while (0)

int goals[] = { 1, 2 };

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
	int *goals;
	size_t goal_num;

	Nit_bimap *funcs;
	Nit_hset *stuff;

	int bound_max;
	int steps;
	Bound_list *bounds;
} W3;

Bound *
bound_new(Nit_entry_list *point, Bound *super)
{
	Bound *bound = palloc(bound);

	bound->point = point;
	bound->super = super;

	return bound;
}

Bound *
bound_list_set(Bound_list *list, Nit_entry_list *point)
{
	Bound *bound = bound_new(point, list->bound);

        list->bound = bound;

	return bound;
}

void
bound_free(Bound *bound)
{
	if (bound->super)
		bound_free(bound->super);

	free(bound);
}

void
w3_add_list(W3 *w3, int goal)
{
	Bound_list *list = palloc(list);

	LIST_CONS(list, w3->bounds);
	DLIST_RCONS(list, NULL);

	if (w3->bounds)
		DLIST_RCONS(w3->bounds, list);

	list->bound = NULL;
	w3->bounds = list;
	list->goal = goal;
}

int
goal_check(Bound_list *list, W3 *w3, int goal, int in)
{
	if (hset_contains(w3->stuff, &in, sizeof(in))) {
		printf("%i -> %i\n", in, goal);
		hset_copy_add(w3->stuff, &goal, sizeof(goal));

		dlist_remove(list);

		if (list->bound)
			bound_free(list->bound);

		if (w3->bounds == list)
			w3->bounds = NULL;

		free(list);

		return 1;
	}

	return 0;
}

static inline int
bound_dat(Bound *bound)
{
	return *(int *) bound->point->entry->dat;
}

int
func_check(Bound_list *list, W3 *w3, int goal)
{
	Nit_entry_list *ins = bimap_rget(w3->funcs, &goal, sizeof(goal));
	Bound *bound;

	if (!ins) {
		if (list->bound) {
			bound = list->bound->super;
			free(list->bound);
			list->bound = bound;
		}

		return 0;
	}

        bound_list_set(list, ins);

	return 1;
}

int
bound_check(Bound_list *list, W3 *w3, int goal)
{
	int dat;

	while (w3->steps > 0 && list->bound->point) {
		if (goal_check(list, w3, goal, (dat = bound_dat(list->bound))))
			return 1;

	        if (!func_check(list, w3, dat))
			LIST_INC(list->bound->point);

		if (--w3->steps <= 0)
			return 0;
	}

	return 0;
}

int
check_bounds(W3 *w3)
{
	int work = 0;
	Bound_list *list = w3->bounds;
	Bound_list *tmp;

	delayed_foreach (tmp, list) {
		if (!bound_check(tmp, w3, tmp->goal))
			work = 1;

		w3->steps = w3->bound_max;
	}

	return work;
}

void
run(W3 *w3)
{
	size_t goal_cnt = 0;

	for (; goal_cnt < w3->goal_num; ++goal_cnt) {
	        w3_add_list(w3, w3->goals[goal_cnt]);

		func_check(w3->bounds, w3, w3->goals[goal_cnt]);
	}

	while (check_bounds(w3));
}

void
stuff_free(void *data)
{
	free(data);
}

void
funcs_free(void *key, void *storage)
{
	(void) key;

	bimap_storage_free(storage);
}

int
main(int argc, char *argv[])
{
	W3 w3 = {
		.goals = goals,
		.goal_num = ARRAY_SIZE(goals),
		.funcs = bimap_new(0, 0),
		.stuff = hset_new(0),
		.bound_max = 1,
		.bounds = NULL
	};

	(void) argc;
	(void) argv;

	bimap_add(w3.funcs,
		  &(int){ 4 }, sizeof(int),
		  &(int){ 1 }, sizeof(int));
	bimap_add(w3.funcs,
		  &(int){ 6 }, sizeof(int),
		  &(int){ 5 }, sizeof(int));
	bimap_add(w3.funcs,
		  &(int){ 5 }, sizeof(int),
		  &(int){ 3 }, sizeof(int));
	bimap_add(w3.funcs,
		  &(int){ 3 }, sizeof(int),
		  &(int){ 2 }, sizeof(int));

	hset_copy_add(w3.stuff, &(int){ 4 }, sizeof(int));
	hset_copy_add(w3.stuff, &(int){ 6 }, sizeof(int));

	run(&w3);

	hset_free(w3.stuff, stuff_free);
	bimap_free(w3.funcs, funcs_free, funcs_free);
}
