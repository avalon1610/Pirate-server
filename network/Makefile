PROG = ../bin/arp
CFLAGS = -O0 -W -Wall -lpcap -lnet -I./inculde -I.. $(CFLAGS_EXTRA)
SOURCES = ./arp.c ./network.c
all: $(PROG)

$(PROG):$(SOURCES)
	$(CC) -g $(SOURCES) -o $@ $(CFLAGS)

