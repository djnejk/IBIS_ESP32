#include <HardwareSerial.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>


// Define the serial port
HardwareSerial mySerial(0);

// WiFi credentials
const char* ssid = "****";
const char* password = "****";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Time offset in seconds (for example for CET is 3600 seconds)
const long utcOffsetInSeconds = 3600;


void setup() {
  // Set up the serial communication with specified settings
  mySerial.begin(1200, SERIAL_7E2);  // 1200 baud rate, 7 data bits, even parity, 2 stop bits
  randomSeed(analogRead(0));

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
  timeClient.update();

  // Get the current epoch time from NTP
  unsigned long epochTime = timeClient.getEpochTime();
  
  // Convert epoch time to TimeLib time
  setTime(epochTime);
  
  // Calculate local time considering daylight saving time (DST)
  int hoursOffset = isDST() ? 2 : 1; // CET is UTC+1 and CEST is UTC+2
  adjustTime(hoursOffset * SECS_PER_HOUR);

  // Get the hours and minutes
  int hours = hour();
  int minutes = minute();

  // Format the time as hh:mm
  String timeString = (hours < 10 ? "0" : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);


  // No need to do anything in the loop
  String slovo = timeString; //buď náhodné (randomString) nebo vlastní ""
  String slovoHEX = stringToHex(slovo); 
  String HEXproSUM = String("7f") + "7A4920" + slovoHEX + "0D";
  String CheckSUM = checksum8XOR(HEXproSUM);
  String hotovyHEX = HEXproSUM.substring(2) + CheckSUM;

  // Convert the String object to a C-string
  char hexString[hotovyHEX.length() + 1];
  hotovyHEX.toCharArray(hexString, sizeof(hexString));

  sendHexString(hexString);
  delay(1000);
}

void sendHexString(const char *hexString) {
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
  int weekDay = weekday(); // Sunday is 1, Monday is 2, ..., Saturday is 7

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