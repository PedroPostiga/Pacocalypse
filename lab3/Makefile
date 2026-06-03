# name of the program (Minix service)
PROG=lab3 

# source code files to be compiled
SRCS = lab3.c kbc.c 

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic
CFLAGS += -DLAB3


# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -llcf -L/home/lcom/labs/lab2 -ltimer -L. -lkbd


LIB_NAME = libkbd.a
LIB_SRCS = kbc.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}


# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
