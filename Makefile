BASIC_CFLAGS = -Wall -O3 -fPIC -g -std=c99 -pedantic -Ilua_headers -DCOLOR16_SUPPORT

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

all: rolibcore.so rolibservo.so rolibsensor.so rolibgpio.so rolib.so

LIB_H += $(wildcard *.h)

OBJECTS += src/core/rolibcore.o src/servo/rolibservo.o src/sensor/rolibsensor.o src/servo/servo.o src/gpio/rolibgpio.o src/rolib.o src/servo/maestro/maestro.o src/gpio/gpio.o

$(OBJECTS): $(LIB_H)

$(OBJECTS): %.o: %.c
	$(CC) -o $*.o -c $(ALL_CFLAGS) $<

rolibcore.so: src/core/rolibcore.o
	 $(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

rolibservo.so: src/servo/rolibservo.o src/servo/servo.o src/servo/maestro/maestro.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

rolibsensor.so: src/sensor/rolibsensor.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

rolibgpio.so: src/gpio/rolibgpio.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

rolib.so: src/rolib.o src/core/rolibcore.o src/servo/rolibservo.o src/sensor/rolibsensor.o src/servo/servo.o src/gpio/rolibgpio.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

maestro.so: src/servo/maestro/maestro.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

install_lua: all
	@echo "Installing"
	$(INSTALL_DATA) bin/rolibcore.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibcore.so
	$(INSTALL_DATA) bin/rolibservo.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibservo.so
	$(INSTALL_DATA) bin/rolibsensor.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibsensor.so
	$(INSTALL_DATA) bin/rolibgpio.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibgpio.so
	$(INSTALL_DATA) bin/rolib.so $(PREFIX)/lib/lua/$(LUA_VERSION)/rolib.so
	@echo "Installation Complete"

install_py:
	sudo python setup.py install

config:
	@echo "Configuring"
	mkdir -p $(BIN)
	@echo "Config Complete"

uninstall:
	@echo "Uninstalling"
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibcore.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibservo.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibsensor.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolibgpio.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/rolib.so
	@echo "Uninstall Complete"

tests_lua:
	lua tests/lua/example1.lua

tests_py:
	python tests/python/example1.py

test_lua_sys:
	clear
	lua tests/lua/sys.lua

reset:
	./reset.sh

cleaning:
	sudo ./cleaning.sh

tags:
	find . \( -name .git -type d -prune \) -o \( -name '*.[hc]' -type f -print \) | xargs ctags -a

clean:
	@echo "Cleaning"
	$(RM) bin/rolibcore.so
	$(RM) -r bin/rolibservo.so.dSYM
	$(RM) bin/rolibservo.so
	$(RM) -r bin/rolibservo.so.dSYM
	$(RM) bin/rolibsensor.so
	$(RM) -r bin/rolibsensor.so.dSYM
	$(RM) bin/rolibgpio.so
	$(RM) -r bin/rolibgpio.so.dSYM
	$(RM) bin/rolib.so
	$(RM) -r bin/rolib.so.dSYM
	$(RM) $(OBJECTS)
	@echo "Clean Complete"

.PHONY: all install config uninstall clean test tags
