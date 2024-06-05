#include <HardwareSerial.h>
#include <Preferences.h>

Preferences preferences;
// Define the serial port
HardwareSerial mySerial(0);
String myString = "";
void setup() {
  // Set up the serial communication with specified settings
  mySerial.begin(1200, SERIAL_7E2);  // 1200 baud rate, 7 data bits, even parity, 2 stop bits
  randomSeed(analogRead(0));

  // Inicializace Preferences s názvem prostoru "my-app"
  preferences.begin("my-app", false);

  // // Uložení řetězce
  // preferences.putString("my-string", "Ahoj, svět!");
  // Načtení řetězce
  myString = preferences.getString("my-string", "default");

  // Zobrazení načteného řetězce v seriovém monitoru
  Serial.println(myString);

  // Ukončení práce s Preferences
  preferences.end();
}

void loop() {
  // Délka řetězce
  int length = 5;

  // Vytvoříme prázdný řetězec pro náhodné znaky
  char randomString[length + 1];  // +1 pro ukončovací znak '\0'

  // Generování náhodných znaků
  for (int i = 0; i < length; i++) {
    randomString[i] = char(random(32, 127));  // ASCII znaky od 32 do 126
  }

  // Ukončovací znak pro řetězec
  randomString[length] = '\0';


  // No need to do anything in the loop
  String slovo = myString;  //buď náhodné (randomString) nebo vlastní ""
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
