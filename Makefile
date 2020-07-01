.POSIX:
.SUFFIXES:
CC     = gcc
CFLAGS = -Wall
DEBUGFLAGS = -O0 -g -DDEBUG
LDLIBS = -lX11 -lcairo

all: bdialog
bdialog: bdialog.o ; $(CC) $(LDFLAGS) -o bdialog bdialog.o $(LDLIBS)
bdialog.o: bdialog.c bdialog.h config.h ; $(CC) -c $(CFLAGS) bdialog.c
debug: ; $(CC) $(LDFLAGS) $(DEBUGFLAGS) -o bdialog bdialog.o $(LDLIBS)
clean: ; rm -f bdialog bdialog.o