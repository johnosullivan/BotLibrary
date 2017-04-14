#!/usr/bin/lua

function sleep(n)
  os.execute("sleep " .. tonumber(n))
end

lib = require("rolibsensor")

sensor = lib.new("Range#1","HC-SR04",18,24)

while (true) do
  sleep(0.05)
  print(sensor:read())
end
