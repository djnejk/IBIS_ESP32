#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#include <esp_system.h>  // Knihovna potřebná pro funkci esp_restart()

Preferences preferences;

// Nastavení pro Access Point
const char* ssid_ap = "ESP32_AP";
const char* password_ap = "12345678";

String ssid_sta = "";
String password_sta = "";

// Vytvoření webového serveru na portu 80
WebServer server(80);

// Vytvoření UDP klienta pro NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

void setup() {
  Serial.begin(115200);

  // Inicializace Preferences s názvem prostoru "my-app"
  preferences.begin("my-app", false);

  // Nastavení pro Station (připojení k místní WiFi síti)
  ssid_sta = preferences.getString("ssid_sta", "default");
  password_sta = preferences.getString("password_sta", "default");

  // Ukončení práce s Preferences
  preferences.end();

  // Nastavení ESP32 jako Access Point
  WiFi.softAP(ssid_ap, password_ap);
  Serial.println("Access Point started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Připojení k místní WiFi síti
  WiFi.begin(ssid_sta, password_sta);

  // Zkoušíme se připojit k WiFi po dobu 10 sekund
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  // Pokud se připojení nezdařilo
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi. Continuing as Access Point.");
  } else {
    Serial.println("Connected to WiFi");
    Serial.print("STA IP Address: ");
    Serial.println(WiFi.localIP());

    // Spuštění NTP klienta
    timeClient.begin();
  }

  // Nastavení obsluhy webového serveru
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Pokud je připojen k WiFi, aktualizace času z NTP serveru
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
  }

  // Obsluha webového serveru
  server.handleClient();
}

// Funkce pro zobrazení webové stránky
void handleRoot() {
  String currentTime = WiFi.status() == WL_CONNECTED ? timeClient.getFormattedTime() : "N/A";

  String html = "<html><head><meta charset=\"UTF-8\"></head><body><h1>Nastavení proměnných</h1>" + currentTime + "<form action=\"/save\" method=\"POST\">"
                                                                            "ssid_sta: <input type=\"text\" name=\"ssid_sta\" value=\""
                + ssid_sta + "\"><br>"
                             "password_sta: <input type=\"text\" name=\"password_sta\" value=\""
                + password_sta + "\"><br>"
                                 "<input type=\"submit\" value=\"Uložit\">"
                                 "</form></body></html>";
  server.send(200, "text/html", html);
}

// Funkce pro zpracování uložených hodnot
void handleSave() {
  if (server.hasArg("ssid_sta")) {
    // Inicializace Preferences s názvem prostoru "my-app"
    preferences.begin("my-app", false);
    preferences.putString("ssid_sta", server.arg("ssid_sta"));
    // Ukončení práce s Preferences
    preferences.end();
  }
  if (server.hasArg("password_sta")) {
    // Inicializace Preferences s názvem prostoru "my-app"
    preferences.begin("my-app", false);
    preferences.putString("password_sta", server.arg("password_sta"));
    // Ukončení práce s Preferences
    preferences.end();
  }
  // Zobrazení potvrzovací zprávy
  String html = "<html><head><meta charset=\"UTF-8\"></head><body><h1>Změna údajů sítě WiFi úspěšná, provádím restart ESP32...</h1></body></html>";
  server.send(200, "text/html", html);
  delay(5000); // Krátká prodleva před restartem
  esp_restart();  // Restartuj ESP32
}
