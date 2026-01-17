#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>


/* ========= I2C PINS ========= */
#define SDA_PIN 21
#define SCL_PIN 22


/* ========= LCD ========= */
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Try 0x3F if 0x27 doesn't work


/* ========= WIFI LIST ========= */
struct WiFiCred {
 const char* ssid;
 const char* pass;
};


WiFiCred wifiList[] = {
 {"Excitel_HARSHIT_2.4G", "dinesh480tak"},
 {"ESP-404",             "k756756756"},
 {"SKIT-WiFi 2.4G",      "skit@2024"}
};


const int WIFI_COUNT = sizeof(wifiList) / sizeof(wifiList[0]);
bool wifiOK = false;


/* ========= WEB ========= */
WebServer server(80);


/* ========= VAR ========= */
unsigned long lastLCD = 0;
bool otaRunning = false;


/* ========= WEB PAGE ========= */
void handleRoot() {
 server.send(200, "text/html",
   "<h2>ESP32 STATUS</h2>"
   "<p>WiFi: Connected</p>"
   "<p>IP: " + WiFi.localIP().toString() + "</p>"
   "<p>RSSI: " + String(WiFi.RSSI()) + " dBm</p>"
 );
}


void setup() {
 Serial.begin(115200);
 delay(1000);
 Serial.println("Starting ESP32 with I2C on GPIO21(SDA), GPIO22(SCL)");
  /* ----- I2C ----- */
 Wire.begin(SDA_PIN, SCL_PIN);
 Serial.println("I2C initialized");
  /* ----- LCD ----- */
 lcd.init();
 lcd.backlight();
 lcd.setCursor(0,0);
 lcd.print("ESP32 WROOM-32");
 lcd.setCursor(0,1);
 lcd.print("I2C:21,22");
 Serial.println("LCD initialized");
 delay(2000);
  /* ----- WIFI MULTI CONNECT ----- */
 lcd.clear();
 lcd.print("WiFi Searching...");
 Serial.println("Starting WiFi connection...");
  WiFi.mode(WIFI_STA);
 WiFi.setSleep(false);
  for (int i = 0; i < WIFI_COUNT && !wifiOK; i++) {
   lcd.clear();
   lcd.print("Trying WiFi:");
   lcd.setCursor(0,1);
   // Show only first 16 chars of SSID
   String ssidDisplay = String(wifiList[i].ssid);
   if (ssidDisplay.length() > 16) {
     ssidDisplay = ssidDisplay.substring(0, 13) + "...";
   }
   lcd.print(ssidDisplay);
  
   Serial.print("Trying WiFi: ");
   Serial.println(wifiList[i].ssid);
  
   WiFi.begin(wifiList[i].ssid, wifiList[i].pass);
  
   unsigned long startAttempt = millis();
   int dots = 0;
   while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 20000) {
     delay(500);
     Serial.print(".");
     dots++;
     if (dots % 40 == 0) Serial.println();
   }
   Serial.println();
  
   if (WiFi.status() == WL_CONNECTED) {
     wifiOK = true;
     lcd.clear();
     lcd.print("WiFi Connected!");
     delay(1000);
     lcd.clear();
     lcd.print("IP:");
     lcd.setCursor(0,1);
     String ipStr = WiFi.localIP().toString();
     lcd.print(ipStr);
    
     Serial.print("WiFi Connected! IP: ");
     Serial.println(ipStr);
     Serial.print("RSSI: ");
     Serial.print(WiFi.RSSI());
     Serial.println(" dBm");
     Serial.print("Gateway IP: ");
     Serial.println(WiFi.gatewayIP());
    
     // Show connection for 3 seconds
     delay(3000);
   } else {
     Serial.println("WiFi connection failed for this network");
     WiFi.disconnect(true);
     delay(1000);
   }
 }
  if (!wifiOK) {
   lcd.clear();
   lcd.print("WiFi FAILED");
   lcd.setCursor(0,1);
   lcd.print("Check Credentials");
   Serial.println("All WiFi connections failed!");
   delay(3000);
  
   // Fallback to AP mode
   lcd.clear();
   lcd.print("Starting AP...");
   WiFi.mode(WIFI_AP);
   bool apStarted = WiFi.softAP("ESP32-AP", "12345678");
  
   if (apStarted) {
     lcd.clear();
     lcd.print("AP Mode Active");
     lcd.setCursor(0,1);
     lcd.print("IP: ");
     lcd.print(WiFi.softAPIP());
     Serial.print("AP Mode IP: ");
     Serial.println(WiFi.softAPIP());
     wifiOK = true;
   } else {
     lcd.clear();
     lcd.print("AP Failed Too");
     Serial.println("AP mode also failed!");
   }
   delay(3000);
 }
  /* ----- WEB SERVER ----- */
 server.on("/", handleRoot);
 server.begin();
 Serial.println("HTTP server started");
  /* ----- OTA ----- */
 ArduinoOTA.setHostname("ESP32-WROOM");
  ArduinoOTA.onStart([]() {
   otaRunning = true;
   lcd.clear();
   lcd.print("OTA Updating");
   Serial.println("OTA Start");
 });
  ArduinoOTA.onEnd([]() {
   otaRunning = false;
   lcd.clear();
   lcd.print("OTA Done");
   Serial.println("OTA End");
   delay(2000);
 });
  ArduinoOTA.onProgress([](unsigned int p, unsigned int t) {
   lcd.setCursor(0,1);
   int percent = (p * 100) / t;
   lcd.print(percent);
   lcd.print("%   ");
 });
  ArduinoOTA.onError([](ota_error_t error) {
   Serial.printf("OTA Error[%u]: ", error);
   lcd.clear();
   lcd.print("OTA Error!");
 });
  ArduinoOTA.begin();
 Serial.println("OTA Ready");
  lcd.clear();
 lcd.print("System Ready");
 lcd.setCursor(0,1);
 if (WiFi.status() == WL_CONNECTED) {
   lcd.print("STA Mode");
 } else if (WiFi.getMode() & WIFI_AP) {
   lcd.print("AP Mode");
 } else {
   lcd.print("No Network");
 }
 delay(2000);
}


void loop() {
 ArduinoOTA.handle();
 server.handleClient();
  if (!otaRunning && millis() - lastLCD > 3000) {
   lastLCD = millis();
   lcd.clear();
  
   if (WiFi.status() == WL_CONNECTED) {
     lcd.print("WiFi: Connected");
     lcd.setCursor(0,1);
     String ipStr = WiFi.localIP().toString();
     if (ipStr.length() <= 16) {
       lcd.print(ipStr);
     } else {
       // Display first part
       lcd.print(ipStr.substring(0, 16));
       delay(2000);
       lcd.clear();
       lcd.print(ipStr.substring(16));
       lcd.setCursor(0,1);
       lcd.print("RSSI:");
       lcd.print(WiFi.RSSI());
       lcd.print("dBm");
     }
   } else if (WiFi.getMode() & WIFI_AP) {
     lcd.print("AP Mode Active");
     lcd.setCursor(0,1);
     lcd.print(WiFi.softAPIP());
   } else {
     lcd.print("No Network");
     lcd.setCursor(0,1);
     lcd.print("Check WiFi");
   }
 }
}

