BASIC_CFLAGS = -Wall -O3 -fPIC -g -std=c99 -pedantic -Ilua_src

ALL_CFLAGS = $(BASIC_CFLAGS) $(CFLAGS)

PREFIX = /usr/local
RM = rm -f
INSTALL = install -p
INSTALL_EXEC = $(INSTALL) -m 0755
INSTALL_DATA = $(INSTALL) -m 0644
LUA_VERSION = 5.3
MODULE_NAME = rolibcore

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S), Darwin)
	SHARELIB_FLAGS = -dynamiclib -Wl,-undefined,dynamic_lookup
else
	SHARELIB_FLAGS = --shared
endif

all: rolibcore.so rolibservo.so


LIB_H += $(wildcard *.h)

OBJECTS += src/rolibcore.o src/rolibservo.o

$(OBJECTS): $(LIB_H)

$(OBJECTS): %.o: %.c
	$(CC) -o $*.o -c $(ALL_CFLAGS) $<

rolibcore.so: $(OBJECTS)
	$(CC) $(SHARELIB_FLAGS) -o $@ $^

rolibservo.so: $(OBJECTS)
		$(CC) $(SHARELIB_FLAGS) -o $@ $^

install: all
	$(INSTALL_DATA) rolibcore.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibcore.so
	$(INSTALL_DATA) rolibservo.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibservo.so

uninstall:
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibcore.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibservo.so

test: all
	lua tests/demo.lua

tags:
	find . \( -name .git -type d -prune \) -o \( -name '*.[hc]' -type f -print \) | xargs ctags -a

clean:
	$(RM) rolibcore.so
	$(RM) -r rolibservo.so.dSYM
	$(RM) rolibservo.so
	$(RM) -r rolibservo.so.dSYM
	$(RM) $(OBJECTS)

.PHONY: all install uninstall clean test tags
