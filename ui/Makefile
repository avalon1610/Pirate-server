SRC = mongoose.c lib_ui.c cJSON.c
LIB = ../lib/libui.a
OBJ = mongoose.o lib_ui.o cJSON.o
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
