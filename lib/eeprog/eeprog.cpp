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

#define PIN_SHIFT_ADDRESS 2
#define PIN_CLOCK_ADDRESS 3
#define PIN_LATCH_ADDRESS 4
#define PIN_WRITE_ENABLE  13

#define PIN_DATA_0 5
#define PIN_DATA_7 12

#define MODE_DELAY_MS 3

#define DATA_OUTPUT_DELAY_MS    3
#define ADDRESS_OUTPUT_DELAY_MS 3
#define DATA_LATCH_DELAY_US     1

void eeprog::reset()
{
    pinMode(PIN_SHIFT_ADDRESS, OUTPUT);
    pinMode(PIN_CLOCK_ADDRESS, OUTPUT);
    pinMode(PIN_LATCH_ADDRESS, OUTPUT);
    pinMode(PIN_WRITE_ENABLE, OUTPUT);

    digitalWrite(PIN_WRITE_ENABLE, HIGH);

    this->_mode_read();
}

void eeprog::write(uint16_t base_address, uint8_t *data, size_t size)
{
    this->_mode_write();

    for(uint16_t i = 0; i < size; ++i)
    {
        _set_address(base_address + i, false);
        delay(ADDRESS_OUTPUT_DELAY_MS);

        _set_data(data[i]);
        delay(DATA_OUTPUT_DELAY_MS);

        digitalWrite(PIN_WRITE_ENABLE, LOW);
        delayMicroseconds(DATA_LATCH_DELAY_US);
        digitalWrite(PIN_WRITE_ENABLE, HIGH);
    }
}

void eeprog::read(uint16_t base_address, uint8_t *buffer, size_t size)
{
    this->_mode_read();

    for(uint16_t i = 0; i < size; ++i)
    {
        _set_address(base_address + i, true);
        delay(ADDRESS_OUTPUT_DELAY_MS);

        uint8_t data = _read_data();
        buffer[i] = data;
    }
}

inline void eeprog::_set_address(uint16_t address, bool read)
{
    // Shift out the address to the address registers
    shiftOut(PIN_SHIFT_ADDRESS, PIN_CLOCK_ADDRESS, MSBFIRST, (address >> 8) | (read ? 0x00 : 0x80));
    shiftOut(PIN_SHIFT_ADDRESS, PIN_CLOCK_ADDRESS, MSBFIRST, address);

    // Latch the address
    digitalWrite(PIN_LATCH_ADDRESS, LOW);
    digitalWrite(PIN_LATCH_ADDRESS, HIGH);
    digitalWrite(PIN_LATCH_ADDRESS, LOW);
}

inline void eeprog::_set_data(uint8_t data)
{
    for (uint8_t dataPin = PIN_DATA_0; dataPin <= PIN_DATA_7; dataPin += 1) {
        digitalWrite(dataPin, data & 1);
        data = data >> 1;
    }
}

inline uint8_t eeprog::_read_data()
{
    uint8_t data = 0;
    for (int pin = PIN_DATA_7; pin >= PIN_DATA_0; pin -= 1) {
        data = (data << 1) + digitalRead(pin);
    }

    return data;
}

inline void eeprog::_mode_read()
{
    for(uint8_t dataPin = PIN_DATA_0; dataPin <= PIN_DATA_7; dataPin += 1)
    {
        pinMode(dataPin, INPUT);
    }

    digitalWrite(PIN_WRITE_ENABLE, HIGH);
    delay(MODE_DELAY_MS);
}

inline void eeprog::_mode_write()
{
    for(uint8_t dataPin = PIN_DATA_0; dataPin <= PIN_DATA_7; dataPin += 1)
    {
        pinMode(dataPin, OUTPUT);
    }

    digitalWrite(PIN_WRITE_ENABLE, HIGH);
    delay(MODE_DELAY_MS);
}