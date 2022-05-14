//+--------------------------------------------------------------------------
//
// mazls - (c) 2022 Marcel Suffel.  All Rights Reserved.
//
// File: Hexagon Desk Main Prg                 
//
// Description: Programm for ESP32DevKitC; reads inputs from MCP23017 puts in array
//              4 LED channels to one array; Inputs TTP223 touch sensors
//              Hexagonal led pcbs, with 6 WS2812B leds and pins for TTP223 Sensors
//              AsyncElegantOTA for Updates to PCB
//   
//
// History:     May-14-2022     mazls      Created
//
//---------------------------------------------------------------------------

#include "FastLED.h"
#include <Wire.h>
#include "Adafruit_MCP23017.h"  //V1.2.0

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#define LED_ch0             0     //LED PINS: 0;2;4;5
#define LED_ch1             2     //LED PINS: 0;2;4;5
#define LED_ch2             4     //LED PINS: 0;2;4;5
#define LED_ch3             5     //LED PINS: 0;2;4;5
#define Hexagons            61
#define Hexagons_channel_0  15
#define Hexagons_channel_1  15
#define Hexagons_channel_2  15
#define Hexagons_channel_3  16
#define PixelPerHexagon     6
#define NUMPIXELS_ch0       Hexagons_channel_0*PixelPerHexagon
#define NUMPIXELS_ch1       Hexagons_channel_1*PixelPerHexagon
#define NUMPIXELS_ch2       Hexagons_channel_2*PixelPerHexagon
#define NUMPIXELS_ch3       Hexagons_channel_3*PixelPerHexagon
#define NUMPIXELS           NUMPIXELS_ch0+NUMPIXELS_ch1+NUMPIXELS_ch2+NUMPIXELS_ch3   //Total pixel count
#define VOLTS               5
#define MILLIAMPS           6000


const char* ssid = "****";
const char* password = "****";

AsyncWebServer server(80);

Adafruit_MCP23017 mcp1;
Adafruit_MCP23017 mcp2;
Adafruit_MCP23017 mcp3;
Adafruit_MCP23017 mcp4;

const uint8_t mcp1_addr = 0; // Adresse 0x20 / 0
const uint8_t mcp2_addr = 1; // Adresse 0x21 / 1
const uint8_t mcp3_addr = 2; // Adresse 0x22 / 2
const uint8_t mcp4_addr = 6; // Adresse 0x26 / 6 //Sorry for that

CRGB leds[NUMPIXELS] = {0};

int touch_ch0[Hexagons_channel_0];
int touch_ch1[Hexagons_channel_1];
int touch_ch2[Hexagons_channel_2];
int touch_ch3[Hexagons_channel_3];
int touch[Hexagons];

void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");


    
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
      mcp2.pinMode(in, INPUT); 
      mcp3.pinMode(in, INPUT); 
      mcp4.pinMode(in, INPUT);
     }
 
}

void loop()
{
  
    for (int t=0; t<15; t++){
      touch_ch0[t]=mcp1.digitalRead(t);
     }
     for (int t=0; t<15; t++){
      touch_ch1[t]=mcp2.digitalRead(t);
     }
     for (int t=0; t<15; t++){
      touch_ch2[t]=mcp3.digitalRead(t);
     }
    for (int t=0; t<16; t++){
      touch_ch3[t]=mcp4.digitalRead(t);
     } 

     for (int t=0; t<15; t++){
      touch[t]=mcp1.digitalRead(t);
     }
     for (int t=0; t<15; t++){
      touch[t+Hexagons_channel_0]=mcp2.digitalRead(t);
     }
     for (int t=0; t<15; t++){
      touch[t+Hexagons_channel_0+Hexagons_channel_1]=mcp3.digitalRead(t);
     }
    for (int t=0; t<16; t++){
      touch[t+Hexagons_channel_0+Hexagons_channel_1+Hexagons_channel_2]=mcp4.digitalRead(t);
     } 
  /*
   * 
   * 
    #define Hexagons_channel_0  15
    #define Hexagons_channel_1  15
    #define Hexagons_channel_2  15
    #define Hexagons_channel_3  16

    
      //for (int j=0; j<NUMPIXELS; j++){
        //res[j] = digitalRead(touch[j]);
        //Serial.println(res[j]);
        if (mcp4.digitalRead(0)==HIGH){
          for (int k=0; k<NUMPIXELS; k++) {
          leds[k] = CRGB::White;
          }
        }
        if (mcp4.digitalRead(0)==LOW){
          for (int l=0; l<NUMPIXELS; l++) {
          leds[l] = leds[l].fadeToBlackBy(8);
          
        }
       }
      FastLED.show();
*/ 

       Serial.println(touch_ch1[0]);
       for (int j=0; j<=Hexagons-1; j++){
        if (touch[j]==HIGH){
          for (int k=j*PixelPerHexagon; k<j*PixelPerHexagon+PixelPerHexagon; k++) {
          leds[k] = CRGB::White;
          }
        }
        if (touch[j]==LOW){
          for (int l=j*PixelPerHexagon; l<j*PixelPerHexagon+PixelPerHexagon; l++) {
          leds[l] = leds[l].fadeToBlackBy(16);
          }
        }
       }

      FastLED.show();
     

      
    /*   
        // This outer loop will go over each strip, one at a time
  
    // This inner loop will go over each led in the current strip, one at a time
    for(int i = 0; i < NUMPIXELS; i++) {
      leds[i] = CRGB::White;
      FastLED.show();
      leds[i] = CRGB::Black;
      delay(100);
    }
  */ 

}
