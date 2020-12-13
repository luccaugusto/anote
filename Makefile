# anote - suckless note taking program
# See LICENSE file for copyright and license details

include config.mk

C_SRC=${wildcard ./src/*.c}
H_SRC=${wildcard ./src/*.h}
OBJ=${subst .c,.o,${subst src,build,${C_SRC}}}

all: clean options BD_DIR ${PROJ_NAME}

options:
	@ echo ${PROJ_NAME} build options:
	@ echo "CFLAGS = ${CFLAGS}"
	@ echo

${PROJ_NAME}: config.h ${OBJ}
	@ echo "building binary using ${CC} linker: $@"
	${CC} $^ -o $@ ${LIBS} ${DEBUG}
	@ echo '------------------------'
	@ echo 'finished building binary'
	@ echo

config.h:
	cp ./src/config.def.h ./src/$@

./build/%.o: ./src/%.c ./src/%.h
	@ echo "Building target using ${CC} compiler: $<"
	${CC} $< -c ${CFLAGS} -o $@
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

