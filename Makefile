SHELL = /bin/sh
.POSIX:

# anote version
VERSION = 1.0.0

CC=gcc
RM=rm -rf

#flags
LCCFLAGS= -c -w -Wall

PROJ_NAME = anote

all: clean options $(PROJ_NAME)

options:
	@echo LC build options:
	@echo "CFLAGS = $(LCCFLAGS)"

$(PROJ_NAME):
	$(CC) -o $@ src/$(PROJ_NAME).c

%.o: %.c %.h
	$(CC) -o $@ $< $(LCCFLAGS)

clean:
	$(RM) $(PROJ_NAME) *.o

run:
	./$(PROJ_NAME)

.PHONY: clean options all

