#define MAX_DATA_SIZE 2048 // Maximum size for received data array

char receivedAscii[MAX_DATA_SIZE]; // Array to store received ASCII characters
uint8_t dataIndex = 0; // Index to keep track of the position in the receivedAscii array

const uint8_t IDENTIFICATION_MESSAGE[] = {0x2F, 0x4C, 0x47, 0x5A, 0x34, 0x5A, 0x4D, 0x46, 0x31, 0x30, 0x30, 0x41, 0x43, 0x2E, 0x4D, 0x32, 0x39, 0x0D, 0x0A};
const size_t IDENTIFICATION_MESSAGE_LEN = sizeof(IDENTIFICATION_MESSAGE) / sizeof(IDENTIFICATION_MESSAGE[0]);
const uint8_t REQUEST_DATA[] = {0x06, 0x30, 0x34, 0x30, 0x0D, 0x0A};
const size_t REQUEST_DATA_LEN = sizeof(REQUEST_DATA) / sizeof(REQUEST_DATA[0]);

void setup() {
  Serial.begin(9600); // Start serial communication on Serial (TX0/RX0)
  Serial2.begin(300); // Start serial communication on Serial2 (TX2/RX2)
}

void loop() {
  Serial2.print("/?!"); // Sending "/?!" through Serial2
  Serial2.print("\r\n"); // Sending carriage return and newline together through Serial2
  Serial2.flush();  // Wait for transmission to complete
  delay(1000);  
  // Check if there's incoming data on Serial2 (RX2)
  while (Serial2.available() > 0) {
    char receivedChar = Serial2.read(); // Read a character from Serial2
    // Store received character in the array if there is space
    if (dataIndex < MAX_DATA_SIZE - 1) {
      receivedAscii[dataIndex] = receivedChar;
      dataIndex++;
    }
  }

  // Null-terminate the received data array
  receivedAscii[dataIndex] = '\0';

  //Serial.println(dataIndex);
  //Serial.println(IDENTIFICATION_MESSAGE_LEN);
  
  if (dataIndex == IDENTIFICATION_MESSAGE_LEN) {
    bool match = true;
    for (size_t i = 0; i < IDENTIFICATION_MESSAGE_LEN; i++) {
      if (receivedAscii[i] != (char)IDENTIFICATION_MESSAGE[i]) {
        match = false;
        break;
      }
    }
    
    // Reset index for the next data reception
    // Move this line outside the if block
    dataIndex = 0;
    // Print the received ASCII array on Serial (TX0/RX0)
    if (match) {
      Serial.print("Received ASCII: ");
      Serial.println(receivedAscii);
      //Request data now
      Serial2.write(REQUEST_DATA, REQUEST_DATA_LEN);
      Serial2.flush();  // Wait for transmission to complete
      Serial.println("Requested Data Now");
      delay(6000); // Adjust this delay as needed to allow time for the response
      while (Serial2.available() > 0) {
          char receivedChar = Serial2.read(); // Read a character from Serial2
          // Store received character in the array if there is space
          if (dataIndex < MAX_DATA_SIZE - 1) {
            receivedAscii[dataIndex] = receivedChar;
            dataIndex++;
          }
    }
    Serial.println("Recieved Data:");
    
    Serial.print(receivedAscii);
  }
  } else {
    // Reset index when condition not met
    dataIndex = 0;
  }
  delay(1000); // Delay 10 seconds before sending the command again
}
