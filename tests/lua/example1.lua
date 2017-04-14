#!/usr/bin/lua

r = require("rolibservo")

port = "/dev/cu.usbmodem00097421"

function sleep(n)
  os.execute("sleep " .. tonumber(n))
end

-- Connects to the servo broad
sbc = r.sbc(port);

-- Create a new servo object
servo = r.new(0,"RL#1","maestro",2000,sbc)

-- Pause for 2 seconds
sleep(2)

-- Sets servo target
servo:settarget(3000)

-- Pause for 2 seconds
sleep(2)

-- Sets servo target
servo:settarget(7000)

-- Exits program
os.exit()
