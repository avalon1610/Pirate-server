CFLAGS = -O0 -W -Wall -lpcap -lnet -pthread
SRC = arp.c network.c ethernet.c icmp.c
LIB = ../lib/libnetwork.a
OBJ = arp.o network.o ethernet.o icmp.o
CFLAGS = -I../include
	
CC = gcc
AR = ar rc
RM = rm -f
	
.PHONY:all
all: $(LIB) 
	
$(LIB):$(OBJ)
	$(AR) $(LIB) $^

$(OBJ):$(SRC)
	$(CC) -c $^ $(CFLAGS)
	
.PHONY:clean
clean:
	$(RM) $(LIB_DIR)$(LIB) *.o


