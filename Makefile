CC = cc
CFLAGS = -O2 -Wall -Wextra
LDFLAGS = -lncurses
DEBUGFLAGS = -g -Og -Wall -Wextra
PREFIX = /usr/local
SRC = snake.c
BINS = snake snake-debug

all: snake

snake: ${SRC}
	${CC} ${CFLAGS} ${LDFLAGS} $< -o $@

debug: ${SRC}
	${CC} ${DEBUGFLAGS} ${LDFLAGS} $< -o snake-debug

clean:
	rm -rf ${BINS}

install: snake
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp snake ${DESTDIR}${PREFIX}/bin
	chmod 711 ${DESTDIR}${PREFIX}/bin/snake

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/snake

.PHONY = all clean debug install uninstall

