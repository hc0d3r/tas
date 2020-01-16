CFLAGS+=-Wall -Wextra -I../..
LDFLAGS+=-static -lutil

../../su: main.c ../../.obj/libtas.a
	@echo "  CC su"
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@strip $@
