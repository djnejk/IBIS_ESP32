#include <HardwareSerial.h>

// Define the serial port
HardwareSerial mySerial(0);

// Define the hex strings to be sent
const char* hexStrings[] = {
  "7A492031303A32300D58",
  "7A492031313A32310D58",
  "7A4920424146210D05"
};

// Define the number of hex strings
const int numHexStrings = 3;

void setup() {
  // Set up the serial communication with specified settings
  mySerial.begin(1200, SERIAL_7E2); // 1200 baud rate, 7 data bits, even parity, 2 stop bits
}

void loop() {
  // Loop through each hex string
  for (int i = 0; i < numHexStrings; i++) {
    sendHexString(hexStrings[i]);
    delay(1000); // Wait for 1 second
  }
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
    byte b = (byte) strtol(hexByte, NULL, 16);

    // Send the byte via serial
    mySerial.write(b);
  }
}
