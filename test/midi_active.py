#!/usr/bin/env python

import rtmidi
import time
import sys

midiout = rtmidi.RtMidiOut()
c = midiout.getPortCount()
if c == 0:
    exit(1)

for i in range(c):
    print("OUT %d: %s" % (i, midiout.getPortName(i)))

a = rtmidi.MidiMessage(b'\xfe')

if len(sys.argv) > 1:
    i = int(sys.argv[1])
else:
    i = 0

print("OUT %d: %s" % (i, midiout.getPortName(i)))

midiout.openPort(i)

while True:
    midiout.sendMessage(a)
    time.sleep(0.3)

midiout.closePort()
