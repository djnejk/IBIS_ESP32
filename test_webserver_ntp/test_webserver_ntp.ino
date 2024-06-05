#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Nastavení pro Access Point
const char* ssid_ap = "ESP32_AP";
const char* password_ap = "12345678";

// Nastavení pro Station (připojení k místní WiFi síti)
const char* ssid_sta = "I'm Under Your Bed!";
const char* password_sta = "****";

// Vytvoření webového serveru na portu 80
WebServer server(80);

// Vytvoření UDP klienta pro NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

void setup() {
  Serial.begin(115200);

  // Nastavení ESP32 jako Access Point
  WiFi.softAP(ssid_ap, password_ap);
  Serial.println("Access Point started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Připojení k místní WiFi síti
  WiFi.begin(ssid_sta, password_sta);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("STA IP Address: ");
  Serial.println(WiFi.localIP());

  // Spuštění NTP klienta
  timeClient.begin();

  // Nastavení obsluhy webového serveru
  server.on("/", handleRoot);
  server.on("/submit", handleForm);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Aktualizace času z NTP serveru
  timeClient.update();

  // Obsluha webového serveru
  server.handleClient();
}

void handleRoot() {
  String currentTime = timeClient.getFormattedTime();
  String html = "<html><body>";
  html += "<h1>ESP32 Web Server</h1>";
  html += "<p>Current Time: " + currentTime + "</p>";
  html += "<form action=\"/submit\" method=\"POST\">";
  html += "Input: <input type=\"text\" name=\"input\">";
  html += "<input type=\"submit\" value=\"Submit\">";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleForm() {
  String inputValue = server.arg("input");
  Serial.println("Input value: " + inputValue);
  server.send(200, "text/plain", "Input received: " + inputValue);
}
