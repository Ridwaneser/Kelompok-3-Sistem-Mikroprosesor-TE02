#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define Buzzer_PIN 6

MFRC522 mfrc522(SS_PIN, RST_PIN); // Inisialisasi MFRC522
Servo myservo;

String UID = "f3112da8";
int ledPinPIR = 4; // Pin LED untuk sensor PIR
int ledPinRFID = 5; // Pin LED untuk sensor RFID

int pirState = LOW; // State awal sensor PIR
bool isRFIDEnabled = false; // Menandakan apakah sensor RFID diaktifkan

void setup() {
  pinMode(ledPinPIR, OUTPUT);
  pinMode(ledPinRFID, OUTPUT);
  pinMode(Buzzer_PIN, OUTPUT);

  // Inisialisasi Serial
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  myservo.attach(3);
  myservo.write(0);

  Serial.println("Waiting for RFID card...");
}

void loop() {
  // PIR Sensor
  int pirValue = digitalRead(2); // Sensor PIR terhubung ke pin 2

  if (pirValue == HIGH && pirState == LOW) {
    digitalWrite(ledPinPIR, HIGH);
    pirState = HIGH;
    isRFIDEnabled = true;
    Serial.println("RFID Dinyalakan");
  } else if (pirValue == LOW && pirState == HIGH) {
    digitalWrite(ledPinPIR, LOW);
    pirState = LOW;
    isRFIDEnabled = false;
    Serial.println("RFID Dimatikan");
  }

  // RFID Sensor
  if (isRFIDEnabled && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Membaca UID kartu
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      uid.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    Serial.print("Detected RFID card with UID: ");
    Serial.println(uid);
    uid.trim();

    // Periksa UID kartu
    if (uid == UID) {
      Serial.println("Kartu diterima");
      digitalWrite(ledPinRFID, HIGH);
      digitalWrite(Buzzer_PIN, HIGH);
      delay(500);
      digitalWrite(ledPinRFID, LOW);
      digitalWrite(Buzzer_PIN, LOW);
      delay(500);
      digitalWrite(ledPinRFID, HIGH);
      digitalWrite(Buzzer_PIN, HIGH);
      delay(500);
      digitalWrite(Buzzer_PIN, LOW);
      myservo.write(180);
      delay(3000);
      myservo.write(0);
      digitalWrite(ledPinRFID, LOW);
    } else {
      Serial.println("Kartu ditolak");
      digitalWrite(ledPinRFID, HIGH);
      digitalWrite(Buzzer_PIN, HIGH);
      delay(3000);
      digitalWrite(ledPinRFID, LOW);
      digitalWrite(Buzzer_PIN, LOW);
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}