#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// 1. WiFi Podaci
const char* ssid = "Ime WIFI - a";
const char* password = "sifra za WIFI";

// 2. Firebase Podaci
#define API_KEY "API key" 
#define DATABASE_URL "URL" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi povezan!");

  // Konfiguracija
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  config.signer.test_mode = true; 
  
  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);

  Serial.println("Sustav spreman!");

  pinMode(5, OUTPUT);
  pinMode(36, INPUT);
}

void loop() {
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 1000) {
    lastTime = millis();
    int val = analogRead(36);
    
    Serial.print("Šaljem podatak: ");
    Serial.println(val);

    // Slanje u bazu (RTDB)
    if (Firebase.RTDB.setInt(&fbdo, "/devices/esp32_01/sensor_value", val)) {
      Serial.println("Podatak u bazi.");
    } else {
      Serial.print("GREŠKA: ");
      Serial.println(fbdo.errorReason());
    }
  }

  if (Firebase.RTDB.getString(&fbdo, "/devices/esp32_01/status")) {
    if (fbdo.dataType() == "string") {
      String status = fbdo.stringData();
      if (status == "ON"){
        Serial.println("Ledica upaljena");
        digitalWrite(5, HIGH);
      }
      else{
        Serial.println("Ledica ugasena");
        digitalWrite(5, LOW);
      }
      
    }
  }
}
