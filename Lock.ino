#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Define pins for RFID
#define SS_PIN 10
#define RST_PIN 9
int LED = 2;
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Define the Servo object
Servo myServo;

const String correctID = "C384B1FE"; // Your correct RFID tag ID
const int servoPin = 3; // Change to the PWM pin you're using
const int lockExtended = 4;   // Pin for the limit switch at the lock position
const int lockRetracted = 5; // Pin for the limit switch at the unlock position
bool isLocked = false;  // Track the current state of the lock

void setup() {
  Serial.begin(9600);  // Initialize serial communications
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  myServo.attach(servoPin); // Attach servo motor to pin 3
  //myServo.write(0);
  pinMode(LED, OUTPUT);      // Set the LED pin as output
  pinMode(lockExtended, INPUT_PULLUP);    // Set the lock limit switch pin as input
  pinMode(lockRetracted, INPUT_PULLUP);  // Set the unlock limit switch pin as input
  Serial.println("Place your RFID tag on the reader...");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump UID
  String readID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    readID += String(mfrc522.uid.uidByte[i], HEX);
  }

  readID.toUpperCase(); // Convert to uppercase for comparison
  Serial.print("RFID tag ID: ");
  Serial.println(readID);

  // Compare the read ID with the correct ID
  if (readID == correctID && isLocked) {
    Serial.println("Correct RFID tag detected! Unlocking...");
    digitalWrite(LED, HIGH);

    while (digitalRead(lockRetracted) == HIGH) {
      // until the unlock limit switch is triggered, retract the lock
      myServo.write(180); // Start rotating clockwise to unlock
    }
    myServo.write(90); // Stop servo (neutral position)
    isLocked = false;  // Update lock state
    Serial.println("Unocked!");
    digitalWrite(LED, LOW);


  }else if(readID == correctID && !isLocked){
    Serial.println("Correct RFID tag detected! Locking...");
    digitalWrite(LED, HIGH);
    while (digitalRead(lockExtended) == HIGH) {
      // Until the lock limit switch is triggered, extend the lock
      myServo.write(0);  // Start rotating anti-clockwise to lock
    }
    myServo.write(90); // Stop servo (neutral position)
    isLocked = true;   // Update lock state
    Serial.println("Unlocked!");
    digitalWrite(LED, LOW);


  } else {
    Serial.println("Incorrect RFID tag.");
    digitalWrite(LED, HIGH);
    delay(300);
    digitalWrite(LED, LOW);
    delay(300);
    digitalWrite(LED, HIGH);
    delay(400);
    digitalWrite(LED, LOW);
  }

  // Halt PICC (Anti-collision loop) 
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();      // Stop encryption on the PCD (reader)
  delay(1000); // Wait a second for easier reading
}
