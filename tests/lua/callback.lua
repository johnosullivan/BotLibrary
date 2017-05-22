#!/usr/bin/lua
--- Imports the required libraries from botlib
core   = require "botlibcore"
sensor = require "botlibsensor"
--- Simple sleep function that allows the current thread to sleep
function sleep(n) os.execute("sleep " .. tonumber(n)) end
--- The range sensor call_back which will print sensor object and value
function range_call_back1 (data) print("SData: " .. data.sensor) print("Value: " .. data.value) end
function range_call_back2 (data) print("SData: " .. data.sensor) print("Value: " .. data.value) end
--- Creates an instance of sensor for HC-SR04 component
a = sensor.new("Range#1","HC-SR04",18,24)
--- Set the call_back of the senor to the range_call_back function
a:setCallBack("range_call_back1")
--- Creates second instance of sensor for HC-SR04 component
b = sensor.new("Range#2","HC-SR04",18,24)
--- Set the call_back of the senor to the range_call_back function
b:setCallBack("range_call_back2")
--- Convert sensor object to string to display the details
print(a)
--- print(b)
print(a:read())
print(a:info())
--- print(b:read())
--- sleep(5)
