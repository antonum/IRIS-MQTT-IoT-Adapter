# Simple Pass Trough IRIS Production

### Installation

- Create default credentials for Java Business Host with _SYSTEM credentials. (Interoperability->Configure->Credentials)

- Create new Production

- Add Java Gateway Initiator business service

- Add org.eclipse.paho.client.mqttv3-1.2.0.jar to the Classpath parameter of Java Gateway Initiator

- Build Java Business host Business Service and Business Operation (Interoperability->Build->Java Business Host) from IRIS-MQTT-Client.jar file. Use previosly created credentials and XML as "Format of Incoming Data". Everything else - default. 

- Load all the xml files from the repo into IRIS namespace.

- Load XML schemas mqtt_schema.xml and mqtt_schema_plaintext.xml as XML VDoc schemas (Interoperability -> Interoperate -> XML -> XML Schema Structures)

- For MQTTOut Operation you might need to adjust LogFile in Addidtional Settings. File should exist and IRIS should have write access to it. (Might need to touch and chmod). To be addressed by https://github.com/antonum/IRIS-MQTT-IoT-Adapter/issues/1#issue-436244831

### Configuration

Production is configured to use localhost/127.0.0.1 MQTT brocker on port 1883. No authentication.
Inbound mqtt topic iris/in. Outbound - iris/out

![alt text](https://github.com/antonum/IRIS-MQTT-IoT-Adapter/raw/master/example/PassThrough/production.png)
 
### Testing

In one terminal window subscribe to the iris/out topic:

`mosquitto_sub -t iris/out`

In another - publish message to iris/in topic:

`mosquitto_pub -t iris/in -m "Hello IRIS"`

If everything is configured correctly - you should see "Hello IRIS" message appearing in the "subscribe" terminal session.

You should also see messages flowing through IRIS.

![alt text](https://github.com/antonum/IRIS-MQTT-IoT-Adapter/raw/master/example/PassThrough/message_trace.png)
