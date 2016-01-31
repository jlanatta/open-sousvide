#define REQUIRESALARMS true

#include <Adafruit_GFX.h>
#include <Adafruit_ssd1306syp.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define SDA_PIN 8
#define SCL_PIN 9
#define PIN_KEYBOARD_UP 3
#define PIN_KEYBOARD_DOWN 5
#define PIN_KEYBOARD_PUMP 6
#define PIN_KEYBOARD_HEATER 7
#define PIN_HEATER A1
#define PIN_PUMP A0

Adafruit_ssd1306syp display(SDA_PIN, SCL_PIN);
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

float currentTemp;
float targetTemp = 55;

bool isPumpOn = false;
bool heaterOn = false;
bool isHeaterOn = false;
bool displayNeedsToBeUpdated = true;
bool outputNeedsToBeUpdated = true;

void setup()
{
  Serial.begin(9600);
  sensors.begin();
  display.initialize();
  pinMode(PIN_KEYBOARD_UP, INPUT_PULLUP);
  pinMode(PIN_KEYBOARD_DOWN, INPUT_PULLUP);
  pinMode(PIN_KEYBOARD_PUMP, INPUT_PULLUP);
  pinMode(PIN_KEYBOARD_HEATER, INPUT_PULLUP);
  pinMode(PIN_HEATER, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
}

void loop() {
  readKeyboard();
  updateCurrentTemp();
  updateHeaterAndPump();
  updateDisplay();
}

void readKeyboard() {
  int pinUp = digitalRead(PIN_KEYBOARD_UP);
  int pinDown = digitalRead(PIN_KEYBOARD_DOWN);
  int pinPump = digitalRead(PIN_KEYBOARD_PUMP);
  int pinHeater = digitalRead(PIN_KEYBOARD_HEATER);
  
  if (pinUp == LOW) {
    targetTemp ++;
    displayNeedsToBeUpdated = true;
    outputNeedsToBeUpdated = true;
  }
  if (pinDown == LOW) {
    targetTemp --;
    displayNeedsToBeUpdated = true;
    outputNeedsToBeUpdated = true;
  }  
  if (pinPump == LOW) {
    isPumpOn = !isPumpOn;
    displayNeedsToBeUpdated = true;
    outputNeedsToBeUpdated = true;
  }
  if (pinHeater == LOW) {
    heaterOn = !heaterOn;
    displayNeedsToBeUpdated = true;
    outputNeedsToBeUpdated = true;
  }
}

int targetTempLastUpdated = 0;

void updateCurrentTemp() {
  if (millis() - targetTempLastUpdated > 1000) {
    sensors.requestTemperaturesByIndex(0); // Send the command to get temperatures
    currentTemp = sensors.getTempCByIndex(0);
    displayNeedsToBeUpdated = true;
    outputNeedsToBeUpdated = true;
    targetTempLastUpdated = millis();
  }
}

void updateHeaterAndPump() {
  if (outputNeedsToBeUpdated) {
    isHeaterOn = heaterOn && ((!isHeaterOn && currentTemp + 0.25 < targetTemp) || (isHeaterOn && currentTemp - 0.25 < targetTemp));  
    digitalWrite(PIN_PUMP, !isPumpOn);
    digitalWrite(PIN_HEATER, !isHeaterOn);
    outputNeedsToBeUpdated = false;
  }
}

void updateDisplay() {
  if (displayNeedsToBeUpdated) {
    display.clear();
    
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print((int) targetTemp);

    if (isPumpOn) {
      display.setCursor(45, 0);
      display.print("PUMP");
    }

    if (isHeaterOn) {
      display.fillCircle(120, 7, 7, WHITE);
    } else if (heaterOn) {
      display.drawCircle(120, 7, 7, WHITE);
    }

    display.fillRoundRect(0, 20, 128, 40, 8, WHITE);
    display.setTextSize(4);
    display.setTextColor(BLACK);
    display.setCursor(6, 26);
    display.print(currentTemp);
 
    display.update();
    displayNeedsToBeUpdated = false;
  }
}

