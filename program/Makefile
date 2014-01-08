SRC = main.c
TARGET = pppp
CFLAG = -g -DDEBUG -I../ui -I../include -I../network 
LDFLAG = -L../lib -lc -lm -lui -pthread -lnetwork -lnet -lcomm
SHELL = /bin/sh
LIB_DIR = ../lib/
BIN_DIR = ../bin/
COMM_DIR = ../comm/
COMM_LIB = ../lib/libcomm.a
UI_DIR = ../ui/
UI_LIB = ../lib/libui.a
NET_DIR = ../network/
NET_LIB = ../lib/libnetwork.a

CC = gcc
AR = ar rc
RM = rm -f

.PHONY:all
all: $(BIN_DIR)$(TARGET)
	
$(BIN_DIR)$(TARGET):$(SRC) $(COMM_LIB) $(UI_LIB) $(NET_LIB)
	$(CC) $(CFLAG) $< -o $@ $(LDFLAG)

$(UI_LIB):$(UI_DIR)*.c
	cd $(UI_DIR);make

$(NET_LIB):$(NET_DIR)*.c
	cd $(NET_DIR);make

$(COMM_LIB):$(COMM_DIR)*.c
	cd $(COMM_DIR);make

.PHONY:clean
clean:
	$(RM) *.o $(BIN_DIR)$(TARGET)
	cd $(UI_DIR);make clean
	cd $(NET_DIR);make clean
	cd $(COMM_DIR);make clean
