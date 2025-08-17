#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
// Motor and control pins
int m1 = 3;
int m2 = 4;
int m3 = 5;
int m4 = 6;
int pb = A2;
const int joystickXPin = A0;
const int joystickYPin = A1;
int md = 0; // 0 for remote control, 1 for joystick
int jxval, jyval;
int x;
// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 11, 10};
byte colPins[COLS] = {9, 8, 7, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int buzzer = A3;

// Variables for password input
int password[4];
int correctPassword[4] = {2, 3, 5, 6};
int passwordIndex = 0;

void setup() {
  Serial.begin(9600); // Initialize serial communication
  accel.begin(); // Initialize accelerometer
   
   
   
   lcd.init(); // Initialize LCD
  lcd.setCursor(4, 0);
  lcd.print("WELCOME:");
  lcd.setCursor(0, 1);
  lcd.print("PASSWORD DOOR LOCK");
  delay(2000);
  lcd.clear();

  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);
  pinMode(pb, INPUT_PULLUP);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Press # to start");
}

void loop() {
  char key = customKeypad.getKey();
  if (key == '#') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter password:");
    passwordIndex = 0;

    // Input password
    while (passwordIndex < 4) {
      key = customKeypad.getKey();
      if (key) {
        if (key >= '0' && key <= '9') {
          password[passwordIndex] = key - '0';
          lcd.setCursor(passwordIndex, 1);
          lcd.print('*');
          passwordIndex++;
        } else if (key == '*') {
          if (passwordIndex > 0) {
            passwordIndex--;
            lcd.setCursor(passwordIndex, 1);
            lcd.print(' ');
            lcd.setCursor(passwordIndex, 1);
          }
        }
      }
    }

    // Check password
    if (checkPassword()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password OK!");
      delay(1000);
      lcd.clear();
      mainControl();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong Password!");
      digitalWrite(buzzer, HIGH);
      delay(2000);
      digitalWrite(buzzer, LOW);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press # to start");
  }
}
void mainControl() {
  while (true)  
 {
    jxval = analogRead(joystickXPin);
    jyval = analogRead(joystickYPin);
    int pval = digitalRead(pb);
 sensors_event_t event; 
 accel.getEvent(&event);
 int xval=event.acceleration.x; 
 int yval=event.acceleration.y; 
    // Display readings
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("P:" + String(pval) + " X:" + String(jxval) + " Y:" + String(jyval));
    lcd.setCursor(0,1);
    lcd.print("X:"+String(xval) + " Y:"+String(yval));
    delay(200);
 joystickControl() ;
    // Alert with buzzer if pb is pressed
    if (pval == 0) {
      digitalWrite(buzzer, HIGH);
      send_sms(1);
    } else {
      digitalWrite(buzzer, LOW);
    }
 if(xval>2 || xval<-2 || yval>2 || yval<-2) {
      stopMotors();
      while (1);  // Freeze the program
    }
     if (Serial.available()) {
       x=Serial.read();
      Serial.print(x);
    if(x==3)
{
  md=0;
}
if(x==7)
{
  md=1;
}
     }
     if (md==0){
       if(x==3){ // Forward
      digitalWrite(m1, HIGH);
      digitalWrite(m2, LOW);
      digitalWrite(m3, LOW);
      digitalWrite(m4, HIGH);
       }
    if(x==4){ 
      digitalWrite(m1, LOW);
      digitalWrite(m2, HIGH);
      digitalWrite(m3, HIGH);
      digitalWrite(m4, LOW);
    }
    if(x==1){ 
      digitalWrite(m1, HIGH);
      digitalWrite(m2, LOW);
      digitalWrite(m3, HIGH);
      digitalWrite(m4, LOW);
    }
    if(x==2){ 
      digitalWrite(m1, LOW);
      digitalWrite(m2, HIGH);
      digitalWrite(m3, LOW);
      digitalWrite(m4, HIGH);
    }
    if(x==5){ 
       digitalWrite(m1, LOW);
      digitalWrite(m2, 0);
      digitalWrite(m3, 0);
      digitalWrite(m4, LOW);
    }
     }
//     if (md==1){
//      joystickControl() ;
//     }
     
  }
}

void joystickControl() {
  // Map joystick values to motor controls
  if (jxval > 1000) { // Forward
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
    digitalWrite(m3, HIGH);
    digitalWrite(m4, LOW);
  } else if (jxval < 100) { // Backward
    digitalWrite(m1, LOW);
    digitalWrite(m2, HIGH);
    digitalWrite(m3, LOW);
    digitalWrite(m4, HIGH);
  } else if (jyval > 950) { // Right
    digitalWrite(m1, LOW);
    digitalWrite(m2, HIGH);
    digitalWrite(m3, HIGH);
    digitalWrite(m4, LOW);
  } else if (jyval < 100) { // Left
    digitalWrite(m1, HIGH);
    digitalWrite(m2, LOW);
    digitalWrite(m3, LOW);
    digitalWrite(m4, HIGH);
  } else {
    stopMotors(); // Stop if joystick is centered
  }
}
//
//void remoteControl(char val) {
//  switch (val) {
//    case '1': // Forward
//      digitalWrite(m1, HIGH);
//      digitalWrite(m2, LOW);
//      digitalWrite(m3, HIGH);
//      digitalWrite(m4, LOW);
//      break;
//    case '2': // Reverse
//      digitalWrite(m1, LOW);
//      digitalWrite(m2, HIGH);
//      digitalWrite(m3, LOW);
//      digitalWrite(m4, HIGH);
//      break;
//    case '3': // Left
//      digitalWrite(m1, HIGH);
//      digitalWrite(m2, LOW);
//      digitalWrite(m3, LOW);
//      digitalWrite(m4, HIGH);
//      break;
//    case '4': // Right
//      digitalWrite(m1, LOW);
//      digitalWrite(m2, HIGH);
//      digitalWrite(m3, HIGH);
//      digitalWrite(m4, LOW);
//      break;
//    case '5': // Stop
//      stopMotors();
//      break;
//    default:
//      Serial.println("Invalid command"); // Debug message
//      break;
//  }
//}

void stopMotors() {
  digitalWrite(m1, LOW);
  digitalWrite(m2, LOW);
  digitalWrite(m3, LOW);
  digitalWrite(m4, LOW);
}

bool checkPassword() {
  for (int i = 0; i < 4; i++) {
    if (password[i] != correctPassword[i]) {
      return false;
    }
  }
  return true;
}

void send_sms(int k) {
  Serial.println("Sending SMS...");
  Serial.println("AT");
  delay(1000);
  Serial.println("ATE0");
  delay(1000);
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.print("AT+CMGS=\"9391295551\"\r\n"); // Replace with your mobile number
  delay(1000);
  if (k == 1) {
    Serial.print("I AM IN DANGER REACH ME SOON");
  }
  delay(500);
  Serial.print(char(26));
  delay(2000);
}