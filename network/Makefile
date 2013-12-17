PROG = ../bin/test
CFLAGS = -Os -W -Wall -lpcap -I. -I.. $(CFLAGS_EXTRA)
SOURCES = test.c
all: $(PROG)

$(PROG):
	$(CC) -g $(SOURCES) -o $@ $(CFLAGS)

