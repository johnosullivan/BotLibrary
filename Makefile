BASIC_CFLAGS = -Wall -O3 -fPIC -g -std=c99 -pedantic -Ilua_headers -DCOLOR16_SUPPORT

ALL_CFLAGS = $(BASIC_CFLAGS) $(CFLAGS)

PREFIX = /usr/local
RM = rm -f
INSTALL = install -p
INSTALL_EXEC = $(INSTALL) -m 0755
INSTALL_DATA = $(INSTALL) -m 0644
LUA_VERSION = 5.3
MODULE_NAME = botlibcore
BIN=./bin/
COMDOC=./botlibdoc/

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S), Darwin)
	SHARELIB_FLAGS = -dynamiclib -Wl,-undefined,dynamic_lookup
else
	SHARELIB_FLAGS = --shared
endif

all: botlibcore.so botlibservo.so botlibsensor.so botlibgpio.so botlib.so

LIB_H += $(wildcard *.h)

OBJECTS += src/core/botlibcore.o src/servo/botlibservo.o src/sensor/botlibsensor.o src/servo/servo.o src/gpio/botlibgpio.o src/botlib.o src/servo/maestro/maestro.o src/gpio/gpio.o src/thpool.o src/threadpool.o

$(OBJECTS): $(LIB_H)

$(OBJECTS): %.o: %.c
	$(CC) -o $*.o -c $(ALL_CFLAGS) $<

botlibcore.so: src/core/botlibcore.o src/gpio/gpio.o
	 $(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

botlibservo.so: src/servo/botlibservo.o src/servo/servo.o src/servo/maestro/maestro.o src/gpio/gpio.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

botlibsensor.so: src/sensor/botlibsensor.o src/sensor/sensor.o src/gpio/gpio.o src/thpool.o src/threadpool.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

botlibgpio.so: src/gpio/botlibgpio.o src/gpio/gpio.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

botlib.so: src/botlib.o src/core/botlibcore.o src/servo/servo.o src/servo/botlibservo.o src/servo/maestro/maestro.o src/sensor/botlibsensor.o src/sensor/sensor.o src/gpio/botlibgpio.o src/gpio/gpio.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

maestro.so: src/servo/maestro/maestro.o
		$(CC) $(SHARELIB_FLAGS) -o $(BIN)$@ $^

install_lua: all
	@echo "Installing"
	mkdir -p $(PREFIX)/lib/lua/$(LUA_VERSION)/$(COMDOC)
	cp -r src/component_docs/* $(PREFIX)/lib/lua/$(LUA_VERSION)/$(COMDOC)
	$(INSTALL_DATA) bin/botlibcore.so $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibcore.so
	$(INSTALL_DATA) bin/botlibservo.so $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibservo.so
	$(INSTALL_DATA) bin/botlibsensor.so $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibsensor.so
	$(INSTALL_DATA) bin/botlibgpio.so $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibgpio.so
	$(INSTALL_DATA) bin/botlib.so $(PREFIX)/lib/lua/$(LUA_VERSION)/botlib.so
	@echo "Installation Complete"

install_py:
	sudo python setup.py install

config:
	@echo "Configuring"
	mkdir -p $(BIN)
	@echo "Config Complete"

uninstall:
	@echo "Uninstalling"
	rm -rf $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibdoc
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibcore.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibservo.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibsensor.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/botlibgpio.so
	$(RM) $(PREFIX)/lib/lua/$(LUA_VERSION)/botlib.so
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
	$(RM) bin/botlibcore.so
	$(RM) -r bin/botlibservo.so.dSYM
	$(RM) bin/botlibservo.so
	$(RM) -r bin/botlibservo.so.dSYM
	$(RM) bin/botlibsensor.so
	$(RM) -r bin/botlibsensor.so.dSYM
	$(RM) bin/botlibgpio.so
	$(RM) -r bin/botlibgpio.so.dSYM
	$(RM) bin/botlib.so
	$(RM) -r bin/botlib.so.dSYM
	$(RM) $(OBJECTS)
	@echo "Clean Complete"

.PHONY: all install config uninstall clean test tags
