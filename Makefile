# bin variables
CC := gcc
CFLAGS := -std=c17 -Wall -Wextra
RELEASE ?= 0
ifeq ($(RELEASE), 0)
	CFLAGS += -fsanitize=undefined,address
	CFLAGS += -g3
	CFLAGS += -Og
else
	CFLAGS += -Werror
	CFLAGS += -D_FORTIFY_SOURCE=2
	CFLAGS += -O2
endif

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

MAIN := $(SRC_DIR)/main.c
SRC := $(wildcard $(SRC_DIR)/*.c)
SRC := $(filter-out $(MAIN), $(SRC))
ifeq ($(RELEASE), 0)
	OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/debug/%.o, $(SRC))
else
	OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/release/%.o, $(SRC))
endif
BIN := losh

# tests variables
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -D_FORTIFY_SOURCE=2 -O2
CXXLDFLAGS := -lgtest

TESTS_DIR := tests
TESTS_OBJ_DIR := $(TESTS_DIR)/obj
TESTS_BIN_DIR := $(TESTS_DIR)/bin
TESTS_INCLUDE := $(SRC_DIR)

TESTS_MAIN := $(TESTS_DIR)/main.cc
TESTS := $(wildcard $(TESTS_DIR)/*.cc)
TESTS := $(filter-out $(TESTS_MAIN), $(TESTS))
TESTS_OBJ := $(patsubst $(TESTS_DIR)/%.cc, $(TESTS_OBJ_DIR)/%.o, $(TESTS))
TESTS_BIN := $(TESTS_BIN_DIR)/main

# general targets
.PHONY: all run test clean

ifeq ($(RELEASE), 0)
all: debug

run: debug
	@./$(BIN_DIR)/debug/$(BIN)
else
all: release

run: release
	@./$(BIN_DIR)/release/$(BIN)
endif

clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(TESTS_OBJ_DIR) $(TESTS_BIN_DIR)

# debug compilation
.PHONY: debug
debug: make_debug_dirs $(BIN_DIR)/debug/$(BIN)

.PHONY: make_debug_dirs
make_debug_dirs: $(OBJ_DIR)/debug $(BIN_DIR)/debug

$(OBJ_DIR)/debug:
	@mkdir -p $@

$(BIN_DIR)/debug:
	@mkdir -p $@

$(BIN_DIR)/debug/$(BIN): $(MAIN) $(OBJ)
	$(CC) $^ $(CFLAGS) -o $@

$(OBJ_DIR)/debug/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< $(CFLAGS) -o $@

# release compilation
.PHONY: release
release: make_release_dirs $(BIN_DIR)/release/$(BIN)

.PHONY: make_release_dirs
make_release_dirs: $(OBJ_DIR)/release $(BIN_DIR)/release

$(OBJ_DIR)/release:
	@mkdir -p $@

$(BIN_DIR)/release:
	@mkdir -p $@

$(BIN_DIR)/release/$(BIN): $(MAIN) $(OBJ)
	$(CC) $^ $(CFLAGS) -o $@

$(OBJ_DIR)/release/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< $(CFLAGS) -o $@

# tests compilation
ifeq ($(RELEASE), 1)
test: release make_tests_dirs $(TESTS_BIN)
	@./$(TESTS_BIN)
endif

.PHONY: make_tests_dirs
make_tests_dirs: $(TESTS_OBJ_DIR) $(TESTS_BIN_DIR)

$(TESTS_OBJ_DIR):
	@mkdir $@

$(TESTS_BIN_DIR):
	@mkdir $@

$(TESTS_BIN): $(TESTS_MAIN) $(TESTS_OBJ)
	$(CXX) $^ $(OBJ) $(CXXLDFLAGS) $(CXXFLAGS) -o $@

$(TESTS_OBJ_DIR)/%.o: $(TESTS_DIR)/%.cc
	$(CXX) -c $< -I $(TESTS_INCLUDE) $(CXXFLAGS) -o $@
