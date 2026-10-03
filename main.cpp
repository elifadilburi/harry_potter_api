#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

// --- Config Bluetooth & Wi‑Fi
#define TEAM_ID         "A08"
#define BTC_NAME        "ESP32_HARRY_POTTER"
#define SERIAL_BAUD     115200
#define BT_TIMEOUT_MS   30000UL  
#define WIFI_TIMEOUT_MS 15000UL  


const char* API_BASE = "http://proiectia.bogdanflorea.ro/api/harry-potter/characters";

BluetoothSerial SerialBT;

// date primite prin Bluetooth
String incoming;

// date Wi‑Fi
String ssid, pass;
bool wifiConfigured = false;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(500);
  Serial.println("Harry Potter API Client");

  // inițializează Bluetooth SPP
  SerialBT.begin(BTC_NAME);
  Serial.println("Bluetooth SPP pornit ca: " BTC_NAME);

  // set Wi‑Fi în STA și deconectează
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void loop() {
  if (SerialBT.available()) {
    incoming = SerialBT.readStringUntil('\n');
    incoming.trim();
    Serial.println("[BT]> " + incoming);

    DynamicJsonDocument cmd(2048);
    if (deserializeJson(cmd, incoming)) {
      Serial.println("JSON invalid!");
      return;
    }
    String action = cmd["action"];

    if (action == "getNetworks") {
      handleGetNetworks();
    }
    else if (action == "connect") {
      handleConfigWiFi(cmd);
      handleConnectWiFi();
    }
    else if (action == "getData") {
      handleGetCharacters();
    }
    else if (action == "getDetails") {
      String id = cmd["id"].as<String>();
      handleGetCharacter(id);
    }
    else {
      DynamicJsonDocument out(256);
      out["error"] = "Unknown action";
      serializeJson(out, incoming);
      SerialBT.println(incoming);
    }
  }
}

// --- Handlere

void handleGetNetworks() {
  Serial.println("Scanning Wi-Fi...");
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();

  for (int i = 0; i < n; ++i) {
    DynamicJsonDocument out(4096);
    out["ssid"]   = WiFi.SSID(i);
    out["strength"]   = WiFi.RSSI(i);
    out["encryption"] = WiFi.encryptionType(i);
    out["teamId"] = TEAM_ID;
    String s;
    serializeJson(out, s);
    SerialBT.println(s);
  }
  Serial.printf("Found %d networks\n", n);
}

void handleConfigWiFi(DynamicJsonDocument& cmd) {
  ssid = cmd["ssid"].as<String>();
  pass = cmd["password"].as<String>();
  wifiConfigured = true;
  Serial.printf("Config Wi‑Fi: SSID=%s PASS_LEN=%d\n", ssid.c_str(), pass.length());
}

void handleConnectWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid.c_str(), pass.c_str());
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  DynamicJsonDocument out(256);
  out["teamId"] = TEAM_ID;
  out["ssid"] = ssid;
  if (WiFi.status() == WL_CONNECTED) {
    out["connected"] = true;
  } else {
    out["connected"] = false;
  }
  String s; serializeJson(out, s);
  SerialBT.println(s);
}

void handleGetCharacters() {
  if (WiFi.status() != WL_CONNECTED) {
    DynamicJsonDocument out(256);
    out["error"] = "NoWiFi";
    out["teamId"] = TEAM_ID;
    String s;
    serializeJson(out, s);
    SerialBT.println(s);
    return;
  }
  String url = String(API_BASE);
  Serial.printf("HTTP GET %s\n", url.c_str());
  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  if (code == 200) {
    String out = http.getString();
    DynamicJsonDocument doc(3072);
    deserializeJson(doc, out.c_str());
    JsonArray list = doc.as<JsonArray>();
    for(JsonVariant val : list) {
      JsonObject listItem = val.as<JsonObject>();
      DynamicJsonDocument outputDocument(3072);

      outputDocument["id"] = listItem["id"];
      outputDocument["name"] = listItem["name"];
      outputDocument["image"] = listItem["image"];
      outputDocument["teamId"] = TEAM_ID;
      
      String output;
      serializeJson(outputDocument, output);
      SerialBT.println(output);
    }
  }
  http.end();
}

void handleGetCharacter(const String& id) {
  if (WiFi.status() != WL_CONNECTED) {
    DynamicJsonDocument out(256);
    out["error"] = "NoWiFi";
    out["teamId"] = TEAM_ID;
    String s; serializeJson(out, s);
    SerialBT.println(s);
    return;
  }
  String url = "http://proiectia.bogdanflorea.ro/api/harry-potter/character?id=" + id;
  Serial.printf("HTTP GET %s\n", url.c_str());
  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  if (code == 200) {
    String output = http.getString();
    DynamicJsonDocument JSONDocument(3072);
    DeserializationError error = deserializeJson(JSONDocument, output.c_str());
    DynamicJsonDocument productDoc(3072);
    
    // Extract Harry Potter character fields
    int char_id = JSONDocument["id"].as<int>();
    String name = JSONDocument["name"].as<String>();
    String species = JSONDocument["species"].as<String>();
    String gender = JSONDocument["gender"].as<String>();
    String house = JSONDocument["house"].as<String>();
    String dateOfBirth = JSONDocument["dateOfBirth"].as<String>();
    int yearOfBirth = JSONDocument["yearOfBirth"].as<int>();
    bool wizard = JSONDocument["wizard"].as<bool>();
    String ancestry = JSONDocument["ancestry"].as<String>();
    String eyeColour = JSONDocument["eyeColour"].as<String>();
    String hairColour = JSONDocument["hairColour"].as<String>();
    String patronus = JSONDocument["patronus"].as<String>();
    bool hogwartsStudent = JSONDocument["hogwartsStudent"].as<bool>();
    bool hogwartsStaff = JSONDocument["hogwartsStaff"].as<bool>();
    String actor = JSONDocument["actor"].as<String>();
    JsonArray alternate_actors = JSONDocument["alternate_actors"].as<JsonArray>();
    String alternate_actors_list = "";
    for(JsonVariant value : alternate_actors) {
        if(alternate_actors_list != "") alternate_actors_list += ", ";
        alternate_actors_list += value.as<String>();
    }
    bool alive = JSONDocument["alive"].as<bool>();
    String image = JSONDocument["image"].as<String>();

    String outputDescription = (
      "Species: " + species + "\n" + 
      "Gender: " + gender + "\n" + 
      "House: " + house + "\n" + 
      "Date of Birth: " + dateOfBirth + "\n" + 
      "Year of Birth: " + String(yearOfBirth) + "\n" + 
      "Wizard: " + (wizard ? "Yes" : "No") + "\n" + 
      "Ancestry: " + ancestry + "\n" + 
      "Eye Colour: " + eyeColour + "\n" + 
      "Hair Colour: " + hairColour + "\n" + 
      "Patronus: " + patronus + "\n" + 
      "Hogwarts Student: " + (hogwartsStudent ? "Yes" : "No") + "\n" + 
      "Hogwarts Staff: " + (hogwartsStaff ? "Yes" : "No") + "\n" + 
      "Actor: " + actor + "\n" + 
      "Alternate Actors: " + alternate_actors_list + "\n" + 
      "Alive: " + (alive ? "Yes" : "No") + "\n"
    );
    
    productDoc["id"] = id;
    productDoc["name"] = name;
    productDoc["image"] = image;
    productDoc["description"] = outputDescription;
    productDoc["teamId"] = TEAM_ID;
    String jsonOutput;
    serializeJson(productDoc, jsonOutput);
    SerialBT.println(jsonOutput);
  }
  http.end();
}