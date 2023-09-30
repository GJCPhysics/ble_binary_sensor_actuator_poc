/**
 * Copyright © 2023 Samarth Holdings AS
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the “Software”), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 ********************************************************************************
 * @file    pir_sensor.h
 * @author  Shubham Sawant
 * @date    09/10/2023
 * @brief   The pir sensor C++ api interface
 ********************************************************************************
 */
#pragma once
#include <cstdint>

namespace PIRSensor
{
    enum State : uint8_t
    {
        PRESENCE_NOT_DETECTED,
        PRESENCE_DETECTED,
    };
    typedef void (*cb_notify) (void*, State);
    void setup(cb_notify, void*);
    void loop(void);
    PIRSensor::State getStatus(void);
}
