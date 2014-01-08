SRC = mongoose.c lib_ui.c 
LIB = ../lib/libui.a
OBJ = mongoose.o lib_ui.o
CFLAGS = -DE4C_THREADSAFE -I../include -I../network

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
