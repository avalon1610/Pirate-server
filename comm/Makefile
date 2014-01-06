SRC = comm.c cJSON.c e4c_lite.c
LIB = ../lib/libcomm.a
OBJ = comm.o cJSON.o e4c_lite.o
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
