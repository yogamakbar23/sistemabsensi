#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

String uidStr, lastInput = "";
unsigned long lastTime = 0;
bool refreshed = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
}

void loop() {
  readSerial();
  refreshScan();
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }
  readRFID();
  writeSerial();
  delay(500);

  rfid.PICC_HaltA();

}

void readRFID(){
  uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidStr += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    uidStr += String(rfid.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();
}

void printLCD(String str){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selamat Datang,");
  lcd.setCursor(0, 1);
  lcd.print(str);
}

void writeSerial(){
  Serial.println(uidStr);
  uidStr = "";
}

void readSerial(){
  if (Serial.available()>0) {
    String input = Serial.readStringUntil('#');
    input.trim();
    if(input!=""&&input!=lastInput){
      lastInput = input;
      printLCD(input);
    }
    lastTime = millis();
    refreshed = false;
    
  }
}

void refreshScan(){
   if (!refreshed && millis() - lastTime > 1000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan Kartu");
    lcd.setCursor(0, 1);
    lcd.print("untuk Absensi...");
    refreshed = true;
    lastInput = "";
  }
}
