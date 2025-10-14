/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2018 Sensnology AB
 * Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Example sketch showing how to control physical relays.
 * This example will remember relay state after power failure.
 * http://www.mysensors.org/build/relay
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95
#define MY_RF24_CHANNEL 49
#define MY_RF24_CE_PIN 10
#define MY_RF24_CS_PIN 9
#define MY_NODE_ID 91

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

#include <MySensors.h>

#define SWITCH_PIN_1 2
#define SWITCH_PIN_2 3
#define RELAY_PIN 4  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define CHILD_ID_RELAY 4
#define CHILD_ID_SWITCH_1 5
#define CHILD_ID_SWITCH_2 6

#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay

MyMessage msgRelay(CHILD_ID_RELAY, V_STATUS);
MyMessage msgSwitch1(CHILD_ID_SWITCH_1, V_STATUS);
MyMessage msgSwitch2(CHILD_ID_SWITCH_2, V_STATUS);

int estadoAnterior = 0;
bool estado = false;
bool estadoAnteriorRelay;

void before()
{
  Serial.begin(115200);
	  pinMode(SWITCH_PIN_1, INPUT_PULLUP);
    pinMode(SWITCH_PIN_2, OUTPUT);
    digitalWrite (SWITCH_PIN_2, LOW);
    pinMode(RELAY_PIN, OUTPUT);
		digitalWrite(RELAY_PIN, loadState(CHILD_ID_RELAY)?RELAY_ON:RELAY_OFF);
}

void setup()
{

}

void presentation()
{
sendSketchInfo("Double Switch Sensor with Relay", "1.1");
present(CHILD_ID_RELAY, S_BINARY, "Relay");
present(CHILD_ID_SWITCH_1, S_BINARY);
present(CHILD_ID_SWITCH_2, S_BINARY);
	
}


void loop()
{

  int  switch1 = digitalRead(SWITCH_PIN_1);
  int relay = digitalRead(RELAY_PIN);

  if(switch1 != estadoAnterior){
    Serial.println("Estamos en el aqui");
    send(msgRelay.set(!relay));
    Serial.println("Estamos en el if");
    digitalWrite(RELAY_PIN, !relay);
    estado = !estado;
    estadoAnterior = estado;
  }
/*
 * 
 *   int  switch1 = digitalRead(SWITCH_PIN_1);
if(switch1 != estadoAnteriorSwitch){
    Serial.println("Estamos en el aqui");
    send(msgRelay.set(!digitalRead(RELAY_PIN)));
        if(digitalRead(SWITCH_PIN_1) == LOW){
          Serial.println("Estamos en el if");
          digitalWrite(RELAY_PIN, LOW);
          estado = false;
          estadoAnteriorSwitch = estado;
      }else{
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("Estamos en el else");
        estado = true;
        estadoAnteriorSwitch = estado;
        }

    }
  int  switch1 = digitalRead(SWITCH_PIN_1);
  bool estadoRelay = digitalRead(RELAY_PIN);
  if(switch1 != estadoAnteriorSwitch){
    estadoRelay = !estadoAnteriorRelay;
    send(msgRelay.set(estadoRelay));
    digitalWrite(RELAY_PIN, estadoRelay);
    estadoAnteriorSwitch = switch1;
    }
    */
    /*
     *   
  if (digitalRead(SWITCH_PIN_1) == LOW){
    digitalWrite(RELAY_PIN, LOW);
    send(msgRelay.set(LOW));
    }else{
    digitalWrite(RELAY_PIN, HIGH);
    send(msgRelay.set(HIGH));
      }
      */
}

void receive(const MyMessage &message)
{
	// We only expect one type of message from controller. But we better check anyway.
	if (message.type==V_STATUS){
		digitalWrite(RELAY_PIN, message.getBool()?RELAY_ON:RELAY_OFF);
		// Store state in eeprom
		saveState(message.sensor, message.getBool());
		// Write some debug info
		Serial.print("Incoming change for sensor:");
		Serial.print(message.sensor);
		Serial.print(", New status: ");
		Serial.println(message.getBool());
	}
}
