#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const unsigned ECHO_PIN= D1; 
const unsigned TRIGGER_PIN= D2; 

const char* SSID = "xxxxxxxxxxxxxxxxxx";
const char* PASSWORD = "xxxxxxxxxxxxxxx";
const char* MQTT_BROKER = "xxxxxxxxxxxxxxx";

const float HEIGHT_CISTERN=3.00;
const float DIAMETER_CISTERN=9.35;

char mqtt_sub_value_WaterLevel[50];
char mqtt_sub_value_inLiter[50];
char mqtt_sub_value_inPercent[50];

unsigned long timestamp;

unsigned long period=5000;

WiFiClient WemosD1;
PubSubClient client(WemosD1);

float Measure(){

    long duration;
    float distance;

    digitalWrite(TRIGGER_PIN,LOW);
    delay(5);
    digitalWrite(TRIGGER_PIN,HIGH);
    delay(10);
    digitalWrite(TRIGGER_PIN,LOW);
    
    duration=pulseIn(ECHO_PIN,HIGH);

    distance=float((duration/2) * 0.03432);
    return (distance/100);
  
}

float CalculateWaterLevel(){

   float water_level=HEIGHT_CISTERN-Measure();

  return water_level;
}

void SetUpWLan(){
        
    WiFi.begin(SSID,PASSWORD); 

    timestamp=millis();

        while ((millis() - timestamp <= period)&&WiFi.status()!=WL_CONNECTED){
        yield();
    }
}

void reconnect() {

  long timestamp2=millis();

  while (!client.connected()&&(millis() - timestamp2 <= period)) {
       //Verbindungsversuch:
    if (client.connect("/dev/level_gauge_cistern_WaterLevel")) {
      delay(2000);
    } else {
      delay(2000);
      ESP.deepSleep(4200000000);      
      delay(100); 
    }
    yield();
  }
}

float CalculateAmountOfWaterInLiter(){
  float inLiter=(CalculateWaterLevel()*(4*pow(DIAMETER_CISTERN,2)/3.14))*1000;
  return inLiter;
}

float CalculateAmountOfWaterInPercent(){
    float cisternMax=((4*pow(DIAMETER_CISTERN,2)/3.14)*HEIGHT_CISTERN);
    float cisternActual=(CalculateWaterLevel()*((pow(DIAMETER_CISTERN,2)*4)/3.14));
    float inPercent=cisternActual*100/cisternMax;

  return inPercent;
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



