#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>
#define FIREBASE_HOST "tempmonitor-32b51-default-rtdb.asia-southeast1.firebasedatabase.app"      
#define FIREBASE_AUTH "YOURAUTHKEY"            
#define WIFI_SSID "vid12"                                  
#define WIFI_PASSWORD "12345678"            

#define DHTPIN D2                                            // Digital pin connected to DHT11
#define DHTTYPE DHT22                                        // Initialize dht type as DHT 11
DHT dht(DHTPIN, DHTTYPE);                                                    

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Connect WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  Serial.printf("Humidity: %.2f %%  Temperature: %.2f °C\n", h, t);
String controlValue;

  if (Firebase.getString(fbdo, "/DHT11/Control")) {
      controlValue = fbdo.stringData();
      Serial.printf("Control value: %s\n", controlValue.c_str());
  } else {
      Serial.println(fbdo.errorReason());
  }
  if(strstr(controlValue.c_str(),"1") != NULL)
  {
    // Send data to Firebase
    if (Firebase.setFloat(fbdo, "/DHT11/Humidity", h)) {
      Serial.println("Humidity updated");
    } else {
      Serial.print("Error updating Humidity: ");
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.setFloat(fbdo, "/DHT11/Temperature", t)) {
      Serial.println("Temperature updated");
    } else {
      Serial.print("Error updating Temperature: ");
      Serial.println(fbdo.errorReason());
    }
  }
  
  delay(5000);
}