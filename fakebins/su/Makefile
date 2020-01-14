CFLAGS+=-Wall -Wextra -I../..
LDFLAGS+=-static -lutil

../../su: main.c ../../.obj/libtas.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	strip $@
