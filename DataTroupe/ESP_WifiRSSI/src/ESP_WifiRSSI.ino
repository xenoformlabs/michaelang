// WiFi RSSI displayed on Wemos OLED screen

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LOLIN_I2C_BUTTON.h>

#ifdef ESP32
#include <ESP32Ping.h>
#endif

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
I2C_BUTTON button(DEFAULT_I2C_BUTTON_ADDRESS); //I2C Address 0x31

// What to measure and display
enum MEASURE_MODE { MEASURE_RSSI, MEASURE_HTTP, MEASURE_PING };

void showSplashScreen();

// TODO - ping mode is not working
MEASURE_MODE measureMode = MEASURE_RSSI;

void setup()   {

  // Start display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();

  Serial.begin(115200);

  showSplashScreen();

  delay(1000);
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
  if (button.get() == 0) { // Button pressed
    if (button.BUTTON_B) {
      nextMode();
    }
  }

  switch (measureMode) {
    case MEASURE_RSSI:
      measureRSSI();
      break;

    case MEASURE_PING:
      measurePing();
      delay(1000);
      break;

    case MEASURE_HTTP:
      measureHTTP();
      delay(1000);
      break;
  }
}

void nextMode() {
  switch (measureMode) {
    case MEASURE_RSSI:
      measureMode = MEASURE_HTTP;
      break;
    
    case MEASURE_HTTP:
      measureMode = MEASURE_RSSI;
      break;

    // MEASURE_PING not working

    default:
      measureMode = MEASURE_RSSI;

  }
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

void measureRSSI() {
  long rssi = WiFi.RSSI();
  showRSSI(WIFI_SSID, rssi);
}

// XXX example from library not working
void measurePing() {
  IPAddress remote = WiFi.gatewayIP();
//  IPAddress remote = WiFi.localIP();
//  char *remote = "8.8.8.8";
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("Ping");
  display.println(remote);
  display.display();

  if (Ping.ping(remote)) {
    display.println("Success");
  } else {
    display.println("FAIL");
  }

  display.display();
}

void measureHTTP() {
  WiFiClient client;
  int port = 80;
  IPAddress host = WiFi.gatewayIP();
  String url = "/";
  //const char *host = "www.michaelang.com";
  //String url = "/xfer/hi.txt";
  
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setCursor(0,0);

  display.println("HTTP");
  display.println(host);
  display.display();
  if (!client.connect(host, port)) {
    display.println("FAIL");
    display.display();
    delay(500);
    return;
  } else {
    display.println("connected");
    display.display();
    delay(500);
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            display.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    display.clearDisplay();
    display.display();
    display.setCursor(0,0);
    while(client.available()) {
        String line = client.readStringUntil('\r');
        display.print(line);
    }
    display.display();

}