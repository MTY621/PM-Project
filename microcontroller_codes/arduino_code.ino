#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- CONFIGURATION ---
// Potentiometer input (ADC)
const int POT_PIN  = A0;
// Voltage divider input (ADC)
const int BATT_PIN = A1;
// MOSFET Gate (PWM - Timer 1)
const int MOTOR_PIN = 9;

// Calibration needed (adjust MAX_PWM until Speed 4 is 2400 RPM)
const int MAX_PWM = 200; 

// I2C LCD settings
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  
  // Start-up splash screen
  lcd.setCursor(0, 0);
  lcd.print("POV BASE v1.0");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM READY");
  delay(1500);
  lcd.clear();
}

void loop() {
  // 1. READ BATTERY VOLTAGE (ADC)
  // Voltage divider 10k/10k cuts voltage by half.
  // Formula: (ADC_Value * 5.0 / 1023.0) * 2 (multiplier for the divider)
  int battRaw = analogRead(BATT_PIN);
  float voltage = (battRaw * 5.0 / 1023.0) * 2.0;

  // 2. READ SPEED SELECTION (ADC)
  int potRaw = analogRead(POT_PIN);
  int speedStep = 0;
  int motorPWM = 0;

  // Logic for 4 equal segments of the 0-1023 range
  if (potRaw <= 255) {
    speedStep = 1;
    motorPWM = MAX_PWM * 0.25;
  } else if (potRaw <= 511) {
    speedStep = 2;
    motorPWM = MAX_PWM * 0.50;
  } else if (potRaw <= 767) {
    speedStep = 3;
    motorPWM = MAX_PWM * 0.75;
  } else {
    speedStep = 4;
    motorPWM = MAX_PWM;
  }

  // 3. CONTROL MOTOR (PWM)
  analogWrite(MOTOR_PIN, motorPWM);

  // 4. UPDATE DISPLAY (I2C)
  // Line 1: Battery Stats
  lcd.setCursor(0, 0);
  lcd.print("BATT: ");
  lcd.print(voltage);
  lcd.print("V    ");

  // Line 2: Speed Stats
  lcd.setCursor(0, 1);
  lcd.print("SPEED: ");
  lcd.print(speedStep);
  lcd.print(" [");
  // Visual progress bar for PWM
  int bars = map(motorPWM, 0, MAX_PWM, 0, 5);
  for(int i=0; i<5; i++) {
    if(i < bars) lcd.print("=");
    else lcd.print(" ");
  }
  lcd.print("]");

  delay(200);
}
