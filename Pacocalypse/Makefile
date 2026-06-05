# name of the program (Minix service)
PROG=proj

.PATH: ${.CURDIR}/model
.PATH: ${.CURDIR}/model/game
.PATH: ${.CURDIR}/model/map
.PATH: ${.CURDIR}/model/player
.PATH: ${.CURDIR}/model/ghost
.PATH: ${.CURDIR}/model/input
.PATH: ${.CURDIR}/model/control
.PATH: ${.CURDIR}/view/renderer
.PATH: ${.CURDIR}/view/resources
.PATH: ${.CURDIR}/view/ui
.PATH: ${.CURDIR}/view/

# source code files to be compiled
SRCS = main.c game.c map.c player.c ghost.c control.c renderer.c sprite.c font.c button.c hud.c input.c menu.c pause.c resources.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic


# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf -L/home/lcom/labs/lab5 -lvideocard -L/home/lcom/labs/lab4 -lmouse -L/home/lcom/labs/lab3 -lkbd -L/home/lcom/labs/lab2 -ltimer


# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
