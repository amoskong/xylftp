# This is the makefile for server of xylftp.
# Copyright (C) Liu Yang, June 10, 2007.
# GPLv2 applies.

.PHONY:all clean

CC=gcc
CFLAGS= -Wall -W
LDFLAGS= -lssl -lcrypto

prepare:
	cp ../src/xylftp.h ./
	cp ../src/do_cmd.h ./

all: prepare do_stat read_configure test_parse_cmd do_mode test_do_pass

do_stat: do_stat.c 
	$(CC) $(CFLAGS) -DDEBUG -ggdb -o $@ $^ $(LDFLAGS)

read_configure: read_configure.c
	$(CC) $(CFLAGS) -DDEBUG -ggdb -o $@ $^ $(LDFLAGS)

test_parse_cmd:test_parse_cmd.c
	$(CC) $(CFLAGS) -DDEBUG -ggdb -o $@ $^ $(LDFLAGS)

do_mode: do_mode.c
	$(CC) $(CFLAGS) -DDEBUG -ggdb -o $@ $^ $(LDFLAGS)

test_do_pass: test_do_pass.c
	$(CC) $(CFLAGS) -DDEBUG -ggdb -o $@ $^ $(LDFLAGS)

clean:
	rm -f do_stat read_configure test_parse_cmd do_mode test_do_pass
	rm -f ./xylftp.h ./do_cmd.h
