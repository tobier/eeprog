# eeprog

[![build](https://travis-ci.org/tobier/eeprog.svg)](https://travis-ci.org/tobier/eeprog)

Firmware for eeprog - EEPROM programmer on the ATmega328 microcontroller

## Hardware

The hardware is implemented as a shield for the Arduino Uno R3, utilizing the digital I/O ports for shifting data and address.

![](docs/schematic-eeprog-arduino-uno.svg)

The shield only supports the 8K AT28C64 EEPROM at this moment, but could be modified to support larger EEPROMs.
