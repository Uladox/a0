# See LICENSE file for copyright and license details.

PREFIX = /usr/local

LIBS = -lnit -lpthread

CFLAGS  = -std=gnu99 -pedantic -g -Wall -Wextra
LDFLAGS = -g $(LIBS)

CC = cc
