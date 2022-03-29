#!/usr/bin/env python

import rtmidi
import time

midiout = rtmidi.RtMidiOut()
pc = midiout.getPortCount()
if pc == 0:
    exit(1)

msgs = []

for i in range(1,17):
    msgs.append(rtmidi.MidiMessage.pitchWheel(i, 8192))
    msgs.append(rtmidi.MidiMessage.controllerEvent(i, 1, 0))
    msgs.append(rtmidi.MidiMessage.channelPressureChange(i, 0))
    msgs.append(rtmidi.MidiMessage.controllerEvent(i, 0x40, 0))
    msgs.append(rtmidi.MidiMessage.allNotesOff(i))
    msgs.append(rtmidi.MidiMessage.allControllersOff(i))

ports = []

for i in range(pc):
    print("OUT %d: %s" % (i, midiout.getPortName(i)))

    port = rtmidi.RtMidiOut()
    port.openPort(i)
    ports.append(port)

for n in range(100):
    for m in msgs:
        #ports[0].sendMessage(m)
        #ports[1].sendMessage(m)
        #ports[2].sendMessage(m)
        #ports[3].sendMessage(m)
        #ports[4].sendMessage(m)
        #ports[5].sendMessage(m)
        #ports[6].sendMessage(m)
        pass

