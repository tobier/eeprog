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

#define MCP23008_DEVICE_I2C_ADDRESS 0x20
#define MCP23008_SLAVE_I2C_ADDRESS  0x04

#define MCP23008_REGISTER_IODIR     0x00
#define MCP23008_REGISTER_GPIO      0x09

#define MCP23017_DEVICE_I2C_ADDRESS 0x20
#define MCP23017_SLAVE_I2C_ADDRESS  0x00

#define MCP23017_REGISTER_IODIRA    0x00
#define MCP23017_REGISTER_IODIRB    0x01

#define MCP23017_REGISTER_GPIOA     0x12
#define MCP23017_REGISTER_GPIOB     0x13

#define IO_ALL_PINS_OUTPUT          0x00
#define IO_ALL_PINS_INPUT           0xFF

#define EEPROG_PIN_RESET 9
#define EEPROG_PIN_CE   10
#define EEPROG_PIN_OE   11
#define EEPROG_PIN_WE   12

#define ADDRESS_OUTPUT_DELAY_US 1

#define DELAY_I2C_US   10

void eeprog::reset()
{
    // Set the digital pin modes
    pinMode(EEPROG_PIN_RESET, OUTPUT);
    pinMode(EEPROG_PIN_CE, OUTPUT);
    pinMode(EEPROG_PIN_OE, OUTPUT);
    pinMode(EEPROG_PIN_WE, OUTPUT);

    // All pins are active-low, so set to high to begin with
    digitalWrite(EEPROG_PIN_RESET, HIGH);
    digitalWrite(EEPROG_PIN_CE, HIGH);
    digitalWrite(EEPROG_PIN_OE, HIGH);
    digitalWrite(EEPROG_PIN_WE, HIGH);

    // Perform a RESET
    digitalWrite(EEPROG_PIN_RESET, LOW);
    delay(1);
    digitalWrite(EEPROG_PIN_RESET, HIGH);

    // Enable i2c interface
    Wire.begin();

    // MCP23017 will only be used as output so set it like so
    Wire.beginTransmission(MCP23017_DEVICE_I2C_ADDRESS | MCP23017_SLAVE_I2C_ADDRESS);
    {
        Wire.write(MCP23017_REGISTER_IODIRA);
        Wire.write(IO_ALL_PINS_OUTPUT);
        Wire.write(MCP23017_REGISTER_IODIRB);
        Wire.write(IO_ALL_PINS_OUTPUT);
    }
    Wire.endTransmission();
}

void eeprog::write(uint16_t base_address, uint8_t *data, uint8_t size)
{
    this->_write_enable();

    for(uint8_t i = 0; i < size; ++i)
    {
        this->_set_address(base_address + i);
        this->_write(data[i]);
    }
}

void eeprog::read(uint16_t start_address, uint8_t *buffer, uint8_t size)
{
    this->_read_enable();

    digitalWrite(EEPROG_PIN_WE, HIGH);

    for(uint8_t i = 0; i < size; ++i)
    {
        this->_set_address(start_address + i);

        digitalWrite(EEPROG_PIN_CE, LOW);
        digitalWrite(EEPROG_PIN_OE, LOW);
        delayMicroseconds(ADDRESS_OUTPUT_DELAY_US);

        uint8_t data = this->_read();

        digitalWrite(EEPROG_PIN_CE, HIGH);
        digitalWrite(EEPROG_PIN_OE, HIGH);
        buffer[i] = data;
    }

}

inline void eeprog::_set_address(uint16_t address)
{
    uint8_t lo_byte = address & 0xFF;
    uint8_t hi_byte = (address >> 8) & 0xFF;

    Wire.beginTransmission(MCP23017_DEVICE_I2C_ADDRESS | MCP23017_SLAVE_I2C_ADDRESS);
    {
        Wire.write(MCP23017_REGISTER_GPIOA);
        Wire.write(lo_byte);
        Wire.write(MCP23017_REGISTER_GPIOB);
        Wire.write(hi_byte);
    }
    Wire.endTransmission();

    delayMicroseconds(DELAY_I2C_US);
}

inline void eeprog::_write_enable()
{
    pinMode(EEPROG_PIN_CE, OUTPUT);
    pinMode(EEPROG_PIN_OE, OUTPUT);
    pinMode(EEPROG_PIN_WE, OUTPUT);

    digitalWrite(EEPROG_PIN_OE, HIGH);

    Wire.beginTransmission(MCP23008_DEVICE_I2C_ADDRESS | MCP23008_SLAVE_I2C_ADDRESS);
    {
        Wire.write(MCP23008_REGISTER_IODIR); // Set I/O direction
        Wire.write(IO_ALL_PINS_OUTPUT);      // Set all pins as output
    }
    Wire.endTransmission();

    delayMicroseconds(DELAY_I2C_US);
}

inline void eeprog::_write(uint8_t data)
{
    Wire.beginTransmission(MCP23008_DEVICE_I2C_ADDRESS | MCP23008_SLAVE_I2C_ADDRESS);
    {
        Wire.write(MCP23008_REGISTER_GPIO); // Select the GPIO register
        Wire.write(data);                   // Write the data to GPIO
    }
    Wire.endTransmission();

    delayMicroseconds(DELAY_I2C_US);
}

inline void eeprog::_read_enable()
{
    pinMode(EEPROG_PIN_CE, OUTPUT);
    pinMode(EEPROG_PIN_OE, OUTPUT);
    pinMode(EEPROG_PIN_WE, OUTPUT);

    Wire.beginTransmission(MCP23008_DEVICE_I2C_ADDRESS | MCP23008_SLAVE_I2C_ADDRESS);
    {
        Wire.write(MCP23008_REGISTER_IODIR); // Set I/O direction
        Wire.write(IO_ALL_PINS_INPUT);       // Set all pins as input
    }
    Wire.endTransmission();

    delayMicroseconds(DELAY_I2C_US);
}

inline uint8_t eeprog::_read()
{
    Wire.beginTransmission(MCP23008_DEVICE_I2C_ADDRESS | MCP23008_SLAVE_I2C_ADDRESS);
    {
        Wire.write(MCP23008_REGISTER_GPIO); // Select the GPIO register
    }
    Wire.endTransmission();

    delayMicroseconds(DELAY_I2C_US);

    Wire.requestFrom(MCP23008_DEVICE_I2C_ADDRESS | MCP23008_SLAVE_I2C_ADDRESS, (uint8_t)1);

    return Wire.read();
}