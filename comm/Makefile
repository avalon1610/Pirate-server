SRC = comm.c cJSON.c e4c.c
LIB = ../lib/libcomm.a
OBJ = comm.o cJSON.o e4c.o
CFLAGS = -I../include -DDEBUG

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
