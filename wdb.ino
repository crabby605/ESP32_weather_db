#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

const char* ssid = "URSSID";
const char* password = "URPASSWD";

const char* weather_api_url = "https://api.open-meteo.com/v1/forecast?latitude=URLAT&longitude=URLONG&current_weather=true"; //pls put ur coords here

#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void connectToWiFi();
void fetchAndDisplayWeather();
void drawSunnyIcon();
void drawCloudyIcon();
void drawRainyIcon();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Connecting to WiFi...");

  connectToWiFi();

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 10);
  tft.println("Fetching weather...");
}

void loop() {
  fetchAndDisplayWeather();
  delay(600000);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  tft.setCursor(10, 40);
  tft.println("WiFi Connected!");
}

void fetchAndDisplayWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(weather_api_url);

    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println(payload);

      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(10, 10);
        tft.println("Error parsing JSON");
        return;
      }

      float temperature = doc["current_weather"]["temperature"];
      int weather_code = doc["current_weather"]["weathercode"];
      float humidity = doc["current_weather"]["humidity"];
      float wind_speed = doc["current_weather"]["windspeed"];
      String weather_description = doc["current_weather"]["weather_description"].as<String>();

      tft.fillScreen(ILI9341_BLACK);

      tft.setTextSize(2);
      tft.setCursor(10, 10);
      tft.setTextColor(ILI9341_WHITE);
      tft.println("URCITY Weather");
      tft.setCursor(10, 40);
      tft.printf("Temp: %.1f C", temperature);

      tft.setCursor(10, 70);
      if (weather_code == 0) {
        tft.println("Sunny");
        drawSunnyIcon();
      } else if (weather_code == 1) {
        tft.println("Cloudy");
        drawCloudyIcon();
      } else {
        tft.println("Rainy");
        drawRainyIcon();
      }

      tft.setCursor(10, 100);
      tft.printf("Humidity: %.1f%%", humidity);
      tft.setCursor(10, 130);
      tft.printf("Wind Speed: %.1f km/h", wind_speed);

      tft.setCursor(10, 160);
      tft.print("Condition: ");
      tft.println(weather_description);

      tft.setTextSize(1);
      tft.setCursor(10, 210);
      tft.println("Made by Vihaan aka crabby605");

    } else {
      Serial.printf("HTTP request failed with code %d\n", httpCode);
      tft.fillScreen(ILI9341_BLACK);
      tft.setCursor(10, 10);
      tft.println("HTTP request failed");
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected!");
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(10, 10);
    tft.println("WiFi disconnected!");
  }
}
// this will draw some shapes!
void drawSunnyIcon() {
  tft.fillCircle(120, 120, 20, ILI9341_YELLOW);
}

void drawCloudyIcon() {
  tft.fillCircle(120, 120, 20, ILI9341_LIGHTGREY);
  tft.fillCircle(140, 120, 20, ILI9341_LIGHTGREY);
}

void drawRainyIcon() {
  tft.fillCircle(120, 100, 20, ILI9341_LIGHTGREY);
  tft.fillCircle(140, 100, 20, ILI9341_LIGHTGREY);
  tft.fillRect(120, 140, 10, 20, ILI9341_BLUE);
  tft.fillRect(140, 140, 10, 20, ILI9341_BLUE);
}
