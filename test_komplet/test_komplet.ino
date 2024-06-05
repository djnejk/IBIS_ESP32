#include <HardwareSerial.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>


// Define the serial port
HardwareSerial mySerial(0);

// WiFi credentials
const char* ssid = "I'm Under Your Bed!";
const char* password = "***";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Time offset in seconds (for example for CET is 3600 seconds)
const long utcOffsetInSeconds = 3600;

const int Prep_1 = 2;
const int Prep_2 = 3;
const int Prep_4 = 4;
const int Prep_8 = 5;


void setup() {
  // Set up the serial communication with specified settings
  mySerial.begin(1200, SERIAL_7E2);  // 1200 baud rate, 7 data bits, even parity, 2 stop bits
  randomSeed(analogRead(0));

  pinMode(Prep_1, INPUT_PULLUP);
  pinMode(Prep_2, INPUT_PULLUP);
  pinMode(Prep_4, INPUT_PULLUP);
  pinMode(Prep_8, INPUT_PULLUP);
  /*Pro pullup
* Q 1 2 4 8
* 0 1 1 1 1
* 1 0 1 1 1
* 2 1 0 1 1
* 3 0 0 1 1
* 4 1 1 0 1
* 5 0 1 0 1
* 6 1 0 0 1
* 7 0 0 0 1
* 8 1 1 1 0
* 9 0 1 1 0
* A 1 0 1 0
* B 0 0 1 0
* C 1 1 0 0 
* D 0 1 0 0
* E 1 0 0 0
* F 0 0 0 0
*/
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
  // Vytvoření pole řetězců
  String slova[] = { "zI ahoj", "L001", "ahoj, jak se máš?", "ESP32", "Arduino Nano" };

  String slovo = "";

  int value1 = digitalRead(Prep_1);
  int value2 = digitalRead(Prep_2);
  int value3 = digitalRead(Prep_4);
  int value4 = digitalRead(Prep_8);

  // Sluč vstupní hodnoty do jednoho binárního číslaz
  int combinedValue = (value1 << 3) | (value2 << 2) | (value3 << 1) | value4;
  switch (combinedValue) {
    case 0b1111:  //0
      slovo = "zI 0";
      break;
    case 0b0111:  //1
      slovo = "zI 1";
      break;
    case 0b1011:  //2
      slovo = "zI 2";
      break;
    case 0b0011:  //3
      slovo = "zI 3";
      break;
    case 0b1101:  //4
      slovo = "zI 4";
      break;
    case 0b0101:  //5
      slovo = "zI 5";
      break;
    case 0b1001:  //6
      slovo = "zI 6";
      break;
    case 0b0001:  //7
      slovo = "zI 7";
      break;
    case 0b1110:  //8
      slovo = "zI 8";
      break;
    case 0b0110:  //9
      slovo = "zI 9";
      break;
    case 0b1010:  //A
      slovo = "zI A";
      break;
    case 0b0010:  //B
      slovo = "zI B";
      break;
    case 0b1100:  //C
      slovo = "zI C";
      break;
    case 0b0100:  //D
      slovo = "zI D";
      break;
    case 0b1000:  //E
      slovo = "zI E";
      break;
    case 0b0000:  //F
      timeClient.update();

      // Get the current epoch time from NTP
      unsigned long epochTime = timeClient.getEpochTime();

      // Convert epoch time to TimeLib time
      setTime(epochTime);

      // Calculate local time considering daylight saving time (DST)
      int hoursOffset = isDST() ? 2 : 1;  // CET is UTC+1 and CEST is UTC+2
      adjustTime(hoursOffset * SECS_PER_HOUR);

      // Get the hours and minutes
      int hours = hour();
      int minutes = minute();

      // Format the time as hh:mm
      String timeString = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);


      // No need to do anything in the loop
      slovo = "zI" + timeString;
      break;
  }




  // Zjištění délky pole
  int listSize = sizeof(slova) / sizeof(slova[0]);

  // Iterace přes pole a výpis jednotlivých položek
  for (int i = 0; i < listSize; i++) {
    String slovoHEX = stringToHex(slova);
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