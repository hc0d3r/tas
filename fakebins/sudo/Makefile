CFLAGS+=-Wall -Wextra -I../..
LDFLAGS+=-static -lutil

.PHONY: config.h
all: config.h ../../sudo

../../sudo: main.c ../../.obj/libtas.a config.h
	@echo "  CC sudo"
	@$(CC) $(CFLAGS) -o $@ main.c ../../.obj/libtas.a $(LDFLAGS)
	@strip $@

config.h:
	@../../configure sudo
