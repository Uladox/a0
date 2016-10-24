# See LICENSE file for copyright and license details.

include config.mk

SRC = server.c client.c kwt.c
OBJ = $(SRC:.c=.o)

all: client server

client: client.o
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

server: server.o kwt.o
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.c.o:
	@$(CC) $(CFLAGS) -c $<

clean:
	@rm -f server client $(OBJ)
