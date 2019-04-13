// WiFi RSSI displayed on Wemos OLED screen

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Put your wifi name and pass here, or define externally
#ifndef WIFI_SSID
#define WIFI_SSID "your ssid"
#endif

#ifndef WIFI_PASS
#define WIFI_PASS "your password"
#endif

#include <WiFi.h>

#define OLED_RESET 0  // GPIO0
#define OLED_ADDRESS 0x3C // for the 64x48 display
Adafruit_SSD1306 display(OLED_RESET);

void showSplashScreen();

void setup()   {
  // Start display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();

  showSplashScreen();

  delay(2000);
  display.clearDisplay();
  display.display();

  display.setCursor(0,0);
  display.println("Connect to ");
  display.println(WIFI_SSID);
  display.display();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    display.print(".");
    display.display();
    delay(500);
  }

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connected!");
  display.display();
}


void loop() {
  long rssi = WiFi.RSSI();
  showRSSI(WIFI_SSID, rssi);
}

void showSplashScreen() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("DataTroupe");
  display.println();

  display.setTextSize(2);
  display.println("RSSI");

  display.setTextSize(1);
  display.println();
  display.println("Display!");
  display.display();
}

void showRSSI(const char* ssid, long rssi) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println(ssid);
  display.println();
  display.println("RSSI");
  display.println();
  display.setTextSize(2);
  display.println(rssi);
  display.display();
}