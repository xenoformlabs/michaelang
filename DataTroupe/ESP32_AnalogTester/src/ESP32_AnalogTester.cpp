
/***************************************************************************************
 *
 *  ESP32_AnalogTester
 *  
 *  Simple MP3 player with volume control for ESP32 on A6 input
 *
 *  Connects to WiFi 
 * 
 * private_config.h contains the wifi credentials information, with just these two lines.
 * Obviously, you should re-define your own
 *    #define WIFI_SSID "Network Name"
 *    #define WIFI_PASSWORD "password"
 * 
 * 
 *  Uses the ESP8266 Audio Libraries, from here:
 *    https://github.com/earlephilhower/ESP8266Audio.git
 * 
 * 
 *  Shows:
 *  (1) Two analog input puts, A4 and A5 in serial window
 *  (2) Uses MSTimer non-blocking timer classes
 *  (3) Neopixel output with the Fast LED libraries
 **************************************************************************************/ 


#include <Arduino.h>

#include <WiFi.h>

// Need these for proper compiling with Visual Studio Code 
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>

#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

// To run,update the SSID info, and upload. Attach line-level speaker input to A0/DAC2

// Enter your WiFi setup here:
#include "private_config.h"
//const char *WIFI_SSID = "network name";
//const char *WIFI_PASSWORD = "password";

//-- FAST LED code for neopixel rings
#define FASTLED_NEEDS_YIELD
#include <FastLED.h>
#define NUM_LEDS 16
CRGBArray<NUM_LEDS> leds;


// MP3 Streams
// GROOVE SALAD
//const char *URL= "http://ice4.somafm.com/groovesalad-128-mp3";

// Live stream jazz
const char *URL="http://jazz.streamr.ru/jazz-64.mp3";


// Audio Player Stuff
AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2S *out;

//-- function declarations 
void serviceVolumeControl();
void serviceMP3();
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
void inputA4();
void inputA5();
void fastLEDShift();
void fastLEDTest();

//-- LED color control
int r = 0;
int rDir = 1;

//-- non-blocking timer
#include "MSTimer.h"
MSTimer displayDataTimer;
MSTimer fastLEDDataTimer;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}


void setup()
{
  Serial.begin(115200);
  //delay(1000);
  Serial.println("Connecting to WiFi");

  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Try forever
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting to WiFi");
    delay(1000);
  }
  Serial.println("Connected");

  file = new AudioFileSourceICYStream(URL);
  file->RegisterMetadataCB(MDCallback, (void*)"ICY");
  buff = new AudioFileSourceBuffer(file, 8096);
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
 
  
  //--this goes to regular I2S
  out = new AudioOutputI2S();
  out->SetPinout(26, 25, 18);


  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  mp3->begin(buff, out);

  displayDataTimer.setTimer(250);
  fastLEDDataTimer.setTimer(2);

   FastLED.addLeds<NEOPIXEL,12>(leds, NUM_LEDS);
}


void loop()
{
  serviceVolumeControl();
  serviceMP3();

  if( displayDataTimer.isExpired() ) {
      inputA4();
      inputA5();
      displayDataTimer.start();
  }

  if( fastLEDDataTimer.isExpired() ) {
    fastLEDShift();
    fastLEDDataTimer.start();
  }
}

//-- float range mapping for volume control
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void serviceVolumeControl() {
  // A6 is our volume control
   //Serial.println(analogRead(A6));
   out->SetGain(mapfloat(analogRead(A6),0.0,4095.0,0.0,3.99));
}

void serviceMP3() {

  static int lastms = 0;

  if (mp3->isRunning()) {
    if (millis()-lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
     }
    if (!mp3->loop()) mp3->stop();
  } else {
    Serial.printf("MP3 done\n");
  }
}

void inputA4() {
  Serial.print("A4 = ");
  Serial.println(analogRead(A4));
}

void inputA5() {
  Serial.print("A5 = ");
  Serial.println(analogRead(A5));
}

void fastLEDShift() {
  r += rDir;
  if( r > 254 || r < 1) 
    rDir = -rDir;

  for(int i = 0; i < NUM_LEDS; i++)
    leds[i] = CRGB(r,0,2*r/3);

  FastLED.show(); 
  //Serial.println(r);
}

