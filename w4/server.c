#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define NIT_SHORT_NAMES
#include <nit/macros.h>
#include <nit/palloc.h>
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>
#include <nit/bimap.h>
#include <nit/socket.h>

#include "kwt.h"

int goals[] = { 1, 2 };

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

void
server_loop(Nit_joint *jnt)
{
	char *buf = malloc(256);
	int32_t size = 256;

	while (1) {
		int32_t msg_size;
		int val;

		switch (val = joint_read(jnt, &buf, &size, &msg_size, 1)) {
		case NIT_JOIN_CLOSED:
			return;
		case NIT_JOIN_ERROR:
			printf("ERROR!\n");
			return;
		case NIT_JOIN_OK:
			printf("msg_size: %" PRIi32 "\n", msg_size);
			buf[msg_size] = '\0';
			printf("buf: %s\n", buf);
		case NIT_JOIN_NONE:
			printf("val: %i\n", val);
			sleep(1);
		}
	}
}

int
main(int argc, char *argv[])
{
	Wi wi = {
		.person = cog_new("person"),
		.goals = goals,
		.goal_num = ARRAY_UNITS(goals),
		.funcs = bimap_new(0, 0),
		.stuff = hset_new(0),
		.bound_max = 2,
		.bounds = NULL
	};

	(void) argc;
	(void) argv;

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

	hset_copy_add(wi.stuff, &(int){ 4 }, sizeof(int));
	hset_copy_add(wi.stuff, &(int){ 6 }, sizeof(int));

	hset_copy_add(wi.person->wants, &(int){ 2 }, sizeof(int));

	run(&wi);

	hset_free(wi.stuff, stuff_free);
	bimap_free(wi.funcs, funcs_free, funcs_free);

	Nit_joiner *jnr = joiner_new("kwt");
	Nit_joint *jnt = joiner_accept(jnr);
	server_loop(jnt);
}
