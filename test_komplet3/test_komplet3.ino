#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#include <esp_system.h>  // Knihovna potřebná pro funkci esp_restart()
#include <HardwareSerial.h>
#include <TimeLib.h>


// Define the serial port
HardwareSerial mySerial(0);

Preferences preferences;

// Nastavení pro Access Point
const char* ssid_ap = "Prevodnik_IBIS";
const char* password_ap = "ibis1234";
// Nastavení statické IP adresy, brány a masky sítě
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

String ssid_sta = "";
String password_sta = "";

boolean wifi_connected = false;
int cas_delay = 0;
String slovoHEX = "";
boolean led_state = true;

// Vytvoření webového serveru na portu 80
WebServer server(80);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");  // Update interval set to 60 seconds

// Time offset in seconds (for example for CET is 3600 seconds)
const long utcOffsetInSeconds = 3600;


// Přepínač piny
const int Prep_1 = 2;
const int Prep_2 = 3;
const int Prep_4 = 4;
const int Prep_8 = 5;

//led na esp32
const int redPin = 14;
const int greenPin = 15;
const int bluePin = 16;


void setup() {
  // Set up the serial communication with specified settings
  mySerial.begin(1200, SERIAL_7E2);  // 1200 baud rate, 7 data bits, even parity, 2 stop bits
  randomSeed(analogRead(0));

  pinMode(Prep_1, INPUT_PULLUP);
  pinMode(Prep_2, INPUT_PULLUP);
  pinMode(Prep_4, INPUT_PULLUP);
  pinMode(Prep_8, INPUT_PULLUP);


  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  delay(10);


  // Inicializace Preferences s názvem prostoru "my-app"
  preferences.begin("my-app", false);

  // Nastavení pro Station (připojení k místní WiFi síti)
  ssid_sta = preferences.getString("ssid_sta", "default");
  password_sta = preferences.getString("password_sta", "default");

  // Ukončení práce s Preferences
  preferences.end();

  // Nastavení ESP32 jako Access Point
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid_ap, password_ap);

  // Připojení k místní WiFi síti
  WiFi.begin(ssid_sta, password_sta);

  // Zkoušíme se připojit k WiFi po dobu 10 sekund
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
  }

  // Pokud se připojení nezdařilo
  if (WiFi.status() != WL_CONNECTED) {
    wifi_connected = false;
  } else {
    wifi_connected = true;

    // Spuštění NTP klienta
    timeClient.begin();
  }

  // Nastavení obsluhy webového serveru
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/save_positions", HTTP_POST, handleSavePositions);
  server.begin();
}

