#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//variables to measure the distance
const unsigned ECHO_PIN= D1; 
const unsigned TRIGGER_PIN= D2; 

//variables for connecting WiFi
const char* SSID = "YOUR SSID";
const char* PASSWORD = "YOUR WLAN PASSWORD";
const char* MQTT_BROKER = "THE IP OF YOUR MQTT BROKER";

char mqtt_sub_value[50];

WiFiClient WemosD1;
PubSubClient client(WemosD1);

//function to measure the distance 
int Measure(){

    long duration,distance;

    digitalWrite(TRIGGER_PIN,LOW);
    delay(5);
    digitalWrite(TRIGGER_PIN,HIGH);
    delay(10);
    digitalWrite(TRIGGER_PIN,LOW);
    
    duration=pulseIn(ECHO_PIN,HIGH);

    distance=(duration/2) * 0.03432;

    return distance;
}

//function to connect the Wemos to the internet
void SetUpWLan(){
        
    WiFi.begin(SSID,PASSWORD);

    while(WiFi.status()!=WL_CONNECTED){
        delay(500);
        Serial.println(".");
    }

    Serial.println(" ");
    Serial.println("WiFi verbunden!");
    
    Serial.println("IP-Adresse ist: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
 
  while (!client.connected()) {
    Serial.print("Es wird sich per MQTT verbunnden.");

    //trying to connect to MQTT Broker
    if (client.connect("YOUR TOPIC THE VALUE IS PUBLISHED TO")) {
      Serial.println("Erfolgreich verbunden!");
      client.publish("YOUR TOPIC THE VALUE IS PUBLISHED TO","Messwert wird übertragen:");
      client.subscribe("YOUR TOPIC THE VALUE IS PUBLISHED TO");
    } else { 
      Serial.print("Fehler, rc=");
      Serial.print(client.state());
      Serial.println(" Nächster Versuch in 5 Sekunden");
      delay(5000);
    }
  }
}

void setup()
{
    Serial.begin (74880);

    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    SetUpWLan();
    client.setServer(MQTT_BROKER, 1883);
}
 

void SendToMQTTBroker(){

    String value_buff;
    value_buff =  String(Measure());
    value_buff.toCharArray(mqtt_sub_value,value_buff.length()+1);
    client.publish("YOUR TOPIC THE VALUE IS PUBLISHED TO", mqtt_sub_value);
}

void loop(){

    if (!client.connected()) {  
     reconnect();
    }
    SendToMQTTBroker();

    delay(2000);

    ESP.deepSleep(4.26e9);
}