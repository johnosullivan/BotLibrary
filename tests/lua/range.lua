#!/usr/bin/lua

GPIO = require("rolib").GPIO
core = require("rolibcore")

function sleep(n)
  os.execute("sleep " .. tonumber(n))
end

gpio_trigger = GPIO(18,"out")
gpio_echo = GPIO(24,"in")

gpio_trigger:write(1)
sleep(0.00001)
gpio_trigger:write(0)


start = core.gettime()

stop = core.gettime()

while gpio_echo:read() == false do
  start = core.gettime()
end

while gpio_echo:read() == true do
  stop = core.gettime()
end

timeelapsed = stop - start

distance = (timeelapsed * 34300) / 2

print("Distance: " .. distance .. "cm")

gpio_trigger:close()
gpio_echo:close()
