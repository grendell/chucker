ifeq ($(OS), Windows_NT)
	BIN_DIR = win
	BIN_EXT = .exe
else
	BIN_DIR = $(shell uname -s)
endif

SRC_PATH = src
BIN_PATH = bin/$(BIN_DIR)

CC = clang
CC_FLAGS = -O2 -Wall -Wextra -Werror -std=c99

ifeq ($(OS), Windows_NT)
	CC_FLAGS += -D_CRT_SECURE_NO_WARNINGS
endif

.PHONY: all
all: $(BIN_PATH) $(BIN_PATH)/chucker$(BIN_EXT)

$(BIN_PATH):
	mkdir -p "$(BIN_PATH)"

$(BIN_PATH)/chucker$(BIN_EXT): $(SRC_PATH)/chucker.c
	$(CC) $(CC_FLAGS) $(SRC_PATH)/chucker.c -o $(BIN_PATH)/chucker$(BIN_EXT)

.PHONY: clean
clean:
	rm -fr "$(BIN_PATH)"