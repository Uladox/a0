#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NIT_SHORT_NAMES
#include <nit/palloc.h>
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>
#include <nit/bimap.h>

#include "kwt.h"

#define LIST_INC(LIST) LIST = LIST_NEXT(LIST)

Cog *
cog_new(const char *name)
{
	Cog *cog = palloc(cog);

	cog->name = name;
	cog->knows = hset_new(0);
	cog->wants = hset_new(0);

	return cog;
}

int
ask(Cog *cog, int prob)
{
	int resp;

	printf(":%s %i? ", cog->name, prob);
	scanf("%i", &resp);

	return resp;
}

void
say(Cog *cog, int prob, int resp)
{
	printf(":%s %i -> %i!\n", cog->name, prob, resp);
}

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
wi_add_list(Wi *wi, int goal)
{
	Bound_list *list = palloc(list);

	LIST_CONS(list, wi->bounds);
	DLIST_RCONS(list, NULL);

	if (wi->bounds)
		DLIST_RCONS(wi->bounds, list);

	list->bound = NULL;
	wi->bounds = list;
	list->goal = goal;
}

void
finish_up(Bound_list *list, Wi *wi, int goal, int in)
{
	printf("%i -> %i\n", in, goal);

	if (hset_contains(wi->person->wants, &goal, sizeof(goal)))
		say(wi->person, in, goal);

	hset_copy_add(wi->stuff, &goal, sizeof(goal));

	dlist_remove(list);

	if (list->bound)
		bound_free(list->bound);

	if (wi->bounds == list)
		wi->bounds = NULL;

	free(list);
}

int
goal_check(Bound_list *list, Wi *wi, int goal, int in)
{
	if (!hset_contains(wi->stuff, &in, sizeof(in)))
		return 0;

	finish_up(list, wi, goal, in);

	return 1;
}

int
ask_check(Bound_list *list, Wi *wi, int goal)
{
	int in;

	if ((in = ask(wi->person, goal)) == -1)
		return 0;

	finish_up(list, wi, goal, in);

	return 1;
}

static inline int
bound_dat(Bound *bound)
{
	return *(int *) bound->point->entry->dat;
}

int
func_check(Bound_list *list, Wi *wi, int goal)
{
	Nit_entry_list *ins = bimap_rget(wi->funcs, &goal, sizeof(goal));
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
bound_check(Bound_list *list, Wi *wi, int goal)
{
	int dat;

	while (wi->steps > 0 && list->bound->point) {
		if (goal_check(list, wi, goal, (dat = bound_dat(list->bound))))
			return 1;

	        if (!func_check(list, wi, dat))
			LIST_INC(list->bound->point);

		if (--wi->steps <= 0) {
			if (ask_check(list, wi, goal))
				return 1;
			else
				return 0;
		}
	}

	return 0;
}

int
check_bounds(Wi *wi)
{
	int work = 0;
	Bound_list *list = wi->bounds;
	Bound_list *tmp;

	delayed_foreach (tmp, list) {
		if (!bound_check(tmp, wi, tmp->goal))
			work = 1;

		wi->steps = wi->bound_max;
	}

	return work;
}

void
run(Wi *wi)
{
	size_t goal_cnt = 0;

	for (; goal_cnt < wi->goal_num; ++goal_cnt) {
	        wi_add_list(wi, wi->goals[goal_cnt]);

		func_check(wi->bounds, wi, wi->goals[goal_cnt]);
	}

	while (check_bounds(wi));
}
