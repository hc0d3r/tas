CFLAGS+=-Wall -Wextra -I../..
LDFLAGS+=-static -lutil

.PHONY: config.h
all: config.h ../../generic-keylogger

../../generic-keylogger: main.c ../../.obj/libtas.a config.h
	@echo "  CC generic-keylogger"
	@$(CC) $(CFLAGS) -o $@ main.c ../../.obj/libtas.a $(LDFLAGS)
	@strip $@

config.h:
	@../../configure generic-keylogger
