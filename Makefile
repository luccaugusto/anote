SHELL = /bin/sh
.POSIX:

# anote version
VERSION = 1.0.0

CC=gcc
LIBS=-lpanel -lncurses
RM=rm -rf
#flags
CFLAGS= -c -w -Wall


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
	${CC} $^ -o $@ ${LIBS}
	@ echo 'finished building binary'
	@ echo ' '

./build/%.o: ./src/%.c ./src/%.h
	@ echo "Building target using ${CC} compiler: $<"
	${CC} $< ${CFLAGS} -o $@ 
	@ echo ' '

./build/main.o: ./src/main.c ${H_SRC}
	@ echo "Building target using ${CC} compiler: $<"
	${CC} $< ${CFLAGS} -o $@
	@ echo ' '

BD_DIR:
	mkdir -p build

clean:
	${RM} ${PROJ_NAME} ./build

run:
	./${PROJ_NAME}

.PHONY: clean options all

