CC = gcc
CFLAGS = -Wall -Wextra -g

SHELL_DIR = src/ForgeOS/Shellforge
SHELL_SRC = $(SHELL_DIR)/shell.c
SHELL_BIN = $(SHELL_DIR)/shellforge

all: shellforge

shellforge: $(SHELL_SRC)
	$(CC) $(CFLAGS) $(SHELL_SRC) -o $(SHELL_BIN)

run: shellforge
	./$(SHELL_BIN)

clean:
	rm -f $(SHELL_BIN) $(SHELL_DIR)/*.o

.PHONY: all shellforge run clean
