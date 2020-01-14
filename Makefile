CFLAGS+=-Wall -Wextra -I$(PWD)

TAS = \
		getfullpath.o globals.o tas-execv.o \
		tty.o xreadlink.o getinode.o

OBJS = $(addprefix .obj/, $(TAS))

.PHONY: all
all: .obj/libtas.a sudo su fun/leet-shell

.obj/libtas.a: $(OBJS)
	ar cvr $@ $^

# __dependencies__

.obj/getfullpath.o: .obj/globals.o
.obj/tas-execv.o: .obj/globals.o .obj/getinode.o
.obj/tty.o: LDFLAGS+=-l:util.a

.obj/%.o: tas/%.c tas/%.h

# __end__

.obj/%.o: tas/%.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

.PHONY: sudo su
sudo: .obj/libtas.a
	$(MAKE) -C fakebins/sudo

su: .obj/libtas.a
	$(MAKE) -C fakebins/su

fun/leet-shell: fun/leet-shell.c .obj/libtas.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -static -lutil

clean:
	rm -f $(OBJS) sudo su fun/leet-shell
