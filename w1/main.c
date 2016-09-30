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
} W1;

void
goal_check(Nit_hset *stuff, int goal, int in)
{
	if (hset_contains(stuff, &in, sizeof(in))) {
		printf("%i -> %i\n", in, goal);
		/* hset_copy_add(stuff, &goal, sizeof(goal)); */
		return;
	}
}

void
func_check(W1 *w1, int goal)
{
	Nit_entry_list *ins = bimap_rget(w1->funcs, &goal, sizeof(goal));

	foreach (ins)
		goal_check(w1->stuff, goal, *(int *) ins->entry->dat);
}

void
run(W1 *w1)
{
	size_t goal_cnt = 0;

	for (; goal_cnt < w1->goal_num; ++goal_cnt)
		func_check(w1, w1->goals[goal_cnt]);
}

void
stuff_free(void *data)
{
	free(data);
}

void
funcs_free(void *key, void *storage)
{
	bimap_storage_free(storage);
}

int
main(int argc, char *argv[])
{
	W1 w1 = {
		.goals = goals,
		.goal_num = ARRAY_SIZE(goals),
		.funcs = bimap_new(0, 0),
		.stuff = hset_new(0),
	};

	(void) argc;
	(void) argv;

	bimap_add(w1.funcs,
		  &(int){ 4 }, sizeof(int),
		  &(int){ 1 }, sizeof(int));
	bimap_add(w1.funcs,
		  &(int){ 5 }, sizeof(int),
		  &(int){ 2 }, sizeof(int));
	bimap_add(w1.funcs,
		  &(int){ 6 }, sizeof(int),
		  &(int){ 3 }, sizeof(int));

	hset_copy_add(w1.stuff, &(int){ 4 }, sizeof(int));
	hset_copy_add(w1.stuff, &(int){ 5 }, sizeof(int));
	hset_copy_add(w1.stuff, &(int){ 6 }, sizeof(int));

	run(&w1);

	hset_free(w1.stuff, stuff_free);
	bimap_free(w1.funcs, funcs_free, funcs_free);
}
