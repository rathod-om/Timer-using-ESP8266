#include <Countimer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
// const int ledPin = D8;
const int bt_set = D4;
const int bt_up = D5;
const int bt_down = D6;
const int bt_start = D7;
const int relay = D0;
const int buzzer = D3;

// Timer
Countimer tdown;

// Time variables
int time_s = 0;
int time_m = 0;
int time_h = 0;

// State variables
int set = 0;
int flag1 = 0, flag2 = 0;

void setup() {
  // Pin modes
  pinMode(bt_set, INPUT_PULLUP);
  pinMode(bt_up, INPUT_PULLUP);
  pinMode(bt_down, INPUT_PULLUP);
  pinMode(bt_start, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  // pinMode(ledPin, OUTPUT);

  // Ensure the buzzer is off by default
  // digitalWrite(buzzer, LOW);
  digitalWrite(relay,HIGH);

  // LCD initialization
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Hello New");
  lcd.setCursor(2, 1);
  lcd.print("Aabhar jova Badal");
  delay(1000);
  lcd.clear();

  // Timer setup
  tdown.setInterval(print_time, 1000); // Call print_time every 1000ms (1 second)

  // Read initial time from EEPROM
  EEPROM.begin(512); // Initialize EEPROM
  eeprom_read();
}

void print_time() {
  if (flag2 == 1) {
    if (time_s > 0) {
      time_s--;
    } else if (time_m > 0) {
      time_s = 59;
      time_m--;
    } else if (time_h > 0) {
      time_s = 59;
      time_m = 59;
      time_h--;
    } else {
      // Timer finished
      time_s = 0;
      time_m = 0;
      time_h = 0;
      flag2 = 0;
      tdown.stop();
      digitalWrite(relay, HIGH);
      buzz();
    }
  }
}

void buzz() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
    delay(200);
  }
}

void loop() {
  tdown.run();

  if (digitalRead(bt_set) == LOW) {
    if (flag1 == 0) {
      set = (set + 1) % 4;
      flag1 = 1;
      delay(200);
    }
  } else {
    flag1 = 0;
  }

  if (digitalRead(bt_up) == LOW) {
    if (set == 1) time_s = (time_s + 1) % 60;
    if (set == 2) time_m = (time_m + 1) % 60;
    if (set == 3) time_h = (time_h + 1) % 100;
    eeprom_write();
    delay(200);
  }

  if (digitalRead(bt_down) == LOW) {
    if (set == 1) time_s = (time_s > 0) ? time_s - 1 : 59;
    if (set == 2) time_m = (time_m > 0) ? time_m - 1 : 59;
    if (set == 3) time_h = (time_h > 0) ? time_h - 1 : 99;
    eeprom_write();
    delay(200);
  }

  if (digitalRead(bt_start) == LOW) {
    flag2 = 1;
    eeprom_read();
    digitalWrite(relay, LOW);
    tdown.restart();
    tdown.start();
  }
  // digitalWrite(relay, HIGH);
  update_lcd();
  delay(10);
}

void update_lcd() {
  lcd.setCursor(0, 0);
  switch (set) {
    case 0: lcd.print("      Timer     "); break;
    case 1: lcd.print("  Set Timer SS  "); break;
    case 2: lcd.print("  Set Timer MM  "); break;
    case 3: lcd.print("  Set Timer HH  "); break;
  }

  lcd.setCursor(4, 1);
  if (time_h < 10) lcd.print("0");
  lcd.print(time_h);
  lcd.print(":");
  if (time_m < 10) lcd.print("0");
  lcd.print(time_m);
  lcd.print(":");
  if (time_s < 10) lcd.print("0");
  lcd.print(time_s);
  lcd.print("   ");
}

void eeprom_write() {
  EEPROM.write(0, time_s);
  EEPROM.write(1, time_m);
  EEPROM.write(2, time_h);
  EEPROM.commit(); // Ensure data is written
}

void eeprom_read() {
  time_s = EEPROM.read(0);
  time_m = EEPROM.read(1);
  time_h = EEPROM.read(2);
}