EXTERNAL_DIR ?= $(realpath ./)/external
TARGET_DIR ?= $(realpath ./)/bin
SRC_DIR = ./src

LIBRARIES = \
	-L$(EXTERNAL_DIR)/libxml2/lib -lxml2 \
	-lm

INCLUDES = \
	-I$(EXTERNAL_DIR)/libxml2/include/libxml2 \
	-I$(SRC_DIR) \

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g

SRC = $(wildcard $(SRC_DIR)/*.c) \
	$(wildcard $(SRC_DIR)/svg/*.c)

OBJ_DIR = build
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

TARGET = $(TARGET_DIR)/main

#
#
#

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(@D)
	$(CC) $(OBJS) $(INCLUDES) -o $@ $(LIBRARIES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


run-tests: tests
	./bin/tests

tests:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) clean -C ./tests
	$(MAKE) -C ./tests \
		MAIN_DIR=$(realpath ./) \
		EXTERNAL_INCLUDES="$(INCLUDES)" \
		EXTERNAL_LIBRARIES="$(LIBRARIES)"

clean:
	$(MAKE) clean -C ./tests
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean tests run-tests
