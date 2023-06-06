#include <Wire.h>
#include <Adafruit_PN532.h>
#include <Servo.h>

#define SERVO_PIN 9
#define PIR_PIN 2
#define PN532_SDA 10
#define PN532_SCL 11

Servo lockServo;
Adafruit_PN532 nfc(PN532_SDA, PN532_SCL);

void setup()
{
  Serial.begin(9600);
  lockServo.attach(SERVO_PIN);
  pinMode(PIR_PIN, INPUT);

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
    while (1);
  }
  nfc.SAMConfig();
  Serial.println("Waiting for RFID/NFC card...");
}

void loop()
{
  if (digitalRead(PIR_PIN) == HIGH)
  {
    Serial.println("Motion detected!");

    uint8_t success;
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    
    if (success)
    {
      Serial.println("RFID card detected!");

      if (checkRFID(uid, uidLength))
      {
        unlockDoor();
        delay(5000);
        lockDoor();
      }
      else
      {
        Serial.println("Unauthorized card!");
      }
    }
    else
    {
      Serial.println("Error reading RFID card!");
    }
  }

  delay(1000);
}

bool checkRFID(uint8_t *uid, uint8_t uidLength)
{
  // Disini kamu bisa menambahkan logika untuk memeriksa apakah UID kartu yang dideteksi
  // berada dalam daftar kartu yang diizinkan atau tidak.
  // Kamu bisa menggunakan database atau array untuk menyimpan daftar UID yang diizinkan.

  // Contoh sederhana yang memeriksa UID kartu
  uint8_t allowedCard[] = {0xAA, 0xBB, 0xCC, 0xDD};
  if (uidLength != sizeof(allowedCard))
    return false;
  for (uint8_t i = 0; i < uidLength; i++)
  {
    if (uid[i] != allowedCard[i])
      return false;
  }
  return true;
}

void unlockDoor()
{
  lockServo.write(90); // Membuka kunci pintu
  Serial.println("Door unlocked");
}

void lockDoor()
{
  lockServo.write(0); // Mengunci pintu
  Serial.println("Door locked");
}
