## COOL AND AWESOME MAKEFILE !!@ ##

# Options
CC=gcc
CFLAGS=-Wall -Wextra -g
OUTPUT_DIR=bin
OUTPUT_FILE=$(OUTPUT_DIR)/server
SRC_DIR=src

PORT=64198
VERBOSITY=2

# Getting all files to compile
SRC=$(wildcard $(SRC_DIR)/*.c)

# Object files (unused)
# OBJ=$(SRC:.c=.o)

all: setup compile run
debug: setup compile debugrun

setup:
	@clear
	@echo "Setting up... "
	@rm -rf $(OUTPUT_DIR)
	@mkdir -p $(OUTPUT_DIR)

compile:
	@echo "Compiling... "
	@$(CC) $(CFLAGS) -o $(OUTPUT_FILE) $(SRC)

run:
	@./bin/server $(PORT) $(VERBOSITY)

debugrun:
	@valgrind --leak-check=full \
		--show-leak-kinds=all \
         ./bin/server $(PORT) $(VERBOSITY)
        #  --track-origins=yes \
        #  --verbose \

clean:
	@echo "Cleaning up..."
	@rm -rf $(OUTPUT_DIR)

# Phony targets
.PHONY: all debug setup compile run debugrun clean
