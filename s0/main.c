#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

enum s0_type {
	S0_VAL,
	S0_WORD,
	/* S0_REF */
};

typedef struct s0_trie S0_trie;

struct s0_trie {
	/* to keep things simple only 'a' or 'b' are allowed
	   or 'c' for leaf */
	S0_trie *a;
	S0_trie *b;
	int c;
};

typedef struct {
	enum s0_type type;
	union {
		int val;
		char *word;
		/* S0_trie *ref; */
	} dat;
} S0;

S0_trie *
s0_trie_new(S0_trie *a, S0_trie *b, int c)
{
	S0_trie *trie = malloc(sizeof(*trie));

	trie->a = a;
	trie->b = b;
	trie->c = c;

	return trie;
}

void
s0_trie_free(S0_trie *trie)
{
	if (trie->a)
		s0_trie_free(trie->a);

	if (trie->b)
		s0_trie_free(trie->b);

	free(trie);
}

int
s0_trie_get(S0_trie *trie, char *word)
{
	while (isspace(*word))
		++word;

	for (; *word != 'c'; ++word)
		trie = (*word == 'a') ? trie->a : trie->b;

	return trie->c;
}

void
s0_trie_define(S0_trie *trie, char *word, S0 **s0)
{
	int val = 0;
	S0_trie **trie_ref;

	for (; *s0; ++s0)
		switch ((*s0)->type) {
		case S0_VAL:
			val += (*s0)->dat.val;
			break;
		case S0_WORD:
			val += s0_trie_get(trie, (*s0)->dat.word);
			break;
		}

	while (isspace(*word))
		++word;

	for (; *word != 'c'; ++word) {
		trie_ref = (*word == 'a') ? &trie->a : &trie->b;

		if (!*trie_ref)
			trie = *trie_ref = s0_trie_new(NULL, NULL, 0);
		else
			trie = *trie_ref;
	}

	trie->c = val;
}

S0 *
s0_new(char **str)
{
	while (isspace(**str)) {
		if (**str == '\n')
			return NULL;

		++*str;
	}

	S0 *s0 = malloc(sizeof(*s0));

	if (isdigit(**str)) {
		s0->type = S0_VAL;
		s0->dat.val = strtol(*str, str, 10);
		return s0;
	}

	s0->type = S0_WORD;
	s0->dat.word = *str;

	/* skips past 'c' */
	for (; **str != 'c'; ++*str);
	++*str;

	return s0;
}

void
skip_word(char **str)
{
	for (; isspace(**str); ++*str);
	for (; !isspace(**str); ++*str);
}

void
repl(S0_trie *trie)
{
	static const char define[] = "define";
	char buf[256];
	char *buf_ptr;
	S0 *s0_arr[128];
	S0 **s0_ptr;

	while (1) {
		buf_ptr = buf;
		s0_ptr = s0_arr;
		printf("> ");

		if (!fgets(buf, sizeof(buf), stdin))
			break;

		if (!strncmp(define, buf, sizeof(define) - 1)) {
			buf_ptr += sizeof(define) - 1;
			skip_word(&buf_ptr);

			for (; *s0_ptr = s0_new(&buf_ptr); ++s0_ptr);

			s0_trie_define(trie, buf + sizeof(define) - 1, s0_arr);

			for (s0_ptr = s0_arr; *s0_ptr; ++s0_ptr)
				free(*s0_ptr);

			continue;
		}

		printf("%i\n", s0_trie_get(trie, buf_ptr));
	}
}

int
main(int argc, char *argv[])
{
	S0_trie *trie = s0_trie_new(NULL, NULL, 0);
	S0 *trie_abc[] = {
		[0] = &(S0) {
			.type = S0_VAL,
			.dat.val = -1
		},

		[1] = NULL
	};
	S0 *trie_bac[] = {
		[0] = &(S0) {
			.type = S0_VAL,
			.dat.val = 1
		},

		[1] = NULL
	};

	s0_trie_define(trie, "abc", trie_abc);
	s0_trie_define(trie, "bac", trie_bac);
	repl(trie);
	s0_trie_free(trie);

	/* printf("%i\n", s0_trie_get(trie, "bbc")); */
}
