LIB_NAME = test-shared
LIB_DIR = ./lib
MAIN_DIR ?= $(realpath ../../..)
EXTERNAL_DIR ?= $(realpath $(MAIN_DIR)/external)
SRC_DIR = ./

INCLUDES = \
	$(EXTERNAL_INCLUDES) \
	-I${SRC_DIR} \

LIBRARIES = \
	$(EXTERNAL_LIBRARIES)

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g

SRC = $(wildcard $(SRC_DIR)/*.c) \

OBJ_DIR = build
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

STATIC_LIB = $(LIB_DIR)/lib$(LIB_NAME).a
SHARED_LIB = $(LIB_DIR)/lib$(LIB_NAME).so

#
#
#

all: shared static

static: $(OBJS)
	mkdir -p $(LIB_DIR)
	ar rcs $(STATIC_LIB) $(OBJS)

shared: $(OBJS)
	mkdir -p $(LIB_DIR)
	$(CC) -shared -o $(SHARED_LIB) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(SHARED_LIB) $(STATIC_LIB)

.PHONY: all clean shared static
