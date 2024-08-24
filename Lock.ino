 
 /* Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Pin definitions
const int SS_PIN = 10;             // SPI SS pin for RFID module
const int RST_PIN = 9;             // Reset pin for RFID module
const int RED_LED_PIN = 2;         // Red LED indicator pin
const int GREEN_LED_PIN = 8;       // Green LED indicator pin
const int SERVO_PIN = 3;           // Servo control pin
const int LOCK_EXTENDED_PIN = 4;   // Limit switch pin for fully locked position
const int LOCK_RETRACTED_PIN = 5;  // Limit switch pin for fully unlocked position
const int LOCK_DISABLE_PIN = 7;    // To disable lock system when the machine is ON
const int IR_SENSOR_PIN = 6;       // Digital input pin for IR sensor module
const int BUZZER_PIN = A5;         // Buzzer pin for blinking along with LEDs
const int RELAY_PIN = A4;         // Relay pin for powering external alarm along with LEDs

// RFID and Servo objects
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Servo lockServo;                   // Create Servo instance

// Constants
const String CORRECT_ID = "C384B1FE";  // Authorized RFID tag ID
const int SERVO_LOCK_POSITION = 0;     // Servo angle for locking direction
const int SERVO_UNLOCK_POSITION = 180; // Servo angle for unlocking direction
const int SERVO_STOP = 90;             // Servo angle to stop movement
const unsigned long BLINK_INTERVAL = 300; // LED blink interval in milliseconds

// State variables
bool isLocked = false;              // Current lock state
unsigned long previousMillis = 0;   // Store last blink time

void setup() {
  initializePins();
  initializeSerialAndRFID();
  determineInitialLockState();
  Serial.print("Pin State ");
  Serial.println(digitalRead(LOCK_DISABLE_PIN));
  Serial.println("Place your RFID tag on the reader...");
}

void loop() {
  if (isNewCardPresent() && readCardSerial()) {
    String cardID = getCardID();
    processCard(cardID);
  }
}

// Initialize all pins
void initializePins() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LOCK_EXTENDED_PIN, INPUT_PULLUP);
  pinMode(LOCK_RETRACTED_PIN, INPUT_PULLUP);
  pinMode(LOCK_DISABLE_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  lockServo.attach(SERVO_PIN);
}

// Initialize Serial communication and RFID module
void initializeSerialAndRFID() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
}

// Determine and set the initial state of the lock
void determineInitialLockState() {
  if (isFullyLocked()) {
    isLocked = true;
    Serial.println("Initial state: Locked");
  } else if (isFullyUnlocked()) {
    isLocked = false;
    Serial.println("Initial state: Unlocked");
  } else {
    moveToUnlockedPosition();
  }
  lockServo.write(SERVO_STOP);
}

// Check if the lock is fully locked (both switches unpressed)
bool isFullyLocked() {
  return digitalRead(LOCK_EXTENDED_PIN) == HIGH && digitalRead(LOCK_RETRACTED_PIN) == HIGH;
}

// Check if the lock is fully unlocked (both switches pressed)
bool isFullyUnlocked() {
  return digitalRead(LOCK_EXTENDED_PIN) == LOW && digitalRead(LOCK_RETRACTED_PIN) == LOW;
}

// Move the lock to the unlocked position
void moveToUnlockedPosition() {
  Serial.println("Initial state ambiguous. Moving to unlocked position...");
  while (!isFullyUnlocked()) {
    lockServo.write(SERVO_UNLOCK_POSITION);
  }
  lockServo.write(SERVO_STOP);
  isLocked = false;
  Serial.println("Moved to unlocked position");
}

// Check if a new RFID card is present
bool isNewCardPresent() {
  return mfrc522.PICC_IsNewCardPresent();
}

// Try to read the RFID card serial
bool readCardSerial() {
  return mfrc522.PICC_ReadCardSerial();
}

// Get the ID of the scanned RFID card
String getCardID() {
  String cardID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardID += String(mfrc522.uid.uidByte[i], HEX);
  }
  cardID.toUpperCase();
  Serial.println("RFID tag ID: " + cardID);
  return cardID;
}

// Process the scanned card
void processCard(const String& cardID) {
  if (cardID == CORRECT_ID) {
    if (isLocked) {
      if (digitalRead(LOCK_DISABLE_PIN) == LOW) {
        unlock();
      } else {
        Serial.print("Unlock disabled! Key is still ON. pin State: ");
        Serial.println(digitalRead(LOCK_DISABLE_PIN));
        indicateLockingDisabled();
      }
    } else {
      if (!isObstacleDetected()) {
        if (digitalRead(LOCK_DISABLE_PIN) == LOW) {
          lock();
        } else {
          Serial.print("Lock disabled!!! Key is still ON. pin State ");
          Serial.println(digitalRead(LOCK_DISABLE_PIN));
          indicateLockingDisabled();
        }
      } else {
        Serial.println("Obstacle detected! Cannot lock.");
        indicateObstacleDetected();
      }
    }
  } else {
    indicateIncorrectCard();
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// Unlock the lock
void unlock() {
  Serial.println("Correct RFID tag detected! Unlocking...");
  while (!isFullyUnlocked()) {
    lockServo.write(SERVO_UNLOCK_POSITION);
    blinkLEDsAndBuzzer();
  }
  // Stop the buzzer after locking
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  lockServo.write(SERVO_STOP);
  isLocked = false;
  Serial.println("Unlocked!");
}

// Lock the lock
void lock() {
  Serial.println("Correct RFID tag detected! Locking...");
  while (!isFullyLocked()) {
    if (isObstacleDetected()) {
      Serial.println("Obstacle detected during locking! Stopping.");
      lockServo.write(SERVO_STOP);
      return;
    }
    lockServo.write(SERVO_LOCK_POSITION);
    blinkLEDsAndBuzzer();
  }
  lockServo.write(SERVO_STOP);
  isLocked = true;
  Serial.println("Locked!");

  // Stop the buzzer after locking
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);
}

// Blink the green LED and buzzer while locking
void blinkLEDsAndBuzzer() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= BLINK_INTERVAL) {
    previousMillis = currentMillis;
    toggleLEDsAndBuzzer();
  }
}

// Toggle LEDs and buzzer state
void toggleLEDsAndBuzzer() {
  bool ledState = digitalRead(GREEN_LED_PIN);
  digitalWrite(GREEN_LED_PIN, !ledState);
  digitalWrite(BUZZER_PIN, !ledState);
}

// Check if an obstacle is detected by the IR sensor
bool isObstacleDetected() {
  return digitalRead(IR_SENSOR_PIN) == LOW;
}

// Indicate locking disabled
void indicateLockingDisabled() {
  for (int i = 0; i < 3; i++) {
    blinkRedLEDAndBuzzer();
  }
}

// Indicate an incorrect card was scanned
void indicateIncorrectCard() {
  Serial.println("Incorrect RFID tag.");
  for (int i = 0; i < 10; i++) {
    blinkRedLEDAndBuzzer();
    digitalWrite(RELAY_PIN, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN, LOW);
  }
}

// Indicate an obstacle was detected
void indicateObstacleDetected() {
  Serial.println("Obstacle detected.");
  for (int i = 0; i < 4; i++) {
    blinkRedLEDAndBuzzer();
  }
}

// Blink red LED and buzzer with delay
void blinkRedLEDAndBuzzer() {
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BLINK_INTERVAL);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  delay(BLINK_INTERVAL);
}
