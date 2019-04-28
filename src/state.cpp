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

#include "state.h"

#include <Arduino.h>

#include "eeprog.h"

#define ASCII_ACK  0x06
#define ASCII_NACK 0x15
#define ASCII_READ 0x72

eeprog programmer;

#define BLOCK_SIZE 64
#define NUMBER_OF_BLOCKS 128

state_t state_reset(void)
{
    Serial.begin(9600);
    programmer.reset();
    return state_t::connect;
}

state_t state_connect(void)
{
    Serial.write(ASCII_ACK);
    return state_t::await_command;
}

state_t state_await_command(void)
{
    if(Serial.available() > 0)
    {
        char command = Serial.read();
        switch(command)
        {
            case ASCII_READ:
                Serial.write(ASCII_ACK);
                return state_t::read;
            default:
                Serial.write(ASCII_NACK);
                break;
        }
    }
    return state_t::await_command;
}

state_t state_read(void)
{
    uint8_t byte_buffer[BLOCK_SIZE];

    for(uint8_t block = 0; block < NUMBER_OF_BLOCKS; ++block)
    {
        uint16_t base_address = block * BLOCK_SIZE;
        programmer.read(base_address, byte_buffer, BLOCK_SIZE);
        for(uint8_t data = 0; data < BLOCK_SIZE; ++data)
        {
            Serial.write(byte_buffer[data]);
        }
    }

    return state_t::await_command;
}