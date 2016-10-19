#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <time.h>

#define NIT_SHORT_NAMES
#include <nit/palloc.h>
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>
#include <nit/bimap.h>
#include <nit/socket.h>

#include "kwt.h"

#define LIST_INC(LIST) LIST = LIST_NEXT(LIST)

Cog *
cog_new(const char *name, Nit_joint *in, double s_span, double a_span)
{
	Cog *cog = palloc(cog);

	cog->name = name;
	cog->in = in;
	cog->knows = hset_new(0);
	cog->wants = hset_new(0);

	cog->goal = -1;
	cog->asked = 0;
        cog->s_span = s_span;
	cog->bored = 0;
        cog->a_span = a_span;
	cog->list = NULL;

	return cog;
}

enum nit_join_status
input_get(Cog *cog, int *resp)
{
	char *buf = malloc(256);
	int32_t size = 256;
	time_t end;
	double diff;

	if (!cog->asked)
		time(&cog->timer);

	while (1) {
		int32_t msg_size;
		int val;

		switch (val = joint_read(cog->in, &buf, &size, &msg_size, 1)) {
		case NIT_JOIN_CLOSED:
		case NIT_JOIN_ERROR:
			sleep(1);
			return val;
		case NIT_JOIN_NONE:

			time(&end);
			diff = difftime(end, cog->timer);

			if (diff > cog->s_span + cog->a_span) {
				printf("bye!\n");

				return NIT_JOIN_CLOSED;
			} else if (diff > cog->s_span && !cog->bored) {
				printf("there?\n");
				cog->bored = 1;
			}

			sleep(1);

			return NIT_JOIN_NONE;
		case NIT_JOIN_OK:
			buf[msg_size] = '\0';
			*resp = atoi(buf);
			sleep(1);
			return NIT_JOIN_OK;
		}
	}
}

void
ask(Bound_list *list, Cog *cog, int prob)
{
	printf(":%s %i?\n", cog->name, prob);
	cog->goal = prob;
	cog->list = list;
}

int
response(Wi *wi, Cog *cog)
{
	int resp;

        switch (input_get(cog, &resp)) {
	case NIT_JOIN_OK:
		return resp;
	case NIT_JOIN_NONE:
		return -1;
	default:
		wi->person = NULL;
		return -1;
	}
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

	if (wi->person && hset_contains(wi->person->wants, &goal, sizeof(goal)))
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

	printf("%i\n", goal);

	while (wi->steps > 0 && list->bound && list->bound->point) {
		if (goal_check(list, wi, goal, (dat = bound_dat(list->bound))))
			return 1;

	        if (!func_check(list, wi, dat))
			LIST_INC(list->bound->point);

		int resp;
		if (wi->person)
			if ((resp = response(wi, wi->person)) >= 0) {
				printf("resp\n");
				/* finish_up(list, wi, wi->person->goal, resp); */
			}

		if (--wi->steps <= 0) {
			if (wi->person && wi->person->goal < 0)
				ask(list, wi->person, goal);

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
