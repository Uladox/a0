#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#define NIT_SHORT_NAMES
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>
#include <nit/bimap.h>

int goals[] = { 1, 2, 3 };

typedef struct {
	int *goals;
	size_t goal_num;

	Nit_bimap *funcs;
	Nit_hset *stuff;

	int bound_max;
} W2;

typedef struct {
	int steps;
	Nit_entry_list *point;
} Bound;

void
goal_check(Nit_hset *stuff, int goal, int in)
{
	if (hset_contains(stuff, &in, sizeof(in))) {
		printf("%i -> %i\n", in, goal);
		hset_copy_add(stuff, &goal, sizeof(goal));
		return;
	}
}

void
func_check(W2 *w2, int goal, int bounds)
{
	Nit_entry_list *ins = bimap_rget(w2->funcs, &goal, sizeof(goal));

	foreach (ins)
		goal_check(w2->stuff, goal, *(int *) ins->entry->dat);
}

void
run(W2 *w2)
{
	size_t goal_cnt = 0;

	for (; goal_cnt < w2->goal_num; ++goal_cnt)
		func_check(w2, w2->goals[goal_cnt]);
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
		.bound_max = 2
	};

	(void) argc;
	(void) argv;

	bimap_add(w2.funcs,
		  &(int){ 4 }, sizeof(int),
		  &(int){ 1 }, sizeof(int));
	bimap_add(w2.funcs,
		  &(int){ 5 }, sizeof(int),
		  &(int){ 2 }, sizeof(int));
	bimap_add(w2.funcs,
		  &(int){ 6 }, sizeof(int),
		  &(int){ 3 }, sizeof(int));

	hset_copy_add(w2.stuff, &(int){ 4 }, sizeof(int));
	hset_copy_add(w2.stuff, &(int){ 5 }, sizeof(int));
	hset_copy_add(w2.stuff, &(int){ 6 }, sizeof(int));

	run(&w2);

	hset_free(w2.stuff, stuff_free);
	bimap_free(w2.funcs, funcs_free, funcs_free);
}
