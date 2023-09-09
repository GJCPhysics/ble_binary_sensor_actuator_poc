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
 * @file    door_sensor.cpp
 * @author  Surendra Nadkarni
 * @date    09/10/2023
 * @brief   The door sensor logic to check the state of the door using reed switch
 *          based digital sensor
 ********************************************************************************
 */
#include <door_sensor.h>
#include <Arduino.h>
#include <board.h>

namespace DoorSensor
{
    static uint8_t  f_last_pin_state = LOW;
    static enum State f_state = DoorSensor::DOOR_CLOSED;
    static cb_notify f_cb = NULL;
    static void *f_context = NULL;

    void setup(cb_notify i_cb, void *i_context)
    {
        pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // set arduino pin to input pull-up mode
        f_cb = i_cb;
        f_context = i_context;
    }

    void loop(void)
    {
        static uint8_t current_pin_state;
        current_pin_state  = digitalRead(DOOR_SENSOR_PIN); // read new state

        if ((f_last_pin_state == LOW) && (current_pin_state == HIGH))  // state change: LOW -> HIGH
        {
            f_state = DoorSensor::DOOR_CLOSED;
            if(f_cb) f_cb(f_context, f_state);
            Serial.println("Door closed");
        }
        else if ((f_last_pin_state == HIGH) && (current_pin_state == LOW))  // state change: HIGH -> LOW
        {
            f_state = DoorSensor::DOOR_OPENED;
            if(f_cb) f_cb(f_context, f_state);
            Serial.println("Door Open");
        }
        else
        {
        }
        f_last_pin_state = current_pin_state;              // save the last state
    }

    DoorSensor::State getStatus(void)
    {
        return f_state;
    }
}