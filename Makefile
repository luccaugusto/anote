SHELL = /bin/sh
.POSIX:

# anote version
VERSION = 1.0.0

CONFIG_DIR=${XDG_CONFIG_HOME/.anote}

CC=gcc
LIBS=-lpanel -lncurses -lmenu
RM=rm -rf
#flags
CFLAGS= -c -w -Wall
#GDB DEBUG ENABLED
DEBUG=-g
#GDB DEBUG DISABLED
#DEBUG=

C_SRC=${wildcard ./src/*.c}
H_SRC=${wildcard ./src/*.h}
OBJ=${subst .c,.o,${subst src,build,${C_SRC}}}

PROJ_NAME = anote

all: clean options BD_DIR ${PROJ_NAME}

options:
	@ echo ${PROJ_NAME} build options:
	@ echo "CFLAGS = ${CFLAGS}"
	@ echo ' '

${PROJ_NAME}: ${OBJ}
	@ echo "building binary using ${CC} linker: $@"
	${CC} $^ -o $@ ${LIBS} ${DEBUG}
	@ echo 'finished building binary'
	@ echo ' '

./build/%.o: ./src/%.c ./src/%.h
	@ echo "Building target using ${CC} compiler: $<"
	${CC} $< ${CFLAGS} ${DEBUG} -o $@
	@ echo ' '

./build/main.o: ./src/main.c ${H_SRC}
	@ echo "Building target using ${CC} compiler: $<"
	${CC} $< ${CFLAGS} ${DEBUG} -o $@
	@ echo ' '

BD_DIR:
	mkdir -p build

clean:
	${RM} ${PROJ_NAME} ./build

run:
	./${PROJ_NAME}

.PHONY: clean options all

