#!/bin/bash
clear
make clean
make uninstall
echo "[=============================Building BotLib===============================]"
make install_lua
echo "[============================Building Complete==============================]"
make clean
#clear
