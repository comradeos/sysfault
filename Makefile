CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2

BUILD_DIR := build
OUT := $(BUILD_DIR)/sysfault
SRC := src/main.c src/fault.c src/errno_data.c src/signal_data.c src/search.c

.PHONY: all build run test clean

all: build

build: $(OUT)

$(OUT): $(SRC) src/fault.h src/search.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
	$(OUT) EACCES

test: $(OUT)
	sh tests/test_basic.sh

clean:
	rm -rf $(BUILD_DIR)
