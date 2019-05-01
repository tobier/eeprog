#!/usr/bin/env python3
# Copyright 2019 Tobias Eriksson

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
import serial
import serial.tools.list_ports

from serial import SerialException

EEPROG_IDENTIFIER = 'eeprog'

class ConnectionFailed(Exception):
    pass

class DeviceNotFound(Exception):
    pass

def connect_to(device):
    try:
        ser = serial.Serial(port = device, baudrate=9600)
        identifier = ser.read_until().decode('ascii').strip()
        if identifier == EEPROG_IDENTIFIER:
            return ser
        raise ConnectionFailed("Failed to connect to device: " + device)
    except SerialException:
        raise ConnectionFailed("No such device: " + device)

def find_device():
    for comport in serial.tools.list_ports.comports():
        try:
            with serial.Serial(port = comport.device, baudrate=9600, timeout=2.0) as ser:
                identifier = ser.read_until().decode('ascii').strip()
                if identifier == EEPROG_IDENTIFIER:
                    return comport.device
        except:
                pass
    raise DeviceNotFound("Failed to find device")
