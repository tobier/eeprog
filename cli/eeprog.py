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
import os
import sys

from device import find_device
from programmer import Programmer

def main():
    parser = argparse.ArgumentParser(description='eeprog is a 28c64 programmer')
    parser.add_argument('-p', '--port', type=str, help='the serial port to use')
    subparsers = parser.add_subparsers(help='sub-command help')

    parser_read = subparsers.add_parser('read', help='read command help')
    parser_read.add_argument('outfile', help='the file to read the data to')

    parser_write = subparsers.add_parser('write', help='write command help')
    parser_write.add_argument('infile', help='the file to write the data from')

    args = parser.parse_args()
    
    serial_port = args.port if args.port else find_device()
    print("Using device: " + serial_port)
    programmer = Programmer(serial_port)

    if hasattr(args, 'outfile'):
        programmer.read(args.outfile)
    elif hasattr(args, 'infile'):
        programmer.write(args.infile)

if __name__== "__main__":
    main()