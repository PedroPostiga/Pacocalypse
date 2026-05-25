# name of the program (Minix service)
PROG=lab5

# source code files to be compiled
SRCS = lab5.c videocard.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic
CFLAGS += -DLAB5


# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF} 
LDADD += -llcf  -L/home/lcom/labs/lab3 -lkbd -L/home/lcom/labs/lab2 -ltimer -lvideocard

LIB_NAME = libvideocard.a
LIB_SRCS = videocard.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}


# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
