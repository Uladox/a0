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
} W2;

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
w2_add_list(W2 *w2, int goal)
{
	Bound_list *list = palloc(list);

	LIST_CONS(list, w2->bounds);
	DLIST_RCONS(list, NULL);

	if (w2->bounds)
		DLIST_RCONS(w2->bounds, list);

	list->bound = NULL;
	w2->bounds = list;
	list->goal = goal;
}

int
goal_check(Bound_list *list, W2 *w2, int goal, int in)
{
	if (hset_contains(w2->stuff, &in, sizeof(in))) {
		printf("%i -> %i\n", in, goal);
		hset_copy_add(w2->stuff, &goal, sizeof(goal));

		dlist_remove(list);

		if (list->bound)
			bound_free(list->bound);

		if (w2->bounds == list)
			w2->bounds = NULL;

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
func_check(Bound_list *list, W2 *w2, int goal)
{
	Nit_entry_list *ins = bimap_rget(w2->funcs, &goal, sizeof(goal));
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
bound_check(Bound_list *list, W2 *w2, int goal)
{
	int dat;

	while (w2->steps > 0 && list->bound->point) {
		if (goal_check(list, w2, goal, (dat = bound_dat(list->bound))))
			return 1;

	        if (!func_check(list, w2, dat))
			LIST_INC(list->bound->point);

		if (--w2->steps <= 0)
			return 0;
	}

	return 0;
}

int
check_bounds(W2 *w2)
{
	int work = 0;
	Bound_list *list = w2->bounds;
	Bound_list *tmp;

	delayed_foreach (tmp, list) {
		if (!bound_check(tmp, w2, tmp->goal))
			work = 1;

		w2->steps = w2->bound_max;
	}

	return work;
}

void
run(W2 *w2)
{
	size_t goal_cnt = 0;

	for (; goal_cnt < w2->goal_num; ++goal_cnt) {
	        w2_add_list(w2, w2->goals[goal_cnt]);

		func_check(w2->bounds, w2, w2->goals[goal_cnt]);
	}

	while (check_bounds(w2));
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
	W2 w2 = {
		.goals = goals,
		.goal_num = ARRAY_SIZE(goals),
		.funcs = bimap_new(0, 0),
		.stuff = hset_new(0),
		.bound_max = 1,
		.bounds = NULL
	};

	(void) argc;
	(void) argv;

	bimap_add(w2.funcs,
		  &(int){ 4 }, sizeof(int),
		  &(int){ 1 }, sizeof(int));
	bimap_add(w2.funcs,
		  &(int){ 6 }, sizeof(int),
		  &(int){ 5 }, sizeof(int));
	bimap_add(w2.funcs,
		  &(int){ 5 }, sizeof(int),
		  &(int){ 3 }, sizeof(int));
	bimap_add(w2.funcs,
		  &(int){ 3 }, sizeof(int),
		  &(int){ 2 }, sizeof(int));

	hset_copy_add(w2.stuff, &(int){ 4 }, sizeof(int));
	hset_copy_add(w2.stuff, &(int){ 6 }, sizeof(int));

	run(&w2);

	hset_free(w2.stuff, stuff_free);
	bimap_free(w2.funcs, funcs_free, funcs_free);
}
