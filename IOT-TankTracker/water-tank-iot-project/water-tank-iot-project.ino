//korištene biblioteke
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//podaci za pristup internetu i povezivanje na mrežu
#define WIFI_SSID "93cae6"
#define WIFI_PASSWORD "280729486"

//povezivanje na firebase
#define API_KEY "AIzaSyBbM0alNX8Vtl8qo9rA4UlR1VV5mBKkVrE"
#define DATABASE_URL "https://iot-water-tank-level-project-default-rtdb.europe-west1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

//definiranje pinova za led diode
#define LEDY_PIN D6
#define LEDG_PIN D7
#define LEDB_PIN D8

//definiranje pinova za hc-sr04
#define TRIG_PIN D4
#define ECHO_PIN D5

//definiranje varijabli za hc-sr04
long duration;
float distance;

#define LVL_MAX 5.00
#define LVL_MIN 15.00
#define LVL_HALF1 9.00
#define LVL_HALF2 10.00

void setup() {
  Serial.begin(115200); 
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LEDY_PIN, OUTPUT);
  pinMode(LEDG_PIN, OUTPUT);
  pinMode(LEDB_PIN, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print("."); delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  
  // Provjera slobodne memorije nakon povezivanja na WiFi
  Serial.print("Free heap memory after WiFi connection: ");
  Serial.println(ESP.getFreeHeap());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("signUp OK");
    signupOK= true;
  } else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void sendDataToFirebase() {
  if (Firebase.RTDB.setFloat(&fbdo, "/water_level", distance)) {
    Serial.println("Water level sent to Firebase successfully!");
  } else {
    Serial.print("Failed to send water level: ");
    Serial.println(fbdo.errorReason());
  }

  //statusa rezervoara
  String status = "";
  if (distance > LVL_MIN) {
    status = "Water tank is empty (0%)";
  } else if (distance > LVL_HALF1) {
    status = "Water tank is almost empty (25%)";
  } else if (distance > LVL_HALF2) {
    status = "Water tank is half full (50%)";
  } else if (distance > LVL_MAX) {
    status = "Water tank is almost full (75%)";
  } else {
    status = "Water tank is full! (100%)";
  }


  if (Firebase.RTDB.setString(&fbdo, "/water_status", status)) {
    Serial.println("Water status sent to Firebase successfully!");
  } else {
    Serial.print("Failed to send water status: ");
    Serial.println(fbdo.errorReason());
  }
}

unsigned long prevTriggerMillis = 0;
const unsigned long triggerInterval = 1000;
unsigned long prevStatusMillis = 0;
const unsigned long statusInterval = 5000;
String lastStatus = "";

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    if (millis() - prevTriggerMillis >= triggerInterval) {
      prevTriggerMillis = millis();
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);

      duration = pulseIn(ECHO_PIN, HIGH);
      distance = duration * 0.034 / 2;

      Serial.printf("Water level: %.2f cm\n", distance);
    }
    sendDataToFirebase();
  }

  digitalWrite(LEDY_PIN, LOW);
  digitalWrite(LEDG_PIN, LOW);
  digitalWrite(LEDB_PIN, LOW);

  // Provjera trenutnog statusa rezervoara
  String currentStatus = "";
  if (distance >= LVL_MIN) {
    digitalWrite(LEDY_PIN, HIGH);
    currentStatus = "Water below desired level";
  } else if (distance >= LVL_HALF1 && distance <= LVL_HALF2) {
    digitalWrite(LEDG_PIN, HIGH);
    currentStatus = "Water tank half full";
  } else if (distance <= LVL_MAX) {
    digitalWrite(LEDB_PIN, HIGH);
    currentStatus = "Water tank is full!";
  }

  if ((millis() - prevStatusMillis >= statusInterval) || currentStatus != lastStatus) {
    prevStatusMillis = millis();
    if (currentStatus != "") {
      Serial.println(currentStatus);
      lastStatus = currentStatus;
    }
  }
}
