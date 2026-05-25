# name of the program (Minix service)
PROG=lab4 

# source code files to be compiled
SRCS = lab4.c mouse.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic
CFLAGS += -DLAB4


# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -llcf -L/home/lcom/labs/lab2 -ltimer -L. -lmouse

LIB_NAME = libmouse.a
LIB_SRCS = mouse.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}


# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
