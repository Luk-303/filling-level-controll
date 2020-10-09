/*
Author: Luk-303

An selfmade IoT device to get the actual water level of a water tank and transmit it to my personal NAS

September 2020
*/



#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//variables to measure the distance
const unsigned ECHO_PIN= D1; 
const unsigned TRIGGER_PIN= D2; 

//variables for connecting WiFi
const char* SSID = "+++++++++++++";
const char* PASSWORD = "+++++++++++";
const char* MQTT_BROKER = "+++++++++++++";

const int HEIGHT_CISTERN=400;

char mqtt_sub_value[50];


WiFiClient WemosD1;
PubSubClient client(WemosD1);

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

int CalculateWaterLevel(){
   int water_level=HEIGHT_CISTERN-Measure();
  return water_level;
}

void SetUpWLan(){
        
    WiFi.begin(SSID,PASSWORD);

    while(WiFi.status()!=WL_CONNECTED){
        delay(500);
        Serial.println(".");
    }

    Serial.println(" ");
    Serial.println("WiFi connected!");
    
    Serial.println("IP-Address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
 
  while (!client.connected()) {
    Serial.print("Trying to connect with MQTT.");

    //Verbindungsversuch:
    if (client.connect("/dev/level_gauge_cistern")) {
      Serial.println("Successful!");
      client.publish("/dev/level_gauge_cistern","It works!");
      client.subscribe("/dev/level_gauge_cistern");
    } else { 
      Serial.print("ERROR!!, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in a few seconds");
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
  
    value_buff =  String(CalculateWaterLevel());

    if (Measure()>50){
      value_buff.toCharArray(mqtt_sub_value,value_buff.length()+1);
      client.publish("/dev/level_gauge_cistern", mqtt_sub_value,true);
    }
    else if (Measure()<=50){
      
      client.publish("/dev/level_gauge_cistern", "water level is very high!",true);
    }
    else
    {
      client.publish("/dev/level_gauge_cistern", "something went wrong!",true);
    }
    


}

void loop(){

    if (!client.connected()) {  
     reconnect();
    }
    SendToMQTTBroker();

    delay(2000);

    ESP.deepSleep(4.26e9);
}
