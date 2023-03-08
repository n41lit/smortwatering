#include <WiFiNINA.h>
#include <ArduinoHA.h>

//define earth sensor pin to A1
#define ESENS_PIN       5
//define motor pin to A2
#define MOT_PIN         6
//define water sensor pin to A3
#define WSENS_PIN       7
//put in mqtt broker address (your home assistant ip)
#define BROKER_ADDR     IPAddress(192,168,0,17)
//change to your wifi network
#define WIFI_SSID       "MyNetwork"
#define WIFI_PASSWORD   "MyPassword"

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

// "myAnalogInput" is unique ID of the sensor. You should define your own ID.
HASensorNumber analogSensorWater("Water Bottle", HASensorNumber::PrecisionP1);
HASensorNumber analogSensorPlant("Plant Humidity", HASensorNumber::PrecisionP1);
HAButton buttonOne("Activate Pump");
HABinarySensor binarySensorPump("Pump active");


void activatePump(){
    digitalWrite(MOT_PIN, HIGH);
    binarySensorPump->setState(HIGH);
    delay(1000);
    digitalWrite(MOT_PIN, LOW);
    binarySensorPump->setState(LOW);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Booting Smort Watering System...");

    // Unique ID must be set!
    byte mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));

    pinMode(ESENS_PIN, INPUT);
    pinMode(WSENS_PIN, INPUT);
    pinMode(MOT_PIN, OUTPUT);
    digitalWrite(MOT_PIN, LOW);

    //callbacks for Buttons
    buttonOne.onCommand(activatePump);

    // connect to wifi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500); // waiting for the connection
    }
    Serial.println();
    Serial.println("Smort Watering system is now connected to the network...");

    // set device details
    device.setName("Smort Watering Solution");
    device.setSoftwareVersion("1.0.0");

    //set sensor and button details
    analogSensorWater.setIcon("mdi:water-alert");
    analogSensorWater.setName("Water Bottle Sensor");
    analogSensorWater.setUnitOfMeasurement("%");

    analogSensorEarth.setIcon("mdi:flower");
    analogSensorEarth.setName("Earth Sensor");
    analogSensorEarth.setUnitOfMeasurement("%");

    buttonOne.setIcon("mdi:watering-can");
    buttonOne.setName("Activate Pump");

    binarySensorPump.setIcon("mdi:pump");
    binarySensorPump.setName("Pump Active");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    mqtt.loop();

    if(WSENS_PIN > 0.5) {
        if(ESENS_PIN < 0.5) {
            activatePump();
        }
    }
    delay(5000);
}