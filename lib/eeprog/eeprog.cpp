// Copyright 2019 Tobias Eriksson

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "eeprog.h"

#include <Wire.h>

#define DEVICE_I2C_ADDRESS 0x20
#define SLAVE_I2C_ADDRESS  0x04

#define REGISTER_IODIR     0x00
#define REGISTER_GPIO      0x09

#define IO_ALL_PINS_OUTPUT 0x00
#define IO_ALL_PINS_INPUT  0xFF

#define PIN_RESET 9

void eeprog::reset()
{
    // Set the digital pin modes
    pinMode(PIN_RESET, OUTPUT);

    // All pins are active-low, so set to high to begin with
    digitalWrite(PIN_RESET, HIGH);

    // Perform a RESET
    digitalWrite(PIN_RESET, LOW);
    delay(1);
    digitalWrite(PIN_RESET, HIGH);

    // Enable i2c interface
    Wire.begin();
}

void eeprog::write(uint16_t base_address, uint8_t *data, uint8_t size)
{
    this->_write_enable();

    for(uint8_t i = 0; i < size; ++i)
    {
        this->_write(data[i]);
    }
}

inline void eeprog::_write_enable()
{
    Wire.beginTransmission(DEVICE_I2C_ADDRESS | SLAVE_I2C_ADDRESS);
    {
        Wire.write(REGISTER_IODIR);     // Set I/O direction
        Wire.write(IO_ALL_PINS_OUTPUT); // Set all pins as output
    }
    Wire.endTransmission();
}

inline void eeprog::_write(uint8_t data)
{
    Wire.beginTransmission(DEVICE_I2C_ADDRESS | SLAVE_I2C_ADDRESS);
    {
        Wire.write(REGISTER_GPIO); // Select the GPIO register
        Wire.write(data);          // Write the data to GPIO
    }
    Wire.endTransmission();
}