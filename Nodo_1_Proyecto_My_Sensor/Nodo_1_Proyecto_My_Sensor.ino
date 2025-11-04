//#define MY_DEBUG

#define MY_DEBUG_VERBOSE_SIGNING
#define MY_SIGNING_SOFT
#define MY_SIGNING_SOFT_RANDOMSEED_PIN 6
#define MY_SIGNING_REQUEST_SIGNATURES

#define MY_RADIO_RF24
#define MY_RF24_CHANNEL 49
#define MY_RF24_CE_PIN 10
#define MY_RF24_CS_PIN 9
#define MY_NODE_ID 91

#define MY_REPEATER_FEATURE

#include <MySensors.h>

#define SWITCH_PIN_1 2
#define SWITCH_PIN_2 3
#define RELAY_PIN 4
#define CHILD_ID_RELAY 4

#define RELAY_ON 1
#define RELAY_OFF 0

MyMessage msgRelay(CHILD_ID_RELAY, V_STATUS);

int estadoAnterior = 0;
bool estado = false;
bool estadoAnteriorRelay;

void before()
{
  Serial.begin(115200);
	  pinMode(SWITCH_PIN_1, INPUT_PULLUP);
    pinMode(SWITCH_PIN_2, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite (SWITCH_PIN_2, LOW);
		digitalWrite(RELAY_PIN, loadState(CHILD_ID_RELAY)?RELAY_ON:RELAY_OFF);
}

void setup()
{

}

void presentation()
{
sendSketchInfo("Double Switch Sensor with Relay", "1.1");
present(CHILD_ID_RELAY, S_BINARY, "Relay");
}


void loop()
{

  int  switch1 = digitalRead(SWITCH_PIN_1);
  int relay = digitalRead(RELAY_PIN);

  if(switch1 != estadoAnterior){
    send(msgRelay.set(!relay));
    digitalWrite(RELAY_PIN, !relay);
    estado = !estado;
    estadoAnterior = estado;
  }
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
