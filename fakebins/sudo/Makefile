CFLAGS+=-Wall -Wextra -I../..
LDFLAGS+=-static

../../sudo: main.c ../../.obj/libtas.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	strip $@
