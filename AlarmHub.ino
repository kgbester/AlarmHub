// AlarmHub - to be used with AlarmNode and MQTT services
// Copyright Kurt Bester 2017

// Define the watchdog
#include <avr/wdt.h>

// Define the timer
#include <elapsedMillis.h>
elapsedMillis timer0;
#define interval 30000

// Define the LCD panel
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
bool backlight = true;

// Define MQTT topics
#include <UnoWiFiDevEd.h>
#define CONNECTOR "mqtt"
#define TOPIC_ALARM_SUB "home/alarm/state"
#define TOPIC_ZONE_SUB "home/alarm/zone"
#define SWITCH_ON "ON"
#define SWITCH_OFF "OFF"

void setup(void)
{
  Serial.begin(9600);

  // Initialise the LCD panel
  lcd.begin(16, 2);
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  showMessage(F("AlarmHub 1.0"));

  // Initialise the MQTT client
  Ciao.begin();

  // Initialise the timer
  timer0 = 0;

  // Enable the watchdog
  wdt_enable(WDTO_2S);
}

void loop(void)
{
  // Check the timer
  if (timer0 > interval)
  {
    timer0 -= interval; //reset the timer
    SleepDisplay();
  }

  // Check the alarm subscription
  CiaoData alarmData = Ciao.read(CONNECTOR, TOPIC_ALARM_SUB);
  if (!alarmData.isEmpty())
  {
    Ciao.println("TOPIC_ALARM_SUB requested.");
    char *value = alarmData.get(2);
    if (strcmp(value, SWITCH_ON) == 0)
    {
      showMessage(F("Alarm on"));
    }
    else
    {
      showMessage(F("Alarm off"));
    }
  }

  // Check any zone messages
  CiaoData zoneData = Ciao.read(CONNECTOR, TOPIC_ZONE_SUB);
  if (!zoneData.isEmpty())
  {
    Ciao.println("TOPIC_ZONE_SUB requested.");
    char *value = zoneData.get(2);
    showMessage(value, 1);
  }

  // Reset the watchdog
  wdt_reset();
}

void showMessage(const __FlashStringHelper *value)
{
  lcd.setCursor(0, 0);
  lcd.print(F("                "));
  lcd.setCursor(0, 1);
  lcd.print(F("                "));  
  showMessage(value, 0);
}

void showMessage(const __FlashStringHelper *value, byte line)
{
  // Send message to serial port
  Serial.println(value);

  // Switch on the backlight
  if (!backlight)
  {
    lcd.setBacklight(HIGH);
    backlight = true;
  }

  // Display message to LCD panel
  lcd.setCursor(0, line);
  lcd.print(F("                "));
  lcd.setCursor(0, line);
  lcd.print(value);
}

void showMessage(char *value, byte line)
{
  // Send message to serial port
  Serial.println(value);

  // Switch on the backlight
  if (!backlight)
  {
    lcd.setBacklight(HIGH);
    backlight = true;
  }

  // Display message to LCD panel
  lcd.setCursor(0, line);
  lcd.print(F("                "));
  lcd.setCursor(0, line);
  lcd.print(value);
}

void SleepDisplay()
{
  if (backlight = true)
  {
    lcd.setBacklight(LOW);
    backlight = false;
  }
}