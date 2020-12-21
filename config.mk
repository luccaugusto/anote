SHELL = /bin/sh
.POSIX:

# anote version
VERSION = 1.0
PROJ_NAME = anote

#paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

#includes and libs
LIBS=-lpanel -lncurses -lmenu

#flags
#no debug:
CFLAGS= -g -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os
#debug:
#CFLAGS= -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os

#compiler and linker
CC=gcc
#cleaning program
RM=rm -rf
