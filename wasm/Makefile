ifndef WASI_SDK_PATH
$(error WASI_SDK_PATH is not set)
endif

MAIN_DIR ?= $(realpath ..)
WASM_DIR ?= dist
SRC_DIR ?= src
OBJ_DIR ?= build/wasm
LIB_DIR ?= build/lib

CC = $(WASI_SDK_PATH)/bin/clang
AR = $(WASI_SDK_PATH)/bin/ar

WASM_OUT = $(WASM_DIR)/svg-to-pdf.wasm
LIBXML2_A = $(LIB_DIR)/libxml2.a

INCLUDES = \
	-I$(SRC_DIR) \
	-I$(SRC_DIR)/libxml2-wasi \
	-I$(MAIN_DIR)/src/pdf/src \
	-I$(MAIN_DIR)/src/pdf/include \
	-I$(MAIN_DIR)/src/geometry/src \
	-I$(MAIN_DIR)/src/geometry/include \
	-I$(MAIN_DIR)/src/svg/src \
	-I$(MAIN_DIR)/src/svg/include \
	-I$(MAIN_DIR)/src/bounding-box/src \
	-I$(MAIN_DIR)/src/bounding-box/include \
	-I$(MAIN_DIR)/external/libxml2/include \
	-I$(MAIN_DIR)/external/libxml2/include/libxml2

CFLAGS = \
	--target=wasm32-wasi \
	--sysroot=$(WASI_SDK_PATH)/share/wasi-sysroot \
	-include $(SRC_DIR)/libxml2-wasi/compat.h \
	-Ddup=xml2_wasi_dup \
	-Wall \
	-Wextra \
	-O2 \
	-fvisibility=hidden

LDFLAGS = \
	--target=wasm32-wasi \
	--sysroot=$(WASI_SDK_PATH)/share/wasi-sysroot \
	-nostartfiles \
	-Wl,--no-entry \
	-Wl,--gc-sections \
	-Wl,--export=svg_to_pdf_alloc \
	-Wl,--export=svg_to_pdf_free \
	-Wl,--export=svg_to_pdf_from_memory \
	-Wl,--export=svg_to_pdf_page_svg_from_memory \
	-Wl,--export=svg_to_pdf_page_svgs_from_memory \
	-Wl,--export-memory

SRC = $(SRC_DIR)/svg_to_pdf_wasm.c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

PROJECT_SRC = \
	$(wildcard $(MAIN_DIR)/src/geometry/src/*.c) \
	$(wildcard $(MAIN_DIR)/src/svg/src/*.c) \
	$(wildcard $(MAIN_DIR)/src/pdf/src/*.c) \
	$(wildcard $(MAIN_DIR)/src/bounding-box/src/*.c)

PROJECT_OBJS = $(patsubst $(MAIN_DIR)/src/%.c,$(OBJ_DIR)/project/%.o,$(PROJECT_SRC))

LIBXML2_SRC_DIR = $(MAIN_DIR)/external/libxml2
LIBXML2_SRC = \
	$(SRC_DIR)/libxml2-wasi/compat.c \
	buf.c \
	c14n.c \
	catalog.c \
	chvalid.c \
	dict.c \
	entities.c \
	encoding.c \
	error.c \
	globals.c \
	hash.c \
	HTMLparser.c \
	HTMLtree.c \
	list.c \
	parser.c \
	parserInternals.c \
	pattern.c \
	relaxng.c \
	SAX2.c \
	schematron.c \
	threads.c \
	tree.c \
	uri.c \
	valid.c \
	xinclude.c \
	xlink.c \
	xmlIO.c \
	xmlmemory.c \
	xmlmodule.c \
	xmlreader.c \
	xmlregexp.c \
	xmlsave.c \
	xmlschemas.c \
	xmlschemastypes.c \
	xmlstring.c \
	xmlunicode.c \
	xmlwriter.c \
	xpath.c \
	xpointer.c

LIBXML2_OBJS = \
	$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%.c,$(LIBXML2_SRC))) \
	$(patsubst %.c,$(OBJ_DIR)/libxml2/%.o,$(filter-out $(SRC_DIR)/%.c,$(LIBXML2_SRC)))

all: wasm

wasm: $(WASM_OUT)

project-objects: $(PROJECT_OBJS)

libxml2-wasi: $(LIBXML2_A)

$(LIBXML2_A): $(LIBXML2_OBJS)
	mkdir -p $(@D)
	$(AR) rcs $@ $^

$(WASM_OUT): $(OBJS) $(PROJECT_OBJS) $(LIBXML2_A)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(OBJS) $(PROJECT_OBJS) $(LIBXML2_A) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/project/%.o: $(MAIN_DIR)/src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/libxml2/%.o: $(LIBXML2_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -DLIBXML_STATIC -c $< -o $@

clean:
	rm -rf build $(WASM_OUT)

.PHONY: all wasm project-objects libxml2-wasi clean
