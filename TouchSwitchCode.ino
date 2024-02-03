#include <TouchScreen.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Arduino.h"
#include <SoftwareSerial.h>
#include <time.h>
#include <FirebaseESP32.h>

const char* ssid = "AZIZA CHATRINIBAS";
const char* password = "passwordlagena5";

#define FIREBASE_HOST "https://smart-switch-board-348dd-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyDn1I3OrFr9_R4rJ2KQ06-Ht1W7Ud701_4"
FirebaseData firebaseData;

hd44780_I2Cexp lcd;
const int lcdColumns = 16;
const int lcdRows = 2;

bool ledStatus;

#define THRESHOLD 42    // Adjust this value to set the touch sensitivity
#define TOUCH_PIN_1 T0  // Touch sensor 1 connected to T0
#define TOUCH_PIN_2 T4  // Touch sensor 2 connected to T4
#define TOUCH_PIN_3 T5  // Touch sensor 3 connected to T5
#define TOUCH_PIN_4 T3  // Touch sensor 4 connected to T3
#define LED_PIN_1 25    // LED 1 connected to pin 6
#define LED_PIN_2 26    // LED 2 connected to pin 7
#define LED_PIN_3 27    // LED 3 connected to pin 8
#define LED_PIN_4 14    // LED 4 connected to pin 9

bool switch1 = false;
bool switch2 = false;
bool switch3 = false;
bool switch4 = false;

double LastPrintedTime = millis();
double lastReadTime = millis();

void setup() {
  // Serial.begin(9600);
  Serial.begin(115200);

  analogReadResolution(12);

  pinMode(TOUCH_PIN_1, INPUT);
  pinMode(TOUCH_PIN_2, INPUT);
  pinMode(TOUCH_PIN_3, INPUT);
  pinMode(TOUCH_PIN_4, INPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);

  lcd.begin(lcdColumns, lcdRows);
  Wire.begin();
  lcd.clear();
  lcd.print("  Smart Switch");
  delay(1000);
  lcd.clear();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Connecting WiFi>");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SWS> ");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  checkSwitches();
  readData();
  updateSwitches();
  printInDisplay();
}

void checkSwitches() {
  // Read the touch sensor values
  int touchValue1 = touchRead(TOUCH_PIN_1);
  int touchValue2 = touchRead(TOUCH_PIN_2);
  int touchValue3 = touchRead(TOUCH_PIN_3);
  int touchValue4 = touchRead(TOUCH_PIN_4);

  if (touchValue1 < THRESHOLD) {
    switch1 = !switch1;  // Toggle the switch state
    updateSwitches();
    Serial.println("pressed ");
    while (touchValue1 < THRESHOLD) {
      touchValue1 = touchRead(TOUCH_PIN_1);
    }
    delay(100);
    Firebase.setBool(firebaseData, "/Switches/S1", switch1);
  }

  if (touchValue2 < THRESHOLD) {
    switch2 = !switch2;  // Toggle the switch state
    updateSwitches();
    Serial.println("pressed");
    while (touchValue2 < THRESHOLD) {
      touchValue2 = touchRead(TOUCH_PIN_2);
    }
    delay(100);
    Firebase.setBool(firebaseData, "/Switches/S2", switch2);
  }

  if (touchValue3 < THRESHOLD) {
    switch3 = !switch3;  // Toggle the switch state
    updateSwitches();
    Serial.println("pressed");
    while (touchValue3 < THRESHOLD) {
      touchValue3 = touchRead(TOUCH_PIN_3);
    }
    delay(100);
    Firebase.setBool(firebaseData, "/Switches/S3", switch3);
  }

  if (touchValue4 < THRESHOLD) {
    switch4 = !switch4;  // Toggle the switch state
    updateSwitches();
    Serial.println("pressed");
    while (touchValue4 < THRESHOLD) {
      touchValue4 = touchRead(TOUCH_PIN_4);
    }
    delay(100);
    Firebase.setBool(firebaseData, "/Switches/S4", switch4);
  }
}

void updateSwitches() {
  digitalWrite(LED_PIN_1, switch1 ? HIGH : LOW);  // Turn on/off LED1 based on switch1 state
  digitalWrite(LED_PIN_2, switch2 ? HIGH : LOW);  // Turn on/off LED2 based on switch2 state
  digitalWrite(LED_PIN_3, switch3 ? HIGH : LOW);  // Turn on/off LED3 based on switch3 state
  digitalWrite(LED_PIN_4, switch4 ? HIGH : LOW);  // Turn on/off LED4 based on switch4 state
}

void printInDisplay() {
  if (millis() - LastPrintedTime > 100) {
    Serial.print("Switch1 ");
    Serial.println(switch1);

    Serial.print("Switch2 ");
    Serial.println(switch2);

    Serial.print("Switch3 ");
    Serial.println(switch3);

    Serial.print("Switch4 ");
    Serial.println(switch4);

    Serial.println("   ");

    if (switch1) {
      lcd.setCursor(5, 0);
      lcd.print("S1");
    } else {
      lcd.setCursor(5, 0);
      lcd.print("__");
    }
    if (switch2) {
      lcd.setCursor(8, 0);
      lcd.print("S2");
    } else {
      lcd.setCursor(8, 0);
      lcd.print("__");
    }
    if (switch3) {
      lcd.setCursor(11, 0);
      lcd.print("S3");
    } else {
      lcd.setCursor(11, 0);
      lcd.print("__");
    }
    if (switch4) {
      lcd.setCursor(14, 0);
      lcd.print("S4");
    } else {
      lcd.setCursor(14, 0);
      lcd.print("__");
    }
    LastPrintedTime = millis();
  }
}

void readData() {
  if (millis() - lastReadTime > 500) {
    lastReadTime = millis();
    if (Firebase.getBool(firebaseData, "/Switches/S1")) {
      if (firebaseData.boolData() != switch1) {
        switch1 = firebaseData.boolData();
      }
    }
    if (Firebase.getBool(firebaseData, "/Switches/S2")) {
      if (firebaseData.boolData() != switch2) {
        switch2 = firebaseData.boolData();
      }
    }
    if (Firebase.getBool(firebaseData, "/Switches/S3")) {
      if (firebaseData.boolData() != switch3) {
        switch3 = firebaseData.boolData();
      }
    }
    if (Firebase.getBool(firebaseData, "/Switches/S4")) {
      if (firebaseData.boolData() != switch4) {
        switch4 = firebaseData.boolData();
      }
    }
  }
}