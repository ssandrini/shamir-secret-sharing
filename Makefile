CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -fsanitize=address
INCLUDE_DIR = includes
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

all: $(OBJ_DIR) bmp_main

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

bmp_main: $(OBJ_DIR)/main.o $(OBJ_DIR)/bmp.o
	$(CC) $(CFLAGS) -o $(BIN_DIR)/bmp_main $(OBJ_DIR)/main.o $(OBJ_DIR)/bmp.o

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(INCLUDE_DIR)/bmp.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/bmp.o: $(SRC_DIR)/bmp.c $(INCLUDE_DIR)/bmp.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/bmp.c -o $(OBJ_DIR)/bmp.o

clean:
	rm -rf $(OBJ_DIR) bmp_main
