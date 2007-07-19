# This is the makefile for server of xylftp.
# Copyright (C) Liu Yang, June 10, 2007.
# GPLv2 applies.

.PHONY:all clean

CC=gcc
CFLAGS= -Wall -W
LDFLAGS= -lssl -lpthread

all: do_stat read_configure test_parse_cmd do_mode test_do_pass

do_stat: do_stat.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -DDEGUG -ggdb -o $@ $^

read_configure: read_configure.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DDEBUG -ggdb -o $@ $^

test_parse_cmd:test_parse_cmd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DDEBUG -ggdb -o $@ $^

do_mode: do_mode.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DDEBUG -ggdb -o $@ $^

test_do_pass: test_do_pass.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DDEBUG -ggdb -o $@ $^

clean:
	rm -f do_stat read_configure test_parse_cmd do_mode test_do_pass