void loop() {
  //vytvoření "asynchroního" delaye 1s
  if (cas_delay >= 1000) {
    cas_delay = 0;

    int value1 = digitalRead(Prep_1);
    int value2 = digitalRead(Prep_2);
    int value3 = digitalRead(Prep_4);
    int value4 = digitalRead(Prep_8);

    String sendSlovo_a = "";
    String sendSlovo_b = "";
    String sendSlovo_c = "";
    String sendSlovo_d = "";

    // Sluč vstupní hodnoty do jednoho binárního číslaz
    int combinedValue = (value1 << 3) | (value2 << 2) | (value3 << 1) | value4;
    preferences.begin("my-app", false);
    switch (combinedValue) {
      case 0b1111:  //0
        sendSlovo_a = preferences.getString("Pozice0a", "");
        sendSlovo_b = preferences.getString("Pozice0b", "");
        sendSlovo_c = preferences.getString("Pozice0c", "");
        sendSlovo_d = preferences.getString("Pozice0d", "");
        break;
      case 0b0111:  //1
        sendSlovo_a = preferences.getString("Pozice1a", "");
        sendSlovo_b = preferences.getString("Pozice1b", "");
        sendSlovo_c = preferences.getString("Pozice1c", "");
        sendSlovo_d = preferences.getString("Pozice1d", "");
        break;
      case 0b1011:  //2
        sendSlovo_a = preferences.getString("Pozice2a", "");
        sendSlovo_b = preferences.getString("Pozice2b", "");
        sendSlovo_c = preferences.getString("Pozice2c", "");
        sendSlovo_d = preferences.getString("Pozice2d", "");
        break;
      case 0b0011:  //3
        sendSlovo_a = preferences.getString("Pozice3a", "");
        sendSlovo_b = preferences.getString("Pozice3b", "");
        sendSlovo_c = preferences.getString("Pozice3c", "");
        sendSlovo_d = preferences.getString("Pozice3d", "");
        break;
      case 0b1101:  //4
        sendSlovo_a = preferences.getString("Pozice4a", "");
        sendSlovo_b = preferences.getString("Pozice4b", "");
        sendSlovo_c = preferences.getString("Pozice4c", "");
        sendSlovo_d = preferences.getString("Pozice4d", "");
        break;
      case 0b0101:  //5
        sendSlovo_a = preferences.getString("Pozice5a", "");
        sendSlovo_b = preferences.getString("Pozice5b", "");
        sendSlovo_c = preferences.getString("Pozice5c", "");
        sendSlovo_d = preferences.getString("Pozice5d", "");
        break;
      case 0b1001:  //6
        sendSlovo_a = preferences.getString("Pozice6a", "");
        sendSlovo_b = preferences.getString("Pozice6b", "");
        sendSlovo_c = preferences.getString("Pozice6c", "");
        sendSlovo_d = preferences.getString("Pozice6d", "");
        break;
      case 0b0001:  //7
        sendSlovo_a = preferences.getString("Pozice7a", "");
        sendSlovo_b = preferences.getString("Pozice7b", "");
        sendSlovo_c = preferences.getString("Pozice7c", "");
        sendSlovo_d = preferences.getString("Pozice7d", "");
        break;
      case 0b1110:  //8
        sendSlovo_a = preferences.getString("Pozice8a", "");
        sendSlovo_b = preferences.getString("Pozice8b", "");
        sendSlovo_c = preferences.getString("Pozice8c", "");
        sendSlovo_d = preferences.getString("Pozice8d", "");
        break;
      case 0b0110:  //9
        sendSlovo_a = preferences.getString("Pozice9a", "");
        sendSlovo_b = preferences.getString("Pozice9b", "");
        sendSlovo_c = preferences.getString("Pozice9c", "");
        sendSlovo_d = preferences.getString("Pozice9d", "");
        break;
      case 0b1010:  //A
        sendSlovo_a = preferences.getString("PoziceAa", "");
        sendSlovo_b = preferences.getString("PoziceAb", "");
        sendSlovo_c = preferences.getString("PoziceAc", "");
        sendSlovo_d = preferences.getString("PoziceAd", "");
        break;
      case 0b0010:  //B
        sendSlovo_a = preferences.getString("PoziceBa", "");
        sendSlovo_b = preferences.getString("PoziceBb", "");
        sendSlovo_c = preferences.getString("PoziceBc", "");
        sendSlovo_d = preferences.getString("PoziceBd", "");
        break;
      case 0b1100:  //C
        sendSlovo_a = preferences.getString("PoziceCa", "");
        sendSlovo_b = preferences.getString("PoziceCb", "");
        sendSlovo_c = preferences.getString("PoziceCc", "");
        sendSlovo_d = preferences.getString("PoziceCd", "");
        break;
      case 0b0100:  //D
        sendSlovo_a = preferences.getString("PoziceDa", "");
        sendSlovo_b = preferences.getString("PoziceDb", "");
        sendSlovo_c = preferences.getString("PoziceDc", "");
        sendSlovo_d = preferences.getString("PoziceDd", "");
        break;
      case 0b1000:  //E
        sendSlovo_a = preferences.getString("PoziceEa", "");
        sendSlovo_b = preferences.getString("PoziceEb", "");
        sendSlovo_c = preferences.getString("PoziceEc", "");
        sendSlovo_d = preferences.getString("PoziceEd", "");
        break;
      case 0b0000:  //F
        sendSlovo_a = preferences.getString("PoziceFa", "");
        sendSlovo_b = preferences.getString("PoziceFb", "");
        sendSlovo_c = preferences.getString("PoziceFc", "");
        sendSlovo_d = preferences.getString("PoziceFd", "");
        break;
    }
    preferences.end();



    // Vytvoření pole obsahujícího slova
    String Sendslova[] = { sendSlovo_a, sendSlovo_b, sendSlovo_c, sendSlovo_d };

    // Smyčka pro a b c d
    for (int i = 0; i < 4; i++) {
      if (String(Sendslova[i]) == "hOdInY") {
        if (wifi_connected) {

          slovoHEX = stringToHex("zI " + getTime());
        } else {
          slovoHEX = stringToHex("zI WIFI!");
        }
      } else {
        slovoHEX = stringToHex(String(Sendslova[i]));
      }
      String HEXproSUM = String("7f") + slovoHEX + "0D";
      String CheckSUM = checksum8XOR(HEXproSUM);
      String hotovyHEX = HEXproSUM.substring(2) + CheckSUM;

      // Convert the String object to a C-string
      char hexString[hotovyHEX.length() + 1];
      hotovyHEX.toCharArray(hexString, sizeof(hexString));

      sendHexString(hexString);
    }









    led_state = !led_state;
    if (!wifi_connected) {
      //stav: není připojeno k wifi
      if (led_state) {
        setColor(0, 1, 1);  //červená
      } else {
        setColor(1, 1, 1);  //černá
      }
    } else {
      //stav: připojeno k wifi
      if (true) {  //nepodařilo se mi nějak udělat ověření komunikace NTP serveru
        //stav: NTP server komunikuje a jsou data o čase
        if (led_state) {
          setColor(0, 0, 0);  // Bílá (místo zelené, která slouží pro nahrávání dat - aby se nepletlo)
        } else {
          setColor(1, 1, 1);  //černá
        }
      } else {
        //stav: chyba NTP serveru
        if (led_state) {
          setColor(0, 0, 1);  // Žlutá
        } else {
          setColor(1, 1, 1);  //černá
        }
      }
    }
  }
  cas_delay++;


  // Pokud je připojen k WiFi, aktualizace času z NTP serveru a uloží se podmínka
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    wifi_connected = true;
  } else {
    wifi_connected = false;
  }

  // Obsluha webového serveru
  server.handleClient();
}



