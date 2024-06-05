#include <HardwareSerial.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

// Define the serial port
HardwareSerial mySerial(0);

// WiFi credentials
const char* ssid = "I'm Under Your Bed!";
const char* password = "****";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Time offset in seconds (for example for CET is 3600 seconds)
const long utcOffsetInSeconds = 3600;

const int Prep_1 = 2;
const int Prep_2 = 3;
const int Prep_4 = 4;
const int Prep_8 = 5;

// Struktura pro více hodnot
struct CaseValues {
  int combinedValue;
  const char* values[3];  // Max 3 hodnoty, můžete upravit podle potřeby
};

//hodnota nullptr musí být v řetězci na konci, označuje  v řetězci konec
CaseValues caseValues[] = {
  { 0b1111, { "zI 0", nullptr } },   //0
  { 0b0111, { "zI 1", nullptr } },   //1
  { 0b1011, { "zI 2", nullptr } },   //2
  { 0b0011, { "zI 3", nullptr } },   //3
  { 0b1101, { "zI 4", nullptr } },   //4
  { 0b0101, { "zI 5", nullptr } },   //5
  { 0b1001, { "zI 6", nullptr } },   //6
  { 0b0001, { "zI 7", nullptr } },   //7
  { 0b1110, { "zI 8", nullptr } },   //8
  { 0b0110, { "zI 9", nullptr } },   //9
  { 0b1010, { "zI A", nullptr } },   //A
  { 0b0010, { "zI B", nullptr } },   //B
  { 0b1100, { "zI C", nullptr } },   //C
  { 0b0100, { "zI D", nullptr } },   //D
  { 0b1000, { "zI E", nullptr } },   //E
  { 0b0000, { "hOdInY", nullptr } }  //F (interním kódem hOdInY se nastaví zobrazování hodin)
};

void setup() {
  // Set up the serial communication with specified settings
  mySerial.begin(1200, SERIAL_7E2);  // 1200 baud rate, 7 data bits, even parity, 2 stop bits
  randomSeed(analogRead(0));

  pinMode(Prep_1, INPUT_PULLUP);
  pinMode(Prep_2, INPUT_PULLUP);
  pinMode(Prep_4, INPUT_PULLUP);
  pinMode(Prep_8, INPUT_PULLUP);

  delay(10);

  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
  timeClient.begin();
}

void loop() {
  String slovoHEX = "";
  int value1 = digitalRead(Prep_1);
  int value2 = digitalRead(Prep_2);
  int value3 = digitalRead(Prep_4);
  int value4 = digitalRead(Prep_8);

  // Sluč vstupní hodnoty do jednoho binárního čísla
  int combinedValue = (value1 << 3) | (value2 << 2) | (value3 << 1) | value4;
  const char* slova[3] = { nullptr };

  // Vyhledání hodnot pro combinedValue
  for (int i = 0; i < sizeof(caseValues) / sizeof(caseValues[0]); ++i) {
    if (caseValues[i].combinedValue == combinedValue) {
      memcpy(slova, caseValues[i].values, sizeof(caseValues[i].values));
      break;
    }
  }



  // Iterace přes pole a výpis jednotlivých položek
  for (int i = 0; i < 3 && slova[i] != nullptr; ++i) {
    if (String(slova[i]) == "hOdInY") {
      timeClient.update();
      unsigned long epochTime = timeClient.getEpochTime();
      setTime(epochTime);

      int hoursOffset = isDST() ? 2 : 1;
      adjustTime(hoursOffset * SECS_PER_HOUR);

      int hours = hour();
      int minutes = minute();
      String timeString = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);

      slovoHEX = stringToHex("zI "+ timeString);
    } else {
      slovoHEX = stringToHex(String(slova[i]));
    }
    String HEXproSUM = String("7f") + slovoHEX + "0D";
    String CheckSUM = checksum8XOR(HEXproSUM);
    String hotovyHEX = HEXproSUM.substring(2) + CheckSUM;

    // Convert the String object to a C-string
    char hexString[hotovyHEX.length() + 1];
    hotovyHEX.toCharArray(hexString, sizeof(hexString));

    sendHexString(hexString);
  }

  delay(1000);
}

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
