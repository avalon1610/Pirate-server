PROG = ../bin/arp
CFLAGS = -O0 -W -Wall -lpcap -lnet -I./include -I../include -I.. $(CFLAGS_EXTRA)
SOURCES = ./arp.c ./network.c ./ethernet.c ./icmp.c
all: $(PROG)

$(PROG):$(SOURCES)
	$(CC) -g $(SOURCES) -o $@ $(CFLAGS)

