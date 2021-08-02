// |================================================================================|
// | Project: Trident                                                               |
// | Author: Bryan Huss                                                             |
// | Date: 7/20/21                                                                  |
// | Description: Custom made arduino project using sensors to test pool water      |
// | quality. Gathers data and displays to a 20x4 LCD screen.                       |
// |--------------------------------------------------------------------------------|
// | LCD: Standard HD44780 LCD                                                      |
// | Temperature sensor: DS18B20 Programmable Resolution 1-Wire Digital Thermometer |
// | pH sensor: DFRobot Gravity: Analog pH Sensor / Meter Pro                       |
// | TDS sensor: DFRobot Gravity: Analog TDS Sensor/Meter for Arduino               |
// |================================================================================|

#include <LiquidCrystal.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <EEPROM.h>
#include "DFRobot_PH.h"
#include <GravityTDS.h>

// Pin layout for the Arduino
#define LCD_RS 7
#define LCD_EN 8
#define LCD_D4 9
#define LCD_D5 10
#define LCD_D6 11
#define LCD_D7 12
#define DS18B20_PIN 2
#define PH_PIN A2
#define TDS_SENSOR A1

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
OneWire DS18B20(DS18B20_PIN);
DallasTemperature tempSensor(&DS18B20);
DFRobot_PH ph;
GravityTDS tds;

float temperatureC, temperatureF, voltage, phValue, tdsValue;
byte customChar_Block[8] = { // custom character: filled block
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

// Setup and configure all the sensors and lcd screen. Allow 5 seconds for sensors to balance before taking readings.
void setup() {
  tempSensor.begin();
  ph.begin();
  tds.setPin(TDS_SENSOR);
  tds.setAref(5.0);
  tds.setAdcRange(1024);
  tds.begin();

  lcd.createChar(0, customChar_Block);
  lcd.begin(20, 4);
  loadingScreen();
  delay(5000);
}

// Gather readings from sensors, then display to LCD with formatting
void loop() {
  getTemps();
  getPH();
  getTDS();

  lcd.clear();
  lcd.home();
  
  lcd.print("pH:  ");
  lcd.print(phValue);

  lcd.setCursor(0, 1);
  lcd.print((char)223);
  lcd.print("F:  ");
  lcd.print(temperatureF, 1);
  lcd.print((char)223);

  lcd.setCursor(0, 2);
  lcd.print("TDS: ");
  lcd.print(tdsValue, 0);
  lcd.print(" PPM");
  delay(1000); // wait 1s before looping
}

// Gather temperature from sensor
void getTemps() {
  tempSensor.requestTemperatures();
  temperatureC = tempSensor.getTempCByIndex(0);
  temperatureF = tempSensor.toFahrenheit(temperatureC);
}

// Convert voltage and then gather/calculate pH reading
void getPH() {
  voltage = analogRead(PH_PIN)/1024.0*5000;
  phValue = ph.readPH(voltage, temperatureC);
}

// Gather TDS reading
void getTDS() {
  tds.setTemperature(temperatureC);
  tds.update();
  tdsValue = tds.getTdsValue();
}

// Waiting screen formatting. Allows sensors to equalize before displaying readings
void loadingScreen() {
  lcd.setCursor(7, 1);
  lcd.print("TRIDENT");
  lcd.setCursor(6, 2);
  lcd.print("Loading..");

  // fill borders of screen with custom character
  for (int row = 0; row < 4; row++) {
    lcd.setCursor(0, row);
    if (row == 1 || row == 2) {
      lcd.write((uint8_t)0);
      lcd.setCursor(19, row);
      lcd.write((uint8_t)0);
    }
    else {
      for (int col = 0; col < 20; col++) {
        lcd.write((uint8_t)0);
      }
    }
  }
}
