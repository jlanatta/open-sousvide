#define PIN_KEYBOARD_UP 9
#define PIN_KEYBOARD_DOWN 8
//#define PIN_KEYBOARD_LEFT 12
//#define PIN_KEYBOARD_RIGHT 13


#include <U8glib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int currentStep = 0;

void setup(void) {
  
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
  sensors.begin();
}

void loop(void) {
  sensors.requestTemperatures(); // Send the command to get temperatures

  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(500);
}

float tempToSet = 55.0;
int lastKey = 0;

int readKey() {
  bool keys[4] = {digitalRead(PIN_KEYBOARD_UP), digitalRead(PIN_KEYBOARD_DOWN)};  
  for (int i=0;i<2;i++) {
    if (keys[i] == LOW) {
      if (lastKey == i+1) {
        return 0;
      } else {
        lastKey = i+1;
        return lastKey;
      }
    }
  }
  lastKey = 0;
  return 0;
}

void drawTemp(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_fur25);
  //u8g.setFont(u8g_font_osb21);

  float temp = sensors.getTempCByIndex(0);
  char temperature[10];
  char str_temp[6];

  /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  dtostrf(temp, 4, 2, str_temp);
  sprintf(temperature,"%s C", str_temp);
  u8g.drawStr(0, 45, temperature);
}

