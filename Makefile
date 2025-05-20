CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS = -lm
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c) search.c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c)) $(OBJ_DIR)/search.o
DEPS = $(wildcard $(INCLUDE_DIR)/*.h)

TARGET = arc

all: directories $(TARGET)

directories:
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/search.o: search.c $(DEPS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean directories