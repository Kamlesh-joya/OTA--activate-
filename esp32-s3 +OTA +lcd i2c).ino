#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define SDA_PIN 8
#define SCL_PIN 9
LiquidCrystal_I2C lcd(0x27, 16, 2);


const char* ssid = "Excitel_HARSHIT_2.4G";
const char* password = "dinesh480tak";


unsigned long lastMsg = 0;


void setup() {
  Serial.begin(115200);


  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();


  lcd.setCursor(0,0);
  lcd.print("WiFi Connecting");


  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);          // 🔥 MOST IMPORTANT
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }


  String ip = WiFi.localIP().toString();
  Serial.println("\nConnected: " + ip);


  lcd.clear();
  lcd.print("WiFi Connected");
  lcd.setCursor(0,1);
  lcd.print(ip);


  ArduinoOTA.setHostname("ESP32-S3");


  ArduinoOTA.onStart([]() {
    lcd.clear();
    lcd.print("OTA Updating");
  });


  ArduinoOTA.onEnd([]() {
    lcd.clear();
    lcd.print("OTA Done");
  });


  ArduinoOTA.onError([](ota_error_t error) {
    lcd.clear();
    lcd.print("OTA Error");
  });


  ArduinoOTA.begin();


  delay(500);
  lcd.clear();
  lcd.print("OTA READY");
  lcd.setCursor(0,1);
  lcd.print(ip);
}


void loop() {
  ArduinoOTA.handle();   // 🔥 MUST RUN FREQUENTLY


  // Non-blocking status update (optional)
  if (millis() - lastMsg > 10000) {
    lastMsg = millis();
    Serial.println("OTA Alive");
  }
}


===============================================================
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SDA_PIN 8
#define SCL_PIN 9
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "Excitel_HARSHIT_2.4G";
const char* password = "dinesh480tak";

unsigned long lastMsg = 0;

void setup() {
  // Initialize Serial with delay to ensure connection
  Serial.begin(115200);
  delay(1000); // Give time for serial monitor to connect
  
  // Print startup message to Serial
  Serial.println("\n\n=================================");
  Serial.println("ESP32-S3 OTA Update System");
  Serial.println("=================================\n");

  // Initialize I2C and LCD
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connecting");

  // Print to Serial
  Serial.println("Initializing WiFi connection...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  // WiFi setup
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
    attempts++;
    
    if (attempts > 100) { // 30 seconds timeout
      Serial.println("\nFailed to connect to WiFi!");
      lcd.clear();
      lcd.print("WiFi Failed");
      lcd.setCursor(0, 1);
      lcd.print("Check Credentials");
      while(1); // Halt execution
    }
  }

  String ip = WiFi.localIP().toString();
  
  // Print connection success to Serial
  Serial.println("\n✅ WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  // Update LCD
  lcd.clear();
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(ip);

  // OTA Configuration
  ArduinoOTA.setHostname("ESP32-S3");
  
  // Print OTA info to Serial
  Serial.println("\n🔧 Configuring OTA...");
  Serial.println("OTA Hostname: ESP32-S3");

  ArduinoOTA.onStart([]() {
    Serial.println("🔄 OTA Update Started");
    lcd.clear();
    lcd.print("OTA Updating");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\n✅ OTA Update Complete!");
    Serial.println("Rebooting...");
    lcd.clear();
    lcd.print("OTA Done");
    delay(1000);
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    static unsigned long lastPrint = 0;
    unsigned int percent = (progress * 100) / total;
    
    // Print progress every 5% or 500ms
    if (millis() - lastPrint > 500 || percent % 5 == 0) {
      Serial.printf("Progress: %u%%\r", percent);
      lastPrint = millis();
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\n❌ OTA Error [%u]: ", error);
    
    switch(error) {
      case OTA_AUTH_ERROR:
        Serial.println("Authentication Failed");
        lcd.clear();
        lcd.print("Auth Error");
        break;
      case OTA_BEGIN_ERROR:
        Serial.println("Begin Failed");
        lcd.clear();
        lcd.print("Begin Error");
        break;
      case OTA_CONNECT_ERROR:
        Serial.println("Connect Failed");
        lcd.clear();
        lcd.print("Connect Error");
        break;
      case OTA_RECEIVE_ERROR:
        Serial.println("Receive Failed");
        lcd.clear();
        lcd.print("Receive Error");
        break;
      case OTA_END_ERROR:
        Serial.println("End Failed");
        lcd.clear();
        lcd.print("End Error");
        break;
      default:
        Serial.println("Unknown Error");
        lcd.clear();
        lcd.print("Unknown Error");
        break;
    }
  });

  ArduinoOTA.begin();

  Serial.println("\n✅ OTA Ready!");
  Serial.println("=================================");
  Serial.println("Waiting for OTA updates...");
  Serial.println("To upload new firmware:");
  Serial.println("1. Tools → Port → Select 'ESP32-S3' network port");
  Serial.println("2. Click Upload\n");

  // Final display
  delay(500);
  lcd.clear();
  lcd.print("OTA READY");
  lcd.setCursor(0, 1);
  lcd.print(ip);
}

void loop() {
  ArduinoOTA.handle(); // Must run frequently

  // Non-blocking status update
  if (millis() - lastMsg > 10000) {
    lastMsg = millis();
    
    // Print status to Serial
    Serial.println("\n📊 System Status:");
    Serial.print("Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.println("Ready for OTA updates...");
    
    // Optional: Update LCD with uptime
    lcd.clear();
    lcd.print("OTA Ready ");
    lcd.print(millis() / 60000); // Minutes
    lcd.print("m");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
  }
  
  delay(10); // Small delay to prevent watchdog timer issues
}

