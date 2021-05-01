#pragma once

#include <PubSubClient.h>
#define PUB_TOPIC "/dev/level_gauge_cistern_WaterLevel"

class MQTT_PUB{
private:

    String waterLevelBuff;
    String inLiterBuff;
    String inPercentBuff;

    char cMqttSubValueWaterlevel[10];
    char cMqttSubValueInLiter[10];
    char cMqttSubValueInPercent[10];

public:
    MQTT_PUB();
    

};