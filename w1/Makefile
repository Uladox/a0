# See LICENSE file for copyright and license details.

include config.mk

SRC = main.c
OBJ = $(SRC:.c=.o)

all: w1

w1: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

.c.o:
	@$(CC) $(CFLAGS) -c $<

clean:
	@rm -f w1 $(OBJ)
