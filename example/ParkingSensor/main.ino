/* The following code would connect to WiFi and MQTT brocker
 * then every second it would send the message to the selected MQTT topic
 * <payload><sensorValue>1</sensorValue><sensorID>80:7D:3A:7F:3A:D5</sensorID></payload>
 * sensor value is read from D1 pin. Tested with Infrared Obstacle Avoidance Sensor EK1254
 * Created by antonum 2019
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);

void initWifi();
void initMQTT();

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "YOUR_MOSQUITTO_BROKER_IP";
const char* mqtt_topic = "smart-parking/from_esp8266";
int ledPin = LED_BUILTIN; 
int sensorPin=5; //D1
//GND and VIN (not 3.3v!!!) as power for the sensor.


void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  // We start by connecting to a WiFi network
  initWifi();
 
}

void loop() {
  // Check if we're connected to the MQTT broker
  if (!client.connected()) {
    // If we're not, attempt to reconnect
    initMQTT();
  }
  digitalWrite(ledPin, HIGH);
  delay(1000);
  // Publish a message to a topic
  int sensor=digitalRead(sensorPin);
  //String payload=String(sensor);
  String payload = "<payload>";
  payload += "<sensorValue>"; 
      payload += String(sensor); 
  payload += "</sensorValue>";
  payload += "<sensorID>"; 
      //payload += WiFi.localIP().toString(); //String(mac[0]);
      payload += WiFi.macAddress();
  payload += "</sensorID>";
  payload += "</payload>"; // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish(mqtt_topic, attributes);
  Serial.println(payload);

  digitalWrite(ledPin, LOW);
  delay(1000);
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
