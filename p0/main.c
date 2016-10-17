#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define NIT_SHORT_NAMES
#include <nit/palloc.h>
#include <nit/list.h>
#include <nit/hset.h>
#include <nit/hmap.h>

typedef struct {
	char *str;
        int size;
	int pos;
} Token_plc;

typedef struct {
	const char *name;
	int pos;
} Doc;

typedef struct {
	Nit_list list;
	Doc *doc;
} Doc_list;

typedef struct {
	Doc_list *list;
} Doc_table;

char *
skip_seps(char *ptr, int *cnt)
{
	while (1)
		switch (*ptr) {
		case ' ':
		case '\n':
			++*cnt;
			++ptr;
		default:
			return ptr;
		}
}

char *
next_token(char *str, int *len, int *cnt)
{
	char *ptr = str;

	while (1)
		switch (*ptr) {
		case ' ':
		case '\n':
			*len = ptr - str;
			return skip_seps(ptr, cnt);
		case '\0':
			*len = ptr - str;
			return ptr;
		default:
			++*cnt;
			++ptr;
		}
}

Token_plc *
new_token(char **str, int *cnt, Token_plc *t)
{
	char *end;
	int len;

	end = next_token(*str, &len, cnt);

	if (end == *str)
		return NULL;

	t->str = *str;
	t->size = len;
	t->pos = *cnt;
	*str = end;
	return t;
}

void
add_doc(Nit_hmap *index, const char *doc_name)
{
	Token_plc t;
	FILE *fp = fopen(doc_name, "r");
	char buff[256];
	char *str = buff;
	int cnt = 0;

	fgets(buff, 255, fp);

	while (new_token(&str, &cnt, &t)) {
		Doc_table *table = hmap_get(index, t.str, t.size);

		if (!table) {
			table = palloc(table);
			table->list = NULL;
			hmap_add(index, t.str, t.size, table);
		}

		Doc *d = palloc(d);
		Doc_list *dl = palloc(dl);

		d->name = doc_name;
		d->pos = t.pos;
		dl->doc = d;
		LIST_CONS(dl, table->list);
		table->list = dl;
	}
}

int
main(int argc, char *argv[])
{
	Nit_hmap *map = hmap_new(0);

	add_doc(map, "test.piq");
	add_doc(map, "test2.piq");

	Doc_table *dt = hmap_get(map, argv[1], strlen(argv[1]));
	Doc_list *dl = dt->list;
	Doc *d = dl->doc;

	printf("name: %s, pos %i\n", d->name, d->pos);
}
