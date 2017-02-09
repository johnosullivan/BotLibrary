#!/bin/bash

# CLeaning the building of both framework

echo "Cleaning Lua and Python frameworks"
rm -rf pyrolib.egg-info
rm -rf build
rm -rf dist
echo "Deleting pyc files in python src"
cd pyrolib
rm -r *.pyc
#########
cd gpio
rm -r *.pyc
cd ..
#########
cd ..
echo "Removing Lua module builds"
make clean
echo "Done"
