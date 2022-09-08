# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

CC = gcc                        # compiler to use
CFLAGS = -lncurses -g -Wall -Wextra

SRCS := $(wildcard *.c)
BINS := $(SRCS:%.c=%)

all: ${BINS}

%: %.o
	${CC} ${CFLAGS} $< -o $@

%.o: %.c
	${CC} -c $<

clean:
	rm -rvf *.o ${BINS}

