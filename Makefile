##
# Odeus Lisp
#
# @file
# @version 0.1

CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -std=c23
INCLUDES = -Icore -Iinclude

SRC_DIR = core
TEST_DIR = tests
BUILD_DIR = build

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Lexer object file
$(BUILD_DIR)/odeus_lexer.o: $(SRC_DIR)/odeus_lexer.c $(SRC_DIR)/odeus_lexer.h | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_DIR)/odeus_lexer.c -o $(BUILD_DIR)/odeus_lexer.o

# Build and run tests
test: test_build
	./$(TEST_DIR)/lexer_test

.PHONY: test

# Build test executable
test_build: $(BUILD_DIR)/odeus_lexer.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_DIR)/lexer_test $(TEST_DIR)/odeus_lexer_test.c $(BUILD_DIR)/odeus_lexer.o

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TEST_DIR)/lexer_test

.PHONY: clean

# end
