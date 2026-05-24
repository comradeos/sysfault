CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2

BUILD_DIR := build
OUT := $(BUILD_DIR)/sysfault
SRC := src/main.c src/fault.c src/fault_catalog.c src/search.c src/translation.c src/translation_en.c src/translation_uk.c

.PHONY: all build run test clean

all: build

build: $(OUT)

$(OUT): $(SRC) src/fault.h src/fault_ids.h src/search.h src/translation.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
	$(OUT) EACCES

test: $(OUT)
	sh tests/test_basic.sh

clean:
	rm -rf $(BUILD_DIR)
