/*
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
 * @file    main.cpp
 * @author  Surendra Nadkarni, Mayuri Kinare, Shubham Sawant
 * @date    09/10/2023
 * @brief   The Arduino application to handle request for Binary switch
 *          and notify if changed on binary sensor.
 ********************************************************************************
 */
#include <Arduino.h>
#include <ArduinoBLE.h>
#include <pir_sensor.h>
#include <door_sensor.h>
#include <smoke_sensor.h>
#include <app_version.h>

#define RESET_DOOR_SENSOR(val) (val & (~BIT_0))
#define SET_DOOR_SENSOR(val) (val | BIT_0)

#define RESET_SMOKE_SENSOR(val) (val & (~BIT_1))
#define SET_SMOKE_SENSOR(val) (val | BIT_1)

#define RESET_PIR_SENSOR(val) (val & (~BIT_2))
#define SET_PIR_SENSOR(val) (val | BIT_2)

static BLEService f_device_service("180A");

// BLE LED Switch Characteristic
static BLEByteCharacteristic f_digital_output_service("2A57", BLERead | BLEWrite);
static BLEByteCharacteristic f_sensor_characteristics("2A56", BLERead | BLENotify);
static uint8_t sensor_value = 0;

void pir_sensor_callback(void *i_context, PIRSensor::State i_state)
{
  if(PIRSensor::State::PRESENCE_NOT_DETECTED == i_state)
  {
    sensor_value = RESET_PIR_SENSOR(sensor_value);
  }
  else
  {
    sensor_value = SET_PIR_SENSOR(sensor_value);
  }
  f_sensor_characteristics.setValue(sensor_value);
}

void door_sensor_callback(void *i_context, DoorSensor::State i_state)
{
  if(DoorSensor::State::DOOR_CLOSED == i_state)
  {
    sensor_value = RESET_DOOR_SENSOR(sensor_value);
  }
  else
  {
    sensor_value = SET_DOOR_SENSOR(sensor_value);
  }
  f_sensor_characteristics.setValue(sensor_value);
}

void smoke_sensor_callback(void *i_context, SmokeSensor::State i_state)
{
  if(SmokeSensor::State::SMOKE_NOT_DETECTED == i_state)
  {
    sensor_value = RESET_SMOKE_SENSOR(sensor_value);
  }
  else
  {
    sensor_value = SET_SMOKE_SENSOR(sensor_value);
  }
  f_sensor_characteristics.setValue(sensor_value);
}



void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED's pin to output mode
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);         // when the central disconnects, turn off the LED
  digitalWrite(LEDR, HIGH);               // will turn the LED off
  digitalWrite(LEDG, HIGH);               // will turn the LED off
  digitalWrite(LEDB, HIGH);               // will turn the LED off

  //register the callback to recieve pir sensor state notification
  PIRSensor::setup(pir_sensor_callback, NULL);

 //register the callback to recieve door state notification
  DoorSensor::setup(door_sensor_callback, NULL);

 //register the callback to recieve smoke sensor state notification
  SmokeSensor::setup(smoke_sensor_callback, NULL);

  

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Physics BLE");
  BLE.setAdvertisedService(f_device_service);

  // add the characteristic to the service
  f_device_service.addCharacteristic(f_digital_output_service);
  f_device_service.addCharacteristic(f_sensor_characteristics);
  Serial.print("BLE Interfacing of Digital Signals. App Version ");
  Serial.println(APP_VERSION);
  // add service
  BLE.addService(f_device_service);

  // set the initial value for the characteristic:
  f_digital_output_service.writeValue(0);
  f_sensor_characteristics.setValue(sensor_value);

  // start advertising
  BLE.advertise();

}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection

    // while the central is still connected to peripheral:
    while (central.connected()) {
      PIRSensor::loop(); // Housekeeping loop for pir sensor
      DoorSensor::loop(); // Housekeeping loop for door sensor
      SmokeSensor::loop(); // Housekeeping loop for smoke sensor
     
      if (f_digital_output_service.written()) {
        // if the remote device wrote to the characteristic,
        // use the value to control the LED:
        switch (f_digital_output_service.value()) {   // any value other than 0
          case 01:
            Serial.println("Red LED on");
            digitalWrite(LEDR, LOW);            // will turn the LED on
            digitalWrite(LEDG, HIGH);         // will turn the LED off
            digitalWrite(LEDB, HIGH);         // will turn the LED off
            break;
          case 02:
            Serial.println("Green LED on");
            digitalWrite(LEDR, HIGH);         // will turn the LED off
            digitalWrite(LEDG, LOW);        // will turn the LED on
            digitalWrite(LEDB, HIGH);        // will turn the LED off
            f_sensor_characteristics.setValue(20);
            break;
          case 03:
            Serial.println("Blue LED on");
            digitalWrite(LEDR, HIGH);         // will turn the LED off
            digitalWrite(LEDG, HIGH);       // will turn the LED off
            digitalWrite(LEDB, LOW);         // will turn the LED on
            break;
          default:
            Serial.println(F("LEDs off"));
            digitalWrite(LEDR, HIGH);          // will turn the LED off
            digitalWrite(LEDG, HIGH);        // will turn the LED off
            digitalWrite(LEDB, HIGH);         // will turn the LED off
            break;
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);         // when the central disconnects, turn off the LED
    digitalWrite(LEDR, HIGH);          // will turn the LED off
    digitalWrite(LEDG, HIGH);        // will turn the LED off
    digitalWrite(LEDB, HIGH);         // will turn the LED off
  }
}

