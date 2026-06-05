# name of the program (Minix service)
PROG=lab1

# source code files to be compiled
SRCS = main.c bitwise.c rtc.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies
DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -llcf -L. -lrtc

LIB_NAME = librtc.a
LIB_SRCS = rtc.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
