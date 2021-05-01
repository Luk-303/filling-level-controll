#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "WATER_LEVEL.h"
#include "MQTT_PUB.h"



const char* SSID = "xxxxxxxxxxx";
const char* PASSWORD = "xxxxxxxxxxxx";
const char* MQTT_BROKER = "xxxxxxxx";

unsigned long nTimestamp;

unsigned long nPeriod=5000;

WiFiClient WemosD1;
PubSubClient client(WemosD1);
void SetUpWLan(){
        
    WiFi.begin(SSID,PASSWORD); 

    nTimestamp=millis();

        while ((millis() - nTimestamp <= nPeriod)&&WiFi.status()!=WL_CONNECTED){
        yield();
    }
}

void reconnect() {

  long nTimestamp2=millis();

  while (!client.connected()&&(millis() - nTimestamp2 <= nPeriod)) {
  //Verbindungsversuch:
    if (client.connect(PUB_TOPIC)) {
      delay(2000);
    }
    else {
      delay(2000);
      ESP.deepSleep(4200000000);      
      delay(100); 
    }
    yield();
  }
}


void SendToMQTTBroker(){
    String waterLevelBuff;
    String inLiterBuff;
    String inPercentBuff;
  
    waterLevelBuff = String(CalculateWaterLevel());
    inLiterBuff= String(CalculateAmountOfWaterInLiter());
    inPercentBuff=String(CalculateAmountOfWaterInPercent());

    if (Measure()>0.50){
      waterLevelBuff.toCharArray(mqtt_sub_value_WaterLevel,waterLevelBuff.length()+1);
      inLiterBuff.toCharArray(mqtt_sub_value_inLiter,inLiterBuff.length()+1);
      inPercentBuff.toCharArray(mqtt_sub_value_inPercent,inPercentBuff.length()+1);


      client.publish("/dev/level_gauge_cistern_WaterLevel", mqtt_sub_value_WaterLevel,true);
      client.publish("/dev/level_gauge_cistern_inLiter",mqtt_sub_value_inLiter,true);
      client.publish("/dev/level_gauge_cistern_inPercent",mqtt_sub_value_inPercent,true);
    }
    else if (Measure()<=0.50){
      
      client.publish("/dev/level_gauge_cistern_error", "water level is very high!",true);
    }
    else
    {
      client.publish("/dev/level_gauge_cistern_error_procedure", "something went wrong!",true);

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
 



void loop(){
    
    if (!client.connected()) {  
     reconnect();
    }
    SendToMQTTBroker();

    delay(1000);
    ESP.deepSleep(4200000000);
    delay(100);
}


