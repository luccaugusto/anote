# anote - suckless note taking program
# See LICENSE file for copyright and license details

include config.mk

C_SRC=${wildcard ./src/*.c}
H_SRC=${wildcard ./src/*.h}
OBJ=${subst .c,.o,${subst src,build,${C_SRC}}}

all: options BD_DIR ${PROJ_NAME}

options:
	@echo ${PROJ_NAME} build options:
	@echo "CFLAGS = ${CFLAGS}"
	@echo

${PROJ_NAME}: ${OBJ}
	@echo
	@echo "building $@ binary using ${CC} linker"
	@echo "Objects: $^"
	@echo "Libs: ${LIBS}"
	@${CC} $^ -o $@ ${LIBS}
	@echo '------------------------'
	@echo 'finished building binary'

config:
	@echo "config"
	cp ./src/config.def.h ./src/$@.h
	@echo

./build/%.o: ./src/%.c ./src/%.h
	@echo "[${CC}] $<"
	@ ${CC} $< -c ${CFLAGS} -o $@

./build/main.o: ./src/main.c ${H_SRC}
	@echo "[${CC}] $<"
	@ ${CC} $< -c ${CFLAGS} -o $@

BD_DIR:
	mkdir -p build

install: ${PROJ_NAME}
	@echo "install"
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${PROJ_NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${PROJ_NAME}
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < ${PROJ_NAME}.1 > ${DESTDIR}${MANPREFIX}/man1/${PROJ_NAME}.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/${PROJ_NAME}.1
	@echo

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${PROJ_NAME}\
		${DESTDIR}${MANPREFIX}/man1/${PROJ_NAME}.1

clean:
	${RM} ${PROJ_NAME} ./build ./src/config.h

run:
	./${PROJ_NAME}

.PHONY: clean options all

