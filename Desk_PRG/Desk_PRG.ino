//+--------------------------------------------------------------------------
//
// mazls - (c) 2022 Marcel Suffel.  All Rights Reserved.
//
// File: Hexagon Desk Main Prg                 
//
// Description: Programm for ESP32DevKitC; reads inputs from MCP23017 puts in array
//              4 LED channels to one array; Inputs TTP223 touch sensors
//              61 Hexagonal led pcbs, with 6 WS2812B leds and pins for TTP223 Sensors
//              AsyncElegantOTA for Updates to PCB
//   
//
// History:     May-14-2022     mazls      Created
//              May-18-2022     mazls      WifiHostname; Debounce; 
//
//---------------------------------------------------------------------------

#include "FastLED.h"
#include <Wire.h>
#include "Adafruit_MCP23017.h"  //V1.2.0
//#include "Hexagon.h"

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


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
#define MILLIAMPS           2000

const byte fadeAmt = 64;

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


String hostname = "Hexagon LED Table";

const char* ssid = "*****";
const char* password = "****";

AsyncWebServer server(80);
/*

*/
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
boolean touch[Hexagons] = {LOW};

//boolean TouchBtn [Hexagons] = {LOW};
boolean TouchBtnPState [Hexagons] = {LOW};
unsigned long lastDebounceTime [Hexagons] = {0};    
unsigned long debounceDelay = 200;    

unsigned long lastOffTime [Hexagons] = {0};    
unsigned long OffDelay = 100; 
 
int count [Hexagons] = {0};     
//unsigned long previousMillis = 0; 
//const long interval = 50; 

void setup()
{



  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable   detector
  Serial.begin(9600);
  
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str()); //define hostname
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
             //lastOffTime[j] = millis();
            //}
          }
        }
        
       
      FastLED.show();
     

      
    /*   
        //Just for testing if all LEDs work
        
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
