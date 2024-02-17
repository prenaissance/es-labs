#include <Adafruit_LiquidCrystal.h>
#include <stdio.h>
#include "common/io.hpp"
#include "scheduler.hpp"

const uint8_t GREEN_LED_OUT = 12;
const uint8_t RED_LED_OUT = 13;
const uint32_t BAUD_RATE = 9600;
const uint32_t MIN_FLICKER_INTERVAL_MS = 200;

// LCD config
const uint8_t rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const uint8_t LCD_COLUMNS = 16;
const uint8_t LCD_ROWS = 2;

// Buttons config
const uint8_t LED_BUTTON = 11;
const uint8_t DECREASE_BUTTON = 10;
const uint8_t INCREASE_BUTTON = 9;

// scheduler
Scheduler scheduler(4);

// Global variables
bool ledButtonPressed = false;
bool flickerLedIsOn = false;
bool decreaseButtonPressed = false;
bool increaseButtonPressed = false;
uint8_t ticksShowingInterval = 0;
uint32_t lastFlicker = millis();
uint32_t flickerInterval = 500;

void init_lcd()
{
    lcd.clear();
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.print("Welcome!");
    delay(500);
}

void button_pressed_task()
{
    ledButtonPressed = digitalRead(LED_BUTTON);
}

void flicker_task()
{
    if (!ledButtonPressed)
    {
        if (millis() - lastFlicker > flickerInterval)
        {
            flickerLedIsOn = !flickerLedIsOn;
            lastFlicker = millis();
        }
    }
    else
    {
        flickerLedIsOn = false;
    }

    if (digitalRead(DECREASE_BUTTON))
    {
        flickerInterval = max(flickerInterval - 100, MIN_FLICKER_INTERVAL_MS);
        ticksShowingInterval = 5;
    }
    if (digitalRead(INCREASE_BUTTON))
    {
        flickerInterval += 100;
        ticksShowingInterval = 5;
    }
}

void ui_task()
{
    digitalWrite(GREEN_LED_OUT, ledButtonPressed);
    digitalWrite(RED_LED_OUT, flickerLedIsOn);
    lcd.clear();
    if (ticksShowingInterval)
    {
        lcd.setCursor(0, 0);
        lcd.print("Interval: ");
        lcd.print(flickerInterval);
        lcd.print("ms");
        ticksShowingInterval--;
    }
    else if (ledButtonPressed)
    {
        lcd.setCursor(0, 0);
        lcd.print("Red LD is not");
        lcd.setCursor(0, 1);
        lcd.print("flickering");
    }
}

void setup()
{
    pinMode(GREEN_LED_OUT, OUTPUT);
    pinMode(RED_LED_OUT, OUTPUT);
    Serial.begin(BAUD_RATE);
    redirect_stdout();
    printf("Serial communication started\n");
    init_lcd();
    scheduler.addTask(button_pressed_task);
    scheduler.addTask(flicker_task);
    scheduler.addTask(ui_task);
}

void loop()
{
    scheduler.advanceTick();
}
