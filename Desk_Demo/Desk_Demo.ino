#include "FastLED.h"
#define LED 0
#define NUMPIXELS      49

int touch[7] = {32,33,34,35,25,26,27}; 
bool res[7];

CRGB leds[NUMPIXELS] = {0};

void setup()
{
    Serial.begin(9600);
    FastLED.addLeds<NEOPIXEL, 0>(leds, NUMPIXELS);
    FastLED.setBrightness(255);

   for (int i=0; i<7; i++){
     pinMode(touch[i],INPUT);
   }
}

void loop()
{
      for (int j=0; j<7; j++){
        res[j] = digitalRead(touch[j]);
        Serial.println(res[j]);
        if (res[j]==HIGH){
          for (int k=j*7; k<j*7+7; k++) {
          leds[k] = CRGB::White;
          }
        }
        if (res[j]==LOW){
          for (int l=j*7; l<j*7+7; l++) {
          leds[l] = leds[l].fadeToBlackBy(16);
          }
        }
       }
      FastLED.show();
}