//funkce pro ibis::

void sendHexString(const char* hexString) {
  // Calculate the length of the hex string
  size_t len = strlen(hexString);

  // Loop through the hex string and convert each pair of hex digits to a byte
  for (size_t i = 0; i < len; i += 2) {
    char hexByte[3];
    hexByte[0] = hexString[i];
    hexByte[1] = hexString[i + 1];
    hexByte[2] = '\0';

    // Convert the hex byte to an integer
    byte b = (byte)strtol(hexByte, NULL, 16);

    // Send the byte via serial
    mySerial.write(b);
  }
}
// Function to convert string to hexadecimal string
String stringToHex(String str) {
  String hexKodSlova = "";
  for (int i = 0; i < str.length(); i++) {
    char hex[3];
    sprintf(hex, "%02X", str.charAt(i));
    hexKodSlova += hex;
  }
  return hexKodSlova;
}
// Function to compute checksum using XOR
String checksum8XOR(String hexString) {
  // Convert hex string to byte array
  int length = hexString.length() / 2;
  byte bytes[length];
  for (int i = 0; i < length; i++) {
    bytes[i] = strtol(hexString.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
  }

  // Initialize checksum
  byte checksum = 0x00;

  // Perform XOR across all bytes
  for (int i = 0; i < length; i++) {
    checksum ^= bytes[i];
  }

  // Convert resulting checksum to hexadecimal string
  char hexChecksum[3];
  sprintf(hexChecksum, "%02X", checksum);
  return String(hexChecksum);
}


//funkce pro získání času:
String getTime() {
  unsigned long epochTime = timeClient.getEpochTime();
  setTime(epochTime);

  int hoursOffset = isDST() ? 2 : 1;
  adjustTime(hoursOffset * SECS_PER_HOUR);

  int hours = hour();
  int minutes = minute();
  String timeString = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
  return timeString;
}
//funkce pro letní/zimní čas::
bool isDST() {
  // Determine if it is Daylight Saving Time (DST) in Central European Time zone
  int monthVal = month();
  int dayVal = day();
  int weekDay = weekday();  // Sunday is 1, Monday is 2, ..., Saturday is 7

  // DST starts at 2:00 on the last Sunday of March
  if (monthVal > 3 && monthVal < 10) {
    return true;
  }
  if (monthVal < 3 || monthVal > 10) {
    return false;
  }
  if (monthVal == 3) {
    int lastSunday = dayVal - weekDay + 1;
    return lastSunday >= 25 && (dayVal - lastSunday) >= 0 && (hour() >= 2);
  }
  if (monthVal == 10) {
    int lastSunday = dayVal - weekDay + 1;
    return lastSunday < 25 || (lastSunday == 25 && hour() < 2);
  }
  return false;
}

//funkce pro ovládání LED na ESP32
void setColor(bool red, bool green, bool blue) {
  digitalWrite(redPin, red);
  digitalWrite(greenPin, green);
  digitalWrite(bluePin, blue);
}

//funkce pro webserver::

// Funkce pro zobrazení webové stránky
void handleRoot() {
  String currentTime = wifi_connected ? getTime() : "Není připojení k WIFI";

String html = "<html><head><meta charset=\"UTF-8\"><style>"
                "table { width: 100%; border-collapse: collapse; }"
                "table, th, td { border: 1px solid black; }"
                "th, td { padding: 10px; text-align: left; }"
                "</style><script>"
                "function togglePassword() {"
                "  var x = document.getElementById('passwordInput');"
                "  if (x.type === 'password') {"
                "    x.type = 'text';"
                "  } else {"
                "    x.type = 'password';"
                "  }"
                "}"
                "</script></head><body><h1>Nastavení WIFI</h1>"
                + currentTime + "<form action=\"/save\" method=\"POST\">"
                              "ssid_sta: <input type=\"text\" name=\"ssid_sta\" value=\""
                + ssid_sta + "\"><br>"
                             "password_sta: <input type=\"password\" id=\"passwordInput\" name=\"password_sta\" value=\""
                + password_sta + "\"><button type=\"button\" onclick=\"togglePassword()\">Show/Hide</button><br>"
                                 "<input type=\"submit\" value=\"Uložit\">"
                                 "</form>"
                                 "<h1>Pozice přepínače a odesílané data</h1>"
                                 "<form action=\"/save_positions\" method=\"POST\">"
                                 "<table><tr><th>Pozice</th><th>a</th><th>b</th><th>c</th><th>d</th></tr>";

  // Definujeme pole s číslováním
  const char* positions[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };

  // Vygenerujeme vstupy pro formulář s hodnotami
  preferences.begin("my-app", false);
  for (int i = 0; i < 16; i++) {
    html += "<tr><td>Pozice " + String(positions[i]) + "</td>";
    html += "<td><input type=\"text\" name=\"Pozice" + String(positions[i]) + "a\" value=\"" + preferences.getString(("Pozice" + String(positions[i]) + "a").c_str(), "") + "\"></td>";
    html += "<td><input type=\"text\" name=\"Pozice" + String(positions[i]) + "b\" value=\"" + preferences.getString(("Pozice" + String(positions[i]) + "b").c_str(), "") + "\"></td>";
    html += "<td><input type=\"text\" name=\"Pozice" + String(positions[i]) + "c\" value=\"" + preferences.getString(("Pozice" + String(positions[i]) + "c").c_str(), "") + "\"></td>";
    html += "<td><input type=\"text\" name=\"Pozice" + String(positions[i]) + "d\" value=\"" + preferences.getString(("Pozice" + String(positions[i]) + "d").c_str(), "") + "\"></td></tr>";
  }
  preferences.end();

  html += "</table><p>Pro nastavení zobrazování hodin použij kód: <b>hOdInY</b></p><input type=\"submit\" value=\"Uložit\">";
  html += "</form></body></html>";



  server.send(200, "text/html", html);
}
void handleSavePositions() {
  preferences.begin("my-app", false);

  // Define the range for numeric positions
  int maxNumericPosition = 9;
  // Define the range for alphabetic positions
  char maxAlphabeticPosition = 'F';  // Include up to 'F'

  // Loop through numeric positions
  for (int i = 0; i <= maxNumericPosition; i++) {
    for (char subPosition = 'a'; subPosition <= 'd'; subPosition++) {
      String positionKey = "Pozice" + String(i) + subPosition;
      preferences.putString(positionKey.c_str(), server.arg(positionKey).c_str());
    }
  }

  // Loop through alphabetic positions
  for (char i = 'A'; i <= maxAlphabeticPosition; i++) {
    for (char subPosition = 'a'; subPosition <= 'd'; subPosition++) {
      String positionKey = "Pozice" + String(i) + subPosition;
      preferences.putString(positionKey.c_str(), server.arg(positionKey).c_str());
    }
  }

  preferences.end();
  server.send(200, "text/html", "<html><head><meta charset=\"UTF-8\"><meta http-equiv=\"refresh\" content=\"1; url=/\" /></head><body><h1>Hodnoty byly uloženy</h1></body></html>");
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
  String html = "<html><head><meta charset=\"UTF-8\"><meta http-equiv=\"refresh\" content=\"1; url=/\" /></head><body><h1>Změna údajů sítě WiFi úspěšná, provádím restart ESP32...</h1></body></html>";
  server.send(200, "text/html", html);
  delay(5000);    // Krátká prodleva před restartem
  esp_restart();  // Restartuj ESP32
}
