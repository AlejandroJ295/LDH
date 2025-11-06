#define MY_RADIO_RF24
#define MY_RF24_CHANNEL 49
#define MY_RF24_CE_PIN 9
#define MY_RF24_CS_PIN 10
#define MY_NODE_ID 91
 
#include <SPI.h>
#include <MySensors.h>  
#include <DHT.h>
#include <Vcc.h>

#define DHT_DATA_PIN 3
#define SENSOR_TEMP_OFFSET 0

static const uint64_t UPDATE_INTERVAL = 3600000;
static const uint8_t FORCE_UPDATE_N_READS = 10;

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define CHILD_ID_BATTERY 4
#define VCC_ID 3

float lastTemp;
float lastHum;
uint8_t nNoUpdatesTemp;
uint8_t nNoUpdatesHum;
bool metric = true;

const float VccMin = 3.0;
const float VccMax = 4.2;
int oldBatteryPcnt = 0;
Vcc vcc;
DHT dht;

MyMessage ch_comm_vcc(VCC_ID, V_VOLTAGE);
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

void presentation()  
{ 
  sendSketchInfo("TemperatureAndHumidity", "1.1");
  sendSketchInfo("Battery Metter", "1.1");

  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
  present(VCC_ID, S_MULTIMETER);
}


void setup()
{
  dht.setup(DHT_DATA_PIN);
  if (UPDATE_INTERVAL <= dht.getMinimumSamplingPeriod()) {
    Serial.println("Warning: UPDATE_INTERVAL is smaller than supported by the sensor!");
  }
  sleep(dht.getMinimumSamplingPeriod());
}


void loop()      
{  
    int voltage = Vcc::measure(100, 1100);
    int batteryPercent = static_cast<int>(100.0 * ((voltage/ 1000.0 - 3) / 1.2));
    send(ch_comm_vcc.set(voltage / 1000.0, 2)); 

    if(batteryPercent > 100) batteryPercent = 100;
    if(batteryPercent <0) batteryPercent = 0;

    sendBatteryLevel(batteryPercent);
    
  dht.readSensor(true);
  float temperature = dht.getTemperature();
  
  if (isnan(temperature)) {
    Serial.println("Failed reading temperature from DHT!");
  } else if (temperature != lastTemp || nNoUpdatesTemp == FORCE_UPDATE_N_READS) {
    lastTemp = temperature;
    temperature += SENSOR_TEMP_OFFSET;
    if (!metric) {
      temperature = dht.toFahrenheit(temperature);
    }
    nNoUpdatesTemp = 0;
    send(msgTemp.set(temperature, 1));
  } else {
    nNoUpdatesTemp++;
  }
  
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
    Serial.println("Failed reading humidity from DHT");
  } else if (humidity != lastHum || nNoUpdatesHum == FORCE_UPDATE_N_READS) {
    lastHum = humidity;
    nNoUpdatesHum = 0;
    send(msgHum.set(humidity, 1));
  } else {
    nNoUpdatesHum++;
  }
  sleep(UPDATE_INTERVAL); 
}
