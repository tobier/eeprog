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

#define CONNECT_IDENTIFIER "eeprog"

#define COMMAND_ACK   0x06
#define COMMAND_NACK  0x15
#define COMMAND_READ  0x72
#define COMMAND_WRITE 0x77

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
    Serial.println(CONNECT_IDENTIFIER);
    return state_t::await_command;
}

state_t state_await_command(void)
{
    if(Serial.available() > 0)
    {
        char command = Serial.read();
        switch(command)
        {
            case COMMAND_READ:
                Serial.write(COMMAND_ACK);
                return state_t::read;
            case COMMAND_WRITE:
                Serial.write(COMMAND_ACK);
                return state_t::write;
            default:
                Serial.write(COMMAND_NACK);
                break;
        }
    }
    return state_t::await_command;
}

state_t state_read(void)
{
    uint8_t buffer[BLOCK_SIZE];
    uint16_t address = 0;

    for(size_t i = 0; i < NUMBER_OF_BLOCKS; ++i)
    {
        programmer.read(address, buffer, BLOCK_SIZE);

        Serial.write(buffer, BLOCK_SIZE);
        address += BLOCK_SIZE;

        Serial.write(COMMAND_ACK);
    }

    return state_t::await_command;
}

state_t state_write(void)
{
    uint8_t write_buffer[BLOCK_SIZE];
    uint8_t read_buffer[BLOCK_SIZE];
    uint16_t address = 0;

    for(size_t i = 0; i < NUMBER_OF_BLOCKS; ++i)
    {
        Serial.readBytes(write_buffer, BLOCK_SIZE);
        programmer.write(address, write_buffer, BLOCK_SIZE);

        bool verified = true;
        programmer.read(address, read_buffer, BLOCK_SIZE);

        for(size_t j = 0; j < BLOCK_SIZE; ++j)
        {
            if(write_buffer[j] != read_buffer[j])
            {
                verified = false;
                break;
            }
        }

        address += BLOCK_SIZE;
        Serial.write(verified ? COMMAND_ACK : COMMAND_NACK);
    }

    return state_t::await_command;
}