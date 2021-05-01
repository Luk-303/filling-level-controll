#pragma once

#include <Arduino.h>

#define ECHO_PIN D1
#define TRIGGER_PIN D2

class WATER_LEVEL{
private:
    const float fHEIGHT_CISTERN=3.00;
    const float fDIAMETER_CISTERN=9.35;

    long nDuration;
    float fDistance;

    float fWaterlevel;
    float fWaterInLiter;
    float fWaterInPercent;

public:
    WATER_LEVEL();

  
    float getfWater();
    float getfWaterInLiter();
    float getfWaterInPercent();
    
    float measure();
    void calculateWaterlevel();
    void calculateAmountOfWaterInLiter();
    void calculateAmountOfWaterInPercent();
};