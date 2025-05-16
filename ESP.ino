#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const int RXPin = D3, TXPin = D4;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;      // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin);      // The serial connection to the GPS device
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define DATABASE_URL "https://animal-monitoring-f36c4-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyBk6Gui05ktYI6mPu-3fuaThaHyXnOCx2w"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String data = "";
bool signupOK = false;
float Lat,Lng,Ecg;
String FEcg = "Wear sensor";
SoftwareSerial bluetooth(3, 1); //RX(GPIO3), TX(GPIO1)


void setup()
{
  bluetooth.begin(9600);
  Serial.begin(9600);
  ss.begin(GPSBaud);
  WiFi.begin("KSRM", "KSRM@123");

    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("Firebase authentication successful!");
        signupOK = true;
    } else {
        Serial.printf("Firebase signup error: %s\n", config.signer.signupError.message.c_str());
    }

    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    pinMode(14, INPUT); // Setup for leads off detection LO +
    pinMode(12, INPUT); // Setup for leads off detection LO -
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  if((digitalRead(10) == 1)||(digitalRead(11) == 1)){
    Serial.println('!');
    bluetooth.print("!!");
    bluetooth.print(";");
    if (Firebase.ready() && signupOK) {
      Firebase.RTDB.setString(&fbdo, "Project/ECG", FEcg);     
    }
    else {
      Serial.println("Error: Failed to update ECG");
    }
  }
  else{
    Serial.println(analogRead(A0));   // send the value of analog input 0:
    bluetooth.print(Ecg);
    bluetooth.print(";");
    Ecg = analogRead(A0);
    if (Firebase.ready() && signupOK) {
      Firebase.RTDB.setString(&fbdo, "Project/ECG", Ecg);     
    }
    else {
      Serial.println("Error: Failed to update ECG");
    }
  }
}


void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.println(gps.location.lng(), 6);
    Lat = gps.location.lat();
    Lng = gps.location.lng();
    bluetooth.print(Lat);
    bluetooth.print(";");
    bluetooth.print(Lng);
    bluetooth.print(";");
    if (Firebase.ready() && signupOK) {
      Firebase.RTDB.setString(&fbdo, "Project/LAT", Lat);
      Firebase.RTDB.setString(&fbdo, "Project/LNG", Lng);      
    }
    else {
      Serial.println("Error: Failed to update location");
    }
  }
  else
  {
    Serial.println(F("INVALID"));
  }
  delay (500);
}
