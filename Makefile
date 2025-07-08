EXTERNAL_DIR ?= $(realpath ./)/external
TARGET_DIR ?= $(realpath ./)/bin
SRC_DIR = ./src

INCLUDES = \
	-I$(EXTERNAL_DIR)/libxml2/include/libxml2 \
	-I$(SRC_DIR)/geometry/include \
	-I$(SRC_DIR)/svg/include \
	-I$(SRC_DIR) \

LIBRARIES = \
	-L$(SRC_DIR)/svg/lib -lcustom-svg \
	-L$(SRC_DIR)/geometry/lib -lgeometry-2d \
	-L$(EXTERNAL_DIR)/libxml2/lib -lxml2 \
	-lm

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ_DIR = build
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

TARGET = $(TARGET_DIR)/main

#
#
#

all: submodules $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(@D)
	$(CC) $(OBJS) $(INCLUDES) -o $@ $(LIBRARIES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

submodules:
	$(MAKE) -C $(SRC_DIR)/geometry MAIN_DIR=$(realpath ./)
	$(MAKE) -C $(SRC_DIR)/svg MAIN_DIR=$(realpath ./)

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
	$(MAKE) clean -C $(SRC_DIR)/geometry
	$(MAKE) clean -C $(SRC_DIR)/svg
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean tests run-tests
