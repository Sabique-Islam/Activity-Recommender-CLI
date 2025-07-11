CC = gcc
CFLAGS = -Wall -Wextra -I./include -I./cjson -pthread
LDFLAGS = -lm -pthread -L/opt/homebrew/opt/libmicrohttpd/lib -lmicrohttpd

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

SRCS = main.c \
       $(SRC_DIR)/api.c \
       $(SRC_DIR)/config.c \
       $(SRC_DIR)/http_server.c \
       $(SRC_DIR)/json_handler.c \
       $(SRC_DIR)/recommender.c \
       $(SRC_DIR)/utils.c \
       $(SRC_DIR)/activities.c \
       $(SRC_DIR)/embeddings.c \
       cjson/cJSON.c

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

TARGET = arc-server

.PHONY: all clean directories

all: directories $(TARGET)

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(SRC_DIR)
	@mkdir -p $(BUILD_DIR)/cjson

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

install: all
	@mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/

run: all
	./$(TARGET)