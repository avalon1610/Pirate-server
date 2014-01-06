SRC = main.cpp
TARGET = pppp
CFLAG = -g -I../ui -I../include -I../network -DDebug
LDFLAG = -L../lib -lui -pthread -lnetwork -lnet
SHELL = /bin/sh
LIB_DIR = ../lib/
BIN_DIR = ../bin/
UI_DIR = ../ui/
UI_LIB = ../lib/libui.a
NET_DIR = ../network/
NET_LIB = ../lib/libnetwork.a

CC = g++
AR = ar rc
RM = rm -f

.PHONY:all
all: $(BIN_DIR)$(TARGET)
	
$(BIN_DIR)$(TARGET):$(SRC) $(UI_LIB) $(NET_LIB)
	$(CC) $(CFLAG) $< -o $@ $(LDFLAG)

$(UI_LIB):$(UI_DIR)*.c
	cd $(UI_DIR);make

$(NET_LIB):$(NET_DIR)*.c
	cd $(NET_DIR);make

.PHONY:clean
clean:
	$(RM) *.o $(BIN_DIR)$(TARGET)
	cd $(UI_DIR);make clean
	cd $(NET_DIR);make clean
