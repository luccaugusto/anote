# anote - suckless note taking program
# See LICENSE file for copyright and license details

include config.mk

C_SRC=${wildcard ./src/*.c}
H_SRC=${wildcard ./src/*.h}
OBJ=${subst .c,.o,${subst src,build,${C_SRC}}}

all: options BD_DIR config ${PROJ_NAME}

options:
	@ echo ${PROJ_NAME} build options:
	@ echo "CFLAGS = ${CFLAGS}"
	@ echo

${PROJ_NAME}: ${OBJ}
	@ echo "building binary using ${CC} linker: $@"
	${CC} $^ -o $@ ${LIBS} ${DEBUG}
	@ echo '------------------------'
	@ echo 'finished building binary'
	@ echo

config:
	if [ -f ./src/$@.h ]; then rm -f ./src/$@.h; fi
	cp ./src/config.def.h ./src/$@.h
	chmod -w ./src/$@.h

./build/%.o: ./src/%.c ./src/%.h
	@ echo "[${CC}] $<"
	${CC} $< -c ${CFLAGS} -o $@
	@ echo ' '

./build/main.o: ./src/main.c ${H_SRC}
	@ echo "[${CC}] $<"
	${CC} $< -c ${CFLAGS} -o $@
	@ echo ' '

BD_DIR:
	mkdir -p build

install: ${PROJ_NAME}
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${PROJ_NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${PROJ_NAME}
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < ${PROJ_NAME}.1 > ${DESTDIR}${MANPREFIX}/man1/${PROJ_NAME}.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/${PROJ_NAME}.1
	tic -sx ${PROJ_NAME}.info

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/dwm\
		${DESTDIR}${MANPREFIX}/man1/dwm.1

clean:
	${RM} ${PROJ_NAME} ./build ./src/config.h

run:
	./${PROJ_NAME}

.PHONY: clean options all

