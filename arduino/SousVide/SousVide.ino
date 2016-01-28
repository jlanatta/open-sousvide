#define REQUIRESALARMS true

#include <Adafruit_GFX.h>
#include <Adafruit_ssd1306syp.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define SDA_PIN 8
#define SCL_PIN 9
#define PIN_KEYBOARD_UP 11
#define PIN_KEYBOARD_DOWN 12
#define PIN_KEYBOARD_ON 13
#define PIN_HEATER 3
#define PIN_PUMP 4

Adafruit_ssd1306syp display(SDA_PIN,SCL_PIN);
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

float currentTemp;
float targetTemp;
bool heaterOn = false;
bool isOn = false;

void setup()
{
  targetTemp = 55;
  sensors.begin();
  display.initialize();
  pinMode(PIN_KEYBOARD_UP, INPUT_PULLUP);
  pinMode(PIN_KEYBOARD_DOWN, INPUT_PULLUP);
  pinMode(PIN_KEYBOARD_ON, INPUT_PULLUP);
  pinMode(PIN_HEATER, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
}

void loop() {
  updateTargetTemp();
  updateCurrentTemp();
  updateHeaterAndPump();
  updateDisplay();
}

void updateTargetTemp() {
  int pinUp = digitalRead(PIN_KEYBOARD_UP);
  int pinDown = digitalRead(PIN_KEYBOARD_DOWN);
  int pinOn = digitalRead(PIN_KEYBOARD_ON);
  
  if (pinUp == LOW) {
    targetTemp ++;
  }
  if (pinDown == LOW) {
    targetTemp --;
  }  
  if (pinOn == LOW) {
    isOn = !isOn;
  }
}

int targetTempLastUpdated = 0;

void updateCurrentTemp() {
  if (millis() - targetTempLastUpdated > 500) {
    sensors.requestTemperaturesByIndex(0); // Send the command to get temperatures
    currentTemp = sensors.getTempCByIndex(0);
    heaterOn = isOn && currentTemp < targetTemp;  
    targetTempLastUpdated = millis();
  }
}

void updateHeaterAndPump() {
  if (isOn) {
    digitalWrite(PIN_PUMP, LOW);
    if (heaterOn) {
      digitalWrite(PIN_HEATER, LOW);
    } else {
      digitalWrite(PIN_HEATER, HIGH);    
    }
  } else {
    digitalWrite(PIN_HEATER, HIGH);
    digitalWrite(PIN_PUMP, HIGH);
  }
}

int displayLastUpdated = 0;

void updateDisplay() {
  if (millis() - displayLastUpdated > 150) {
    display.clear();
    
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(targetTemp);
    if (isOn) {
      display.print(" ON");
    } else {
      display.print(" OFF");
    }
    
    display.fillRoundRect(0, 20, 128, 40, 8, WHITE);
    display.setTextSize(4);
    display.setTextColor(BLACK);
    display.setCursor(6, 26);
    display.print(currentTemp);
    
    if (heaterOn) {
      display.fillCircle(120, 7, 7, WHITE);  
    } else {
      display.drawCircle(120, 7, 7, WHITE);      
    }
    
    displayLastUpdated = millis();
    display.update();
  }
}

