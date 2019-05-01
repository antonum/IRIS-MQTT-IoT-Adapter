/* The following code would connect to WiFi and MQTT brocker
 * then would send the message to the selected MQTT topic
 * <payload><sensorValue>1</sensorValue><sensorID>80:7D:3A:7F:3A:D5</sensorID></payload> 
 *
 * antonum 2019
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
WiFiClient espClient;
PubSubClient client(espClient);



// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

//void initWifi();
//void initMQTT();

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "MQTT_BROCKER_IP";
const char* mqtt_topic = "iris/in";
int ledPin = LED_BUILTIN; 

//readings should be within no more than 3 seconds from each other
#define REPORTING_PERIOD_MS     3000
//minimum consecutive readings required
#define MIN_READINGS     10
int numReadings=0;
uint32_t tsLastReport = 0;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected() {
    Serial.println("Beat!");
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      //Readings too far apart. Reset the counter, start over
      numReadings=0;
    } else {
      numReadings++;
      Serial.print(numReadings);
      Serial.print(" Heart rate:");
      Serial.print(pox.getHeartRate());
      Serial.print("bpm / SpO2:");
      Serial.println(pox.getSpO2()); 
      if (numReadings >= MIN_READINGS) {
          Serial.print("Sending message:");
          Serial.println(pox.getHeartRate());
          mqttSend(String(pox.getHeartRate()));
          numReadings=0;
      }
    }
    tsLastReport = millis();
}


void mqttSend(String reading) {
  String payload = "<payload>";
  payload += "<sensorValue>"; 
      payload += reading; 
  payload += "</sensorValue>";
  payload += "<sensorID>"; 
      payload += WiFi.macAddress();
  payload += "</sensorID>";
  payload += "</payload>"; // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  if (!client.connected()) {
    initMQTT();
  }
  client.publish(mqtt_topic, attributes);
  Serial.print(mqtt_topic);
  Serial.println(payload);


}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  //pinMode(sensorPin, INPUT);
  // We start by connecting to a WiFi network
  initWifi();
  initMQTT();
  initPox();
}

void loop() {
      pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    //if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        //Serial.print("Heart rate:");
        //Serial.print(pox.getHeartRate());
        //Serial.print("bpm / SpO2:");
        //Serial.print(pox.getSpO2());
        //Serial.println("%");

        
    //}

}
// This function initialize Pulse Oximeter
void initPox() {
      Serial.print("Initializing pulse oximeter..");
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

// This function connects to the MQTT broker
void initMQTT() {
  // Set our MQTT broker address and port
  client.setServer(mqtt_server, 1883);
  client.setClient(espClient);

  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    Serial.println("Attempt to connect to MQTT broker");
    char clientId[100];
    WiFi.macAddress().toCharArray(clientId, 100 );
    client.connect(clientId);
    //Two short blinks - MQTT connection fails
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    // Wait some time to space out connection requests
    delay(3000);
  }

  Serial.println("MQTT connected");
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);  

  int timeout = 10 * 4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
   Serial.println("Failed to connect, going back to sleep");
   while (1==1) { //Blink non stop indicates WiFi connection failed
      digitalWrite(ledPin, HIGH);
      delay(300);
      digitalWrite(ledPin, LOW);
      delay(300);
     }
  }

  Serial.print("WiFi connected in: "); 
  Serial.print(millis());
  Serial.print(", IP address: "); 
  Serial.print(WiFi.localIP());
  Serial.print(", MAC address: "); 
  Serial.println(WiFi.macAddress());
 }
