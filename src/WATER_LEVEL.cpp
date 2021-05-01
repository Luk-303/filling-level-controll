#include "WATER_LEVEL.h"


float WATER_LEVEL::getfWater(){return fWaterlevel;}
float WATER_LEVEL::getfWaterInLiter(){return fWaterInLiter;}
float WATER_LEVEL::getfWaterInPercent(){return fWaterInPercent;}

float WATER_LEVEL::measure(){
     
    digitalWrite(TRIGGER_PIN,LOW);
    delay(5);
    digitalWrite(TRIGGER_PIN, HIGH);
    delay(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    nDuration=pulseIn(ECHO_PIN,HIGH);

    fDistance=float((nDuration/2) * 0.03432);
    fDistance=(fDistance/100);
    return fDistance;
}

void WATER_LEVEL::calculateWaterlevel(){   
    fWaterlevel=fHEIGHT_CISTERN-fDistance;
}

void WATER_LEVEL::calculateAmountOfWaterInLiter(){
  fWaterInLiter=fWaterlevel*(4*pow(fDIAMETER_CISTERN,2)/3.14)*1000;
}

void WATER_LEVEL::calculateAmountOfWaterInPercent(){
    float fCisternMax=((4*pow(fDIAMETER_CISTERN,2)/3.14)*fHEIGHT_CISTERN);
    float fCisternActual=(fWaterlevel*((pow(fDIAMETER_CISTERN,2)*4)/3.14));
    float fWaterInPercent=fCisternActual*100/fCisternMax;
}

