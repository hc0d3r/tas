CFLAGS+=-Wall -Wextra -I../..
LDFLAGS+=-static -lutil

.PHONY: config.h
all: config.h ../../su

../../su: main.c ../../.obj/libtas.a config.h
	@echo "  CC su"
	@$(CC) $(CFLAGS) -o $@ main.c ../../.obj/libtas.a $(LDFLAGS)
	@strip $@

config.h:
	@../../configure su
