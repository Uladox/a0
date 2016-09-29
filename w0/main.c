#include <stdio.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

int goals[] = { 1, 2, 3 };
int stuff[] = { 4, 5, 6 };

typedef struct {
	int in;
	int out;
} F;


typedef struct {
	int *goals;
	size_t goal_num;

	F *funcs;
	size_t func_num;

	int *stuff;
	size_t stuff_num;
} W0;

F funcs[] = {
	[0] = { .in = 4, .out = 1 },
	[1] = { .in = 5, .out = 2 },
	[2] = { .in = 6, .out = 3 }
};

void
goal_check(F *func, int *stuff, size_t stuff_num)
{
	while (stuff_num--) {
		if (stuff[stuff_num] == func->in) {
			printf("%i -> %i\n", func->in, func->out);
			stuff[stuff_num] = func->out;
			return;
		}
	}
}

void
func_check(W0 *w0, int goal)
{
	size_t func_cnt = 0;

	for (; func_cnt < w0->func_num; ++func_cnt)
		if (w0->funcs[func_cnt].out == goal)
			goal_check(w0->funcs + func_cnt,
				   w0->stuff, w0->stuff_num);
}

void
run(W0 *w0)
{
	size_t goal_cnt = 0;

	for (; goal_cnt < w0->goal_num; ++goal_cnt)
		func_check(w0, w0->goals[goal_cnt]);
}

int
main(int argc, char *argv[])
{
	W0 w0 = {
		.goals = goals,
		.goal_num = ARRAY_SIZE(goals),
		.funcs = funcs,
		.func_num = ARRAY_SIZE(funcs),
		.stuff = stuff,
		.stuff_num = ARRAY_SIZE(stuff)
	};

	run(&w0);
}
