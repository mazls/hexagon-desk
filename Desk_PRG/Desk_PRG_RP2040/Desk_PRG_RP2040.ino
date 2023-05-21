//+--------------------------------------------------------------------------
//
// mazls - (c) 2022 Marcel Suffel.  All Rights Reserved.
//
// File: Hexagon Desk Main Prg                 
//
// Description: Programm for Raspberry Pico RP2040; reads inputs from MCP23017 puts in array
//              4 LED channels to one array; Inputs TTP223 touch sensors
//              Hexagonal led pcbs, with 6 WS2812B leds and pins for TTP223 Sensors
//              
//   
//
// History:     May-14-2022     mazls      Created
//              Sep-05-2022     mazls      RP2040 Version 
//
//---------------------------------------------------------------------------

#include "FastLED.h"
#include "Adafruit_MCP23017.h"
//#include "Hexagon.h"


#define LED_ch0             18     //LED PINS: 0;2;4;5
#define LED_ch1             19     //LED PINS: 0;2;4;5
#define LED_ch2             20     //LED PINS: 0;2;4;5
#define LED_ch3             21     //LED PINS: 0;2;4;5
#define Hexagons_channel_0  12     //Hexagons on CH0
#define Hexagons_channel_1  12     //Hexagons on CH1
#define Hexagons_channel_2  12     //Hexagons on CH2
#define Hexagons_channel_3  14     //Hexagons on CH3
#define Hexagons            Hexagons_channel_0+Hexagons_channel_1+Hexagons_channel_2+Hexagons_channel_3 //Total Hexagons
#define PixelPerHexagon     6      //Pixels on Hexagon PCB
#define NUMPIXELS_ch0       Hexagons_channel_0*PixelPerHexagon
#define NUMPIXELS_ch1       Hexagons_channel_1*PixelPerHexagon
#define NUMPIXELS_ch2       Hexagons_channel_2*PixelPerHexagon
#define NUMPIXELS_ch3       Hexagons_channel_3*PixelPerHexagon
#define NUMPIXELS           NUMPIXELS_ch0+NUMPIXELS_ch1+NUMPIXELS_ch2+NUMPIXELS_ch3   //Total pixel count
#define VOLTS               5
#define MILLIAMPS           2000
#define debug               0       //Activates a point runnning along the LEDS; to check PCB Wiring for LEDS


const byte fadeAmt = 2;

/*
#define NUM_COLORS 8
static const CRGB FieldColors [NUM_COLORS] = 
{
    CRGB::Red,
    CRGB::Blue,
    CRGB::Purple,
    CRGB::Green,
    CRGB::Yellow,
    CRGB::White,
    CRGB::Orange,
    CRGB::Indigo,
};




*/
Adafruit_MCP23017 mcp1;
Adafruit_MCP23017 mcp2;
Adafruit_MCP23017 mcp3;
Adafruit_MCP23017 mcp4;

const uint8_t mcp1_addr = 0; // Adresse 0x20 / 0
const uint8_t mcp2_addr = 1; // Adresse 0x21 / 1
const uint8_t mcp3_addr = 2; // Adresse 0x22 / 2
const uint8_t mcp4_addr = 3; // Adresse 0x23 / 3

CRGB leds[NUMPIXELS] = {0};

int touch_ch0[Hexagons_channel_0];
int touch_ch1[Hexagons_channel_1];
int touch_ch2[Hexagons_channel_2];
int touch_ch3[Hexagons_channel_3];
boolean touch[Hexagons] = {LOW};

boolean TouchBtnPState [Hexagons] = {LOW};
unsigned long lastDebounceTime [Hexagons] = {0};    
unsigned long debounceDelay = 200;    

unsigned long lastOffTime [Hexagons] = {0};    
unsigned long OffDelay = 100; 
 
int count [Hexagons] = {0};     




void setup()
{

//Wire.begin();

  Serial.begin(9600);
  
  /*
*/
    //Setting up one array with many strips
    FastLED.addLeds<NEOPIXEL, LED_ch0>(leds, NUMPIXELS_ch0);                                //Start with index 0
    FastLED.addLeds<NEOPIXEL, LED_ch1>(leds, NUMPIXELS_ch0, NUMPIXELS_ch1);                 //change index to number of pixels on ch0
    FastLED.addLeds<NEOPIXEL, LED_ch2>(leds, NUMPIXELS_ch0+NUMPIXELS_ch1, NUMPIXELS_ch2);
    FastLED.addLeds<NEOPIXEL, LED_ch3>(leds, NUMPIXELS_ch0+NUMPIXELS_ch1+NUMPIXELS_ch2, NUMPIXELS_ch3);
    FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MILLIAMPS);  //Current protetction
    FastLED.setBrightness(255); //max brightness
      
      //set adress for mcp
      mcp1.begin(mcp1_addr);
      mcp2.begin(mcp2_addr);
      mcp3.begin(mcp3_addr);
      mcp4.begin(mcp4_addr);
      
      //all mcp pins as inputs
     for (int in=0; in<=15; in++){
      mcp1.pinMode(in, INPUT);
      mcp1.pullUp(in, HIGH); 
      mcp2.pinMode(in, INPUT);
      mcp2.pullUp(in, HIGH); 
      mcp3.pinMode(in, INPUT);
      mcp3.pullUp(in, HIGH); 
      mcp4.pinMode(in, INPUT);
      mcp4.pullUp(in, HIGH);
     }
    
}

void loop()
{
  if(debug==0){
     for (int t=0; t<Hexagons_channel_0; t++){
      touch[t]=mcp1.digitalRead(t);
     }
     for (int t=0; t<Hexagons_channel_1; t++){
      touch[t+Hexagons_channel_0]=mcp2.digitalRead(t);
     }
     for (int t=0; t<Hexagons_channel_2; t++){
      touch[t+Hexagons_channel_0+Hexagons_channel_1]=mcp3.digitalRead(t);
     }
    for (int t=0; t<Hexagons_channel_3; t++){
      touch[t+Hexagons_channel_0+Hexagons_channel_1+Hexagons_channel_2]=mcp4.digitalRead(t);
     } 

  for (int i = 0; i < Hexagons; i++) {
    if (touch[i] != TouchBtnPState[i]) {
      if  ((millis() - lastDebounceTime[i]) > debounceDelay) {
        lastDebounceTime[i] = millis();
        TouchBtnPState[i] = touch[i];
      }
    }
  }
       for (int j=0; j<=Hexagons-1; j++){
        if ((TouchBtnPState[j]==HIGH)){
          for (int k=j*PixelPerHexagon; k<j*PixelPerHexagon+PixelPerHexagon; k++) {
          leds[k] = CRGB::White; //White
            //leds[k] = FieldColors[random(0, NUM_COLORS)];

          }
        }
       }

        for (int j=0; j<=Hexagons-1; j++){  
           if (TouchBtnPState[j]==LOW){
            //if  ((millis() - lastOffTime[j]) > OffDelay) {
             for (int l=j*PixelPerHexagon; l<j*PixelPerHexagon+PixelPerHexagon; l++) {
             leds[l] = leds[l].fadeToBlackBy(fadeAmt);
             }
          }
        }
  }
  else{
        
      for(int i = 0; i < NUMPIXELS; i++) {
      leds[i] = CRGB::White;
      FastLED.show();
      leds[i] = CRGB::Black;
      delay(100);
    }
  }      
      FastLED.show();
}
