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
from tqdm import tqdm

from device import connect_to

COMMAND_ACK  = chr(0x06)
COMMAND_READ = b'r'

class RemoteCommandFailed(Exception):
    pass

class Programmer:
    def __init__(self, port):
        self.serial = connect_to(port)
    
    def read_to(self, outfile):
        self.serial.write(COMMAND_READ)
        ack = self.serial.read()
        if ack.decode('ascii') != COMMAND_ACK:
            raise RemoteCommandFailed("No ACK for read command")
        with open(outfile, "wb") as to_file:
            for i in tqdm(iterable = range(128), desc='Reading from EEPROM', unit='block'):
                block_buffer = bytearray(self.serial.read(64))
                to_file.write(block_buffer)