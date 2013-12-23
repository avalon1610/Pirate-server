PROG = ../bin/ui
EXE_SUFFIX = 
CFLAGS = -std=c99 -Os -W -Wall -pedantic -pthread -pipe -I. -I.. $(CFLAGS_EXTRA)
SOURCES = mongoose.c main.c
all: $(PROG) 

$(PROG):
	$(CC) -g $(SOURCES) -o $@ $(CFLAGS)

clean:
	rm $(PROG)
