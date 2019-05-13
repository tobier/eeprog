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

BLOCK_SIZE = 64
NUMBER_OF_BLOCKS = 128

COMMAND_ACK   = chr(0x06)
COMMAND_READ  = b'r'
COMMAND_WRITE = b'w'

class RemoteCommandFailed(Exception):
    pass

class Programmer:
    def __init__(self, port):
        self.serial = connect_to(port)
    
    def read(self, outfile):
        self.serial.write(COMMAND_READ)
        ack = self.serial.read()
        if ack.decode('ascii') != COMMAND_ACK:
            raise RemoteCommandFailed("No ACK for read command")
        with open(outfile, "wb") as to_file:
            for i in tqdm(iterable = range(NUMBER_OF_BLOCKS), desc='Reading from EEPROM', unit='blocks'):
                block_buffer = bytearray(self.serial.read(BLOCK_SIZE))
                to_file.write(block_buffer)

                ack = self.serial.read()
                if ack.decode('ascii') != COMMAND_ACK:
                    raise RemoteCommandFailed("No ACK after block read")

    def write(self, infile):
        self.serial.write(COMMAND_WRITE)
        ack = self.serial.read()
        if ack.decode('ascii') != COMMAND_ACK:
            raise RemoteCommandFailed("No ACK for write command")
        with open(infile, "rb") as from_file:
            for i in tqdm(iterable = range(NUMBER_OF_BLOCKS), desc='Writing to EEPROM', unit='blocks'):
                block_buffer = bytearray(from_file.read(BLOCK_SIZE))
                self.serial.write(block_buffer)

                ack = self.serial.read()
                if ack.decode('ascii') != COMMAND_ACK:
                    raise RemoteCommandFailed("No ACK after block write")
