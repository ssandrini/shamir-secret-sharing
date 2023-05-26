CC = gcc
CFLAGS = -Wall -std=c99
INCLUDE_DIR = includes
SRC_DIR = src
OBJ_DIR = obj

all: $(OBJ_DIR) bmp_main

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

bmp_main: $(OBJ_DIR)/main.o $(OBJ_DIR)/bmp.o
	$(CC) $(CFLAGS) -o bmp_main $(OBJ_DIR)/main.o $(OBJ_DIR)/bmp.o

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(INCLUDE_DIR)/bmp.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/bmp.o: $(SRC_DIR)/bmp.c $(INCLUDE_DIR)/bmp.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SRC_DIR)/bmp.c -o $(OBJ_DIR)/bmp.o

clean:
	rm -rf $(OBJ_DIR) bmp_main
