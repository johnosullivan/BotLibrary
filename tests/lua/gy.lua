#!/usr/bin/lua
--- Imports the required libraries from botlib
core   = require "botlibcore"
sensor = require "botlibsensor"

s = sensor.new("NAME","GY521",2,2)

print(s:read().sensor)
