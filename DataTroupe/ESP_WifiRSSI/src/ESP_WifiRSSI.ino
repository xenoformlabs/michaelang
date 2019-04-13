// WiFi RSSI displayed on Wemos OLED screen

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 0  // GPIO0
#define OLED_ADDRESS 0x3C // for the 64x48 display
Adafruit_SSD1306 display(OLED_RESET);

void showSplashScreen();

void setup()   {
  Serial.begin(115200);

  // Start display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();

  showSplashScreen();

  delay(2000);
  display.clearDisplay();
  display.display();
}


void loop() {

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