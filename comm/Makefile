SRC = comm.c cJSON.c 
LIB = ../lib/libcomm.a
OBJ = comm.o cJSON.o 
CFLAGS = -I../include -I../network -DDEBUG

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
