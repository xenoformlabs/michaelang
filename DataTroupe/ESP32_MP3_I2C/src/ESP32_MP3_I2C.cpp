

/***************************************************************************************
 *
 *  ESP32_MP3_I2C
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
 **************************************************************************************/ 

#include <Arduino.h>


//-- function declarations (more convenient in this cpp file)
void serviceVolumeControl();
void serviceMP3();
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);

#include <WiFi.h>

// Need these for proper compiling with Visual Studio Code 
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>

// Libraries from the ESP
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

// To run,update the SSID info, and upload. Attach line-level speaker input to A0/DAC2

// Enter your WiFi setup here:
#include "private_config.h"
//const char *WIFI_SSID = "network name";
//const char *WIFI_PASSWORD = "password";


// MP3 Streams
// GROOVE SALAD
const char *URL= "http://ice4.somafm.com/groovesalad-128-mp3";

// Live stream jazz
//const char *URL="http://jazz.streamr.ru/jazz-64.mp3";


// Audio Player Stuff
AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2S *out;


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

  //-- change the pinout, so we donfree up I2C on pin 22
  out->SetPinout(26, 25, 18);


  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  mp3->begin(buff, out);
}


void loop()
{
  serviceVolumeControl();
  serviceMP3();

}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//-- Control the volume (gain) of the amp via A6 Potentiometer
void serviceVolumeControl() {
  // A6 is our volume control
   //Serial.println(analogRead(A6));
   out->SetGain(mapfloat(analogRead(A6),0.0,4095.0,0.0,3.99));
}

//-- check the MP3 and loop (or indicate done)
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
    
    //-- restart
    //mp3->begin(buff, out);
    //delay(1000);
  }
}

