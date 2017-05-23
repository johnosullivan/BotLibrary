#!/usr/bin/lua
--- Imports the required libraries from botlib
sensor = require "botlibsensor"
human = sensor.new("HumanSensor#1","WAPIRS",18)
print(human)
print(human:read())
print(human:info())
