#include <SPI.h> 
#include <MFRC522.h>  // Library untuk RFID
#include <Servo.h>    // Library untuk Servo

#define SS_PIN 10     // Mendefinisikan PIN 10 Arduino Sebagai SDA
#define RST_PIN 9     // Mendefinisikan PIN 9 Arduino Sebagai Reset
#define Buzzer_PIN 6  // Mendefinisikan PIN 6 Arduino Sebagai Input ke Buzzer

MFRC522 mfrc522(SS_PIN, RST_PIN); // Inisialisasi MFRC522
Servo myservo;                    // Inisialisasi Servo

String UID = "f3ae61a6";  // UID valid
int ledPinPIR = 4;        // Pin LED untuk sensor PIR
int ledPinRFID = 5;       // Pin LED untuk sensor RFID

int pirState = LOW; // State awal sensor PIR
bool isRFIDEnabled = false; // Menandakan apakah sensor RFID diaktifkan

void setup() {
  pinMode(ledPinPIR, OUTPUT);   // Mengataur PIN ledPinPIR sebagai output
  pinMode(ledPinRFID, OUTPUT);  // Mengataur PIN ledPinRFID sebagai output
  pinMode(Buzzer_PIN, OUTPUT);  // Mengataur PIN Buzzer_PIN sebagai output

  // Inisialisasi Serial
  Serial.begin(9600);   // Inisialisasi komunikasi serial
  SPI.begin();          // Inisialisasi komunikasi SPI
  mfrc522.PCD_Init();   // Inisialisasi MFRC522
  myservo.attach(3);    // Posisi Pin Servo
  myservo.write(0);     // Posisi Awal Servo

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
      Serial.println("Halo Selamat Datang");
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
      delay(1000);
      digitalWrite(ledPinRFID, LOW);
      digitalWrite(Buzzer_PIN, LOW);
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}