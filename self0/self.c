#include <stdio.h>
#include <stdlib.h>

typedef struct {
	size_t len;
	char *bytes;
} Msg;

typedef Msg Sym;

typedef struct {
	Msg *msg;
	size_t start;
	size_t end;
	Sym *sym;
} Synco;

typedef struct obj {
	struct obj *parent;
	void *rules;
} Obj;

void *
obj_message(Obj *obj, Msg *msg, Obj *self)
{
	if (obj->parent)
		obj_message(obj->parent, msg, self);
}

int
main(int argc, char *args[])
{
	Obj obj = { .parent = NULL };
	Msg msg;

	obj_message(&obj, &msg, &obj);
}
