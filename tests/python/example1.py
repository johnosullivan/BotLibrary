#!/usr/bin/Python

print "Running Test Example1: Started"

import pyrolib
from pyrolib import gpio

core_module = pyrolib

gpio_module = pyrolib.gpio

print "Version:"
print "Core: " + str(core_module.get_version())
print "GPIO: " + str(gpio_module.get_version())

print "Running Test Example1: Completed"

exit()
