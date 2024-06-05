#include <WiFi.h>
#include <WebServer.h>

// Definice SSID a hesla pro přístupový bod
const char* ssid = "ESP32_Access_Point";
const char* password = "12345678";

// Vytvoření webového serveru na portu 80
WebServer server(80);

// Proměnné pro ukládání hodnot
String promenna1 = "";
String promenna2 = "";

// Funkce pro zobrazení webové stránky
void handleRoot() {
  String html = "<html><body><h1>Nastavení proměnných</h1>"
                "<form action=\"/save\" method=\"POST\">"
                "Promenna1: <input type=\"text\" name=\"promenna1\" value=\"" + promenna1 + "\"><br>"
                "Promenna2: <input type=\"text\" name=\"promenna2\" value=\"" + promenna2 + "\"><br>"
                "<input type=\"submit\" value=\"Uložit\">"
                "</form></body></html>";
  server.send(200, "text/html", html);
}

// Funkce pro zpracování uložených hodnot
void handleSave() {
  if (server.hasArg("promenna1")) {
    promenna1 = server.arg("promenna1");
  }
  if (server.hasArg("promenna2")) {
    promenna2 = server.arg("promenna2");
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  // Nastavení přístupového bodu
  WiFi.softAP(ssid, password);

  // Výpis IP adresy brány
  Serial.println();
  Serial.print("IP adresa brány: ");
  Serial.println(WiFi.softAPIP());

  // Nastavení obslužných funkcí pro webový server
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  // Spuštění webového serveru
  server.begin();
}

void loop() {
  // Obsluha webového serveru
  server.handleClient();
}
