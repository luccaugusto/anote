SHELL = /bin/sh
.POSIX:

# anote version
VERSION = 1.0
PROJ_NAME = anote


#paths
CONFIG_DIR=${XDG_CONFIG_HOME/anote}
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

#includes and libs
LIBS=-lpanel -lncurses -lmenu

#flags
CFLAGS= -c -w -Wall
CFLAGS   = -g -std=c99 -pedantic -Wall -O0
#CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os

#compiler and linker
CC=gcc
#cleaning program
RM=rm -rf
