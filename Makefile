CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -Wfloat-equal -Wpointer-arith \
	  -Wstrict-prototypes -Wwrite-strings -Wcast-qual -Wswitch-enum \
	  -Wconversion -fsanitize=undefined -g3 -Og

SRC_DIR := src
OBJ_DIR := obj

MAIN := $(SRC_DIR)/main.c
SRC := $(wildcard $(SRC_DIR)/*.c)
SRC := $(filter-out $(MAIN), $(SRC))
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
BIN := losh

CDEPS := $(patsubst %.c, %.d, $(SRC))

# general targets
.PHONY: all run clean clean_all

all: make_dirs $(BIN)

run: make_dirs $(BIN)
	@echo
	@./$(BIN)

clean:
	@rm -rf $(OBJ_DIR) $(BIN)

clean_all:
	@rm -rf $(OBJ_DIR) $(BIN) $(TESTS_OBJ_DIR) $(TESTS_BIN)

# bin compilation
.PHONY: make_dirs
make_dirs: $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir -p $@

$(BIN): $(MAIN) $(OBJ)
	@echo "  (LD) Linking executable '"$@"'"
	@$(CC) $^ $(CFLAGS) -o $@
	@echo "Compilation finished"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c Makefile
	@echo "  (CC) Compiling '"$<"'"
	@$(CC) -c $< $(CFLAGS) -MMD -MP -o $@

# tests compilation
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wfloat-equal -Wpointer-arith \
	    -Wwrite-strings -Wswitch-enum -Wconversion -fsanitize=undefined -O2
CXXLDFLAGS := -lgtest

TESTS_DIR := tests
TESTS_OBJ_DIR := $(TESTS_DIR)/obj
TESTS_INCLUDE := $(SRC_DIR)

TESTS_MAIN := $(TESTS_DIR)/main.cc
TESTS := $(wildcard $(TESTS_DIR)/*.cc)
TESTS := $(filter-out $(TESTS_MAIN), $(TESTS))
TESTS_OBJ := $(patsubst $(TESTS_DIR)/%.cc, $(TESTS_OBJ_DIR)/%.o, $(TESTS))
TESTS_BIN := $(TESTS_DIR)/main

CXXDEPS := $(patsubst %.cc, %.d, $(TESTS))

.PHONY: tests
tests: make_dirs $(OBJ) make_tests_dirs $(TESTS_BIN)
	@echo
	@./$(TESTS_BIN)

.PHONY: make_tests_dirs
make_tests_dirs: $(TESTS_OBJ_DIR)

$(TESTS_OBJ_DIR):
	@mkdir $@

$(TESTS_BIN): $(TESTS_MAIN) $(OBJ) $(TESTS_OBJ)
	@echo "  (LD) Linking tests executable '"$@"'"
	@$(CXX) $^ $(CXXLDFLAGS) $(CXXFLAGS) -o $@
	@echo "Compilation finished"

$(TESTS_OBJ_DIR)/%.o: $(TESTS_DIR)/%.cc Makefile
	@echo "  (CC) Compiling test '"$<"'"
	@$(CXX) -c $< -I$(TESTS_INCLUDE) $(CXXFLAGS) -MMD -MP -o $@

-include $(CDEPS)
-include $(CXXDEPS)
