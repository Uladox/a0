#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>

#define NIT_SHORT_NAMES
#include <nit/macros.h>
#include <nit/palloc.h>
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>
#include <nit/bimap.h>
#include <nit/socket.h>

#include "kwt.h"

int goal_arr[] = { 10, 1, 2 };

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
	Nit_joiner *jnr = joiner_new("kwt");
	Nit_joint *jnt = joiner_accept(jnr);

	Wi wi = {
		.person = cog_new("person", jnt, 5, 5),
		.goal_arr = goal_arr,
		.goal_num = ARRAY_UNITS(goal_arr),
		.funcs = bimap_new(0, 0),
		.stuff = hset_new(0),
		.bound_max = 1,
		.goals = NULL
	};

	(void) argc;
	(void) argv;

	/* 4 -> 1, 6 -> 5, 5 -> 3, 3 -> 2 */
	bimap_add(wi.funcs,
		  &(int){ 4 }, sizeof(int),
		  &(int){ 1 }, sizeof(int));
	bimap_add(wi.funcs,
		  &(int){ 6 }, sizeof(int),
		  &(int){ 5 }, sizeof(int));
	bimap_add(wi.funcs,
		  &(int){ 5 }, sizeof(int),
		  &(int){ 3 }, sizeof(int));
	bimap_add(wi.funcs,
		  &(int){ 3 }, sizeof(int),
		  &(int){ 2 }, sizeof(int));

	/* 4, 6 */
	hset_copy_add(wi.stuff, &(int){ 4 }, sizeof(int));
	hset_copy_add(wi.stuff, &(int){ 6 }, sizeof(int));

	hset_copy_add(wi.person->wants, &(int){ 2 }, sizeof(int));

	run(&wi);

	hset_free(wi.stuff, stuff_free);
	bimap_free(wi.funcs, funcs_free, funcs_free);
}
