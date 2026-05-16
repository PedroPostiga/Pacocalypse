# name of the program (Minix service)
PROG=proj

.PATH: ${.CURDIR}/controller
.PATH: ${.CURDIR}/controller/rtc
.PATH: ${.CURDIR}/controller/timer
.PATH: ${.CURDIR}/controller/keyboard
.PATH: ${.CURDIR}/controller/mouse
.PATH: ${.CURDIR}/controller/videocard

# source code files to be compiled
SRCS = main.c rtc.c timer.c kbc.c mouse.c videocard.c utils.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic


# list of library dependencies 
DPADD += ${LIBLCF} 
LDADD += -llcf 


# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
