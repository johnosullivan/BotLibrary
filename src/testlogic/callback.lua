#!/usr/bin/lua
--- Imports the required libraries from botlib
core = require "botlibcore"
sensor = require "botlibsensor"
--- Simple sleep function that allows the current thread to sleep
function sleep(n) os.execute("sleep " .. tonumber(n)) end
--- The range sensor call_back which will print sensor object and value
function range_call_back1 (data) print("SData: " .. data.sensor) print("Value: " .. data.value) end
function range_call_back2 (data) print("SData: " .. data.sensor) print("Value: " .. data.value) end
--- Creates an instance of sensor for HC-SR04 component
s = sensor.new("Range#1","HC-SR04",18,24)
--- Set the call_back of the senor to the range_call_back function
s:setCallBack("range_call_back1")
--- Creates second instance of sensor for HC-SR04 component
ss = sensor.new("Range#2","HC-SR04",18,24)
--- Set the call_back of the senor to the range_call_back function
ss:setCallBack("range_call_back2")
--- Convert sensor object to string to display the details
print(s)
print(ss)
--- sleep(5)
--- sensor.kill()
--- sleep(1)
