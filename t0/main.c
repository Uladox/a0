#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define NIT_SHORT_NAMES
#include <nit/palloc.h>
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>

typedef struct trie Trie;

/* enum trie_type { TRIE_IN, TRIE_ACT }; */

typedef struct {
	Nit_dlist list;
	Trie *match;
	Trie *result;
} Trie_list;

typedef struct {
	Nit_dlist list;
	char *act;
} Act_list;

struct trie {
	/* enum trie_type type; */
	Nit_hmap *simp;
	Trie_list *abst;
	char *act; /* string is just temp data */
};

typedef struct {
	/* Act_list *start; */
	Act_list *end;
	Trie *root;
} Crawl;

Trie *
trie_new(char *act)
{
	Trie *trie = palloc(trie);

	trie->simp = hmap_new(0);
	trie->abst = NULL;
	trie->act = act;

	return trie;
}

Crawl *
crawl_new(Trie *root)
{
	Crawl *crawl = palloc(crawl);

	crawl->end = NULL;
	crawl->root = root;

	return crawl;
}

void
trie_add_simp(Trie *trie, char c, Trie *next)
{
	hmap_add(trie->simp, &c, sizeof(c), next);
}

void
trie_add_abst(Trie *trie, Trie *match, Trie *next)
{
	Trie_list *list = palloc(list);

	list->match = match;
	list->result = next;

	LIST_CONS(list, trie->abst);
	DLIST_RCONS(list, NULL);

	if (trie->abst)
		DLIST_RCONS(trie->abst, list);

	trie->abst = list;
}

int
run(Crawl *crawl, Trie *trie, char *data);

Trie *
trie_abst(Crawl *crawl, Trie *trie, char *data)
{
	Trie_list *list = trie->abst;

	foreach (list)
		if (run(crawl, list->match, data))
			return list->result;

	return NULL;
}

int
run(Crawl *crawl, Trie *trie, char *data)
{
	int node = 0;

	if (trie->act) {
		Act_list *act = palloc(act);

		act->act = trie->act;

		LIST_CONS(act, crawl->end);
		DLIST_RCONS(act, NULL);

		if (crawl->end)
			DLIST_RCONS(crawl->end, act);

		crawl->end = act;
		node = 1;
	}


	Trie *next = hmap_get(trie->simp, data, sizeof(*data));

	if (next)
		++data;
	else
		next = trie_abst(crawl, trie, data);

	if (!next)
		return node;

	return run(crawl, next, data);
}

void
do_acts(Act_list *list)
{
	if (list)
		do_acts(LIST_NEXT(list));
	else
		return;

	printf("%s", list->act);
}

int
main(int argc, char *argv[])
{
	Trie *root = trie_new("hello");
	Trie *sub1 = trie_new(" ");
	Trie *sub2 = trie_new("world");
	Trie *sub3 = trie_new("!");
	Crawl *crawl = crawl_new(root);

	trie_add_simp(root, 'a', sub1);
	trie_add_abst(sub1, sub2, sub3);
	run(crawl, root, "a");
	do_acts(crawl->end);
}
