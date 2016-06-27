#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <Wire.h>
//#include <Adafruit_INA219.h>

// Connect to the WiFi
const char* ssid = "WUN-TDI";
const char* password = "gtb2740!";
const char* mqtt_server = "erickersten.com";
const char* mqtt_topic = "ledStatus";
const byte ledPin = LED_BUILTIN; // Pin with LED on Adafruit Huzzah
const int switchPin = 5;
volatile int lastSwitchStatus;
volatile int ledStatus = HIGH;

WiFiClient espClient;
PubSubClient client(espClient);

void setLedStatus(int status) {
    ledStatus = status;
    client.publish("ledStatus", (char*)ledStatus);
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    int newStatus = 0;
    for (int i=0;i<length;i++) {
        char receivedChar = (char)payload[i];
        Serial.print(receivedChar);
        if (receivedChar == '0') {
            setLedStatus(HIGH);
        }
        if (receivedChar == '1') {
            setLedStatus(LOW);
        }
    }
    Serial.println();
    Serial.print("remote changed to ");
    Serial.println(ledStatus);
}


void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266 Client")) {
            Serial.println("connected");
            // ... and subscribe to topic
            client.subscribe(mqtt_topic);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    pinMode(ledPin, OUTPUT);

    pinMode(switchPin, INPUT);
    lastSwitchStatus = digitalRead(switchPin);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    digitalWrite(ledPin, ledStatus);

    if (digitalRead(switchPin) != lastSwitchStatus) {
        setLedStatus(!ledStatus);
        lastSwitchStatus = ledStatus;
    }
}
