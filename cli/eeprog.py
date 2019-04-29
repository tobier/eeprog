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
import argparse
import serial
import time

from tqdm import tqdm

ASCII_ACK  = chr(0x06)
ASCII_READ = b'r'

class ConnectionFailedException(Exception):
    pass

class RemoteCommandFailed(Exception):
    pass

def read_rom(ser, file):
  ser.write(ASCII_READ)
  buffer = ser.read(1)
  if buffer.decode('ascii') != ASCII_ACK:
    raise RemoteCommandFailed()

  with open(file, "wb") as outfile:
    for i in tqdm(iterable = range(128), desc='Reading from EEPROM', unit='block'):
      block_buffer = bytearray(ser.read(64))
      outfile.write(block_buffer)

def connect(serial_port):
  ser = serial.Serial(port=serial_port)
  buffer = ser.read(1)
  if buffer.decode('ascii') == ASCII_ACK:
    print("Connected to device...")
  else:
    raise ConnectionFailedException()

  return ser

def main():
    root_parser = argparse.ArgumentParser(description="eeprog command-line interface")

    command_group = root_parser.add_mutually_exclusive_group()
    command_group.add_argument("--read", "-r", type=str)

    root_parser.add_argument("SERIAL", type=str, help="the serial port to use")
    args = root_parser.parse_args()

    ser = connect(args.SERIAL)

    if args.read:
      outfile = args.read
      read_rom(ser, outfile)


if __name__== "__main__":
  try:
    main()
  except ConnectionFailedException:
    print("Failed to connect, have you tried resetting the device?")
  except RemoteCommandFailed:
    print("Device refused command, giving up...")