EXTERNAL_DIR ?= $(realpath ../)/external
TARGET_DIR ?= $(realpath ../)/bin
SRC_DIR = ./src
MAIN_DIR ?= $(realpath ../)

INCLUDES = \
	$(EXTERNAL_INCLUDES) \
	-I$(MAIN_DIR)/src/geometry/include \
	-I$(MAIN_DIR)/src/svg/include \
	-I./src/shared/include \
	-I./src/svg/include \
	-I$(SRC_DIR) \

LIBRARIES = \
	-L$(MAIN_DIR)/src/svg/lib -lcustom-svg \
	-L$(MAIN_DIR)/src/geometry/lib -lgeometry-2d \
	-L./src/svg/lib -ltest-svg \
	-L./src/shared/lib -ltest-shared \
	$(EXTERNAL_LIBRARIES)

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g

SRC = $(wildcard $(SRC_DIR)/*.c)


OBJ_DIR = build
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

TARGET = $(TARGET_DIR)/tests

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
	$(MAKE) -C ./src/shared \
		MAIN_DIR=$(MAIN_DIR) \
		EXTERNAL_INCLUDES="$(EXTERNAL_INCLUDES)" \
		EXTERNAL_LIBRARIES="$(EXTERNAL_LIBRARIES)"
	$(MAKE) -C ./src/svg \
		MAIN_DIR=$(MAIN_DIR) \
		EXTERNAL_INCLUDES="$(EXTERNAL_INCLUDES)" \
		EXTERNAL_LIBRARIES="$(EXTERNAL_LIBRARIES)"

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	$(MAKE) clean -C ./src/shared
	$(MAKE) clean -C ./src/svg

.PHONY: all clean
