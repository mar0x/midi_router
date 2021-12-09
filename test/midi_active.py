#!/usr/bin/env python

import rtmidi
import time

midiout = rtmidi.RtMidiOut()
c = midiout.getPortCount()
if c == 0:
    exit(1)

a = rtmidi.MidiMessage(b'\xfe')
i = 0

print("OUT %d: %s" % (i, midiout.getPortName(i)))

midiout.openPort(i)

while True:
    midiout.sendMessage(a)
    time.sleep(0.3)

midiout.closePort()
