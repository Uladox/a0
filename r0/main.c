#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node Node;
typedef struct edge Edge;

struct node {
	void *dat;
	Edge *edges[256];
};

struct edge {
	Node *node;
	size_t len;
	char str[];
};

void *
lookup(Node *node, char *str)
{
	Edge *e;

	for (; *str; str += e->len, node = e->node) {
		e = node->edges[*str++];

		/* works even when e is of len 0 */
		if (!e || strncmp(str, e->str, e->len) != 0)
			return NULL;
	}

	return node->dat;
}

void
node_init(Node *node, void *dat)
{
	node->dat = dat;
	memset(node->edges, 0, sizeof(node->edges));
}

Node *
node_new(void *dat)
{
	Node *node = malloc(sizeof(*node));

	node_init(node, dat);
	return node;
}

Edge *
edge_new(Node *node, char *pre, size_t len)
{
	Edge *e = malloc(sizeof(*e) + len);

	e->node = node;
	e->len = len;
	strncpy(e->str, pre, len);

	return e;
}

int
edge_split(Edge **old_ref, char *str, void *dat)
{
	Node *split;
	Edge *common;
	Edge *e = *old_ref;
	char *e_str = e->str;
	int i = 0;

	for (; i < e->len; ++i, ++e_str, ++str)
		if (!*str || *e_str != *str)
			break;

	if (i == e->len)
		return 0;

	if (!*str) {
		split = node_new(dat);
	} else {
		split = node_new(NULL);
		split->edges[*str++] = edge_new(node_new(dat), str, strlen(str));
	}

        common = edge_new(split, e->str, i);
	split->edges[*e_str++] = edge_new(e->node, e_str, e->len - i - 1);
	free(*old_ref);
	*old_ref = common;
	return 1;
}

int
node_insert(Node *node, char *str, void *dat)
{
	Edge *e;
	Edge **er;
	Node *new_node = node_new(dat);

	for (; *str; e = *er, str += e->len, node = e->node) {
		if (!*(er = &node->edges[*str++]))
			return !!(*er = edge_new(new_node, str, strlen(str)));

		if (edge_split(er, str, dat))
			return 1;
	}

	return 1;
}

int
main(int argc, char *argv[])
{
	Node *node = node_new(NULL);

	node_insert(node, "firs",    &(int){ 3 });
	node_insert(node, "first",   &(int){ 1 });
	node_insert(node, "second",  &(int){ 2 });
	node_insert(node, "secs",    &(int){ 4 });
	node_insert(node, "secoms",  &(int){ 5 });
	printf("%i\n", *(int *) lookup(node, "first"));
	printf("%i\n", *(int *) lookup(node, "second"));
	printf("%i\n", *(int *) lookup(node, "secs"));
	printf("%i\n", *(int *) lookup(node, "secoms"));
	printf("%i\n", *(int *) lookup(node, "firs"));
}
