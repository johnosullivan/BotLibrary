BASIC_CFLAGS = -Wall -O3 -fPIC -g -std=c99 -pedantic -Ilua_headers

ALL_CFLAGS = $(BASIC_CFLAGS) $(CFLAGS)

PREFIX = /usr/local
RM = rm -f
INSTALL = install -p
INSTALL_EXEC = $(INSTALL) -m 0755
INSTALL_DATA = $(INSTALL) -m 0644
LUA_VERSION = 5.3
MODULE_NAME = rolibcore
BIN=./bin/

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S), Darwin)
	SHARELIB_FLAGS = -dynamiclib -Wl,-undefined,dynamic_lookup
else
	SHARELIB_FLAGS = --shared
endif

all: rolibcore.so rolibservo.so


LIB_H += $(wildcard *.h)

OBJECTS += src/core/rolibcore.o src/servo/rolibservo.o

$(OBJECTS): $(LIB_H)

$(OBJECTS): %.o: %.c
	$(CC) -o $*.o -c $(ALL_CFLAGS) $<

rolibcore.so: src/core/rolibcore.o
	 $(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

rolibservo.so: src/servo/rolibservo.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

install: all
	@echo "Installing..."
	$(INSTALL_DATA) bin/rolibcore.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibcore.so
	$(INSTALL_DATA) bin/rolibservo.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibservo.so
	@echo "Installation Complete"

config:
	@echo "Configuring"
	mkdir -p $(BIN)
	@echo "Config Complete"

uninstall:
	@echo "Uninstalling..."
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibcore.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibservo.so
	@echo "Uninstall Complete"

test: all
	lua tests/demo.lua

tags:
	find . \( -name .git -type d -prune \) -o \( -name '*.[hc]' -type f -print \) | xargs ctags -a

clean:
	$(RM) bin/rolibcore.so
	$(RM) -r bin/rolibservo.so.dSYM
	$(RM) bin/rolibservo.so
	$(RM) -r bin/rolibservo.so.dSYM
	$(RM) $(OBJECTS)

.PHONY: all install uninstall clean test tags
