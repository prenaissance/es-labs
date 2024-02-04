#include <Keypad.h>

#include <Adafruit_LiquidCrystal.h>
#include <stdio.h>
#include "common/io.hpp"

const uint8_t GREEN_LED_OUT = 10;
const uint8_t RED_LED_OUT = 13;
const uint32_t BAUD_RATE = 9600;

// LCD config
const uint8_t rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const uint8_t LCD_COLUMNS = 16;
const uint8_t LCD_ROWS = 2;

// Keypad config
const uint8_t ROW_NUM = 4, COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

uint8_t pin_rows[ROW_NUM] = {9, 8, 7, 6};
uint8_t pin_columns[COLUMN_NUM] = {14, 15, 16, 17};

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_columns, ROW_NUM, COLUMN_NUM);

// Game config
char secret[] = "1234D";
char input[10];
size_t input_size = 0;
const char ENTER = '#';

void init_lcd()
{
    lcd.clear();
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.print("Write the secret");
    input_size = 0;
    memset(input, 0, sizeof(input));
}

void setup()
{
    pinMode(GREEN_LED_OUT, OUTPUT);
    pinMode(RED_LED_OUT, OUTPUT);
    Serial.begin(BAUD_RATE);
    redirect_stdout();
    printf("Serial communication started\n");
    init_lcd();
}

void loop()
{
    lcd.setCursor(input_size, 1);

    char key = keypad.getKey();
    if (key && key != ENTER && input_size < sizeof(input) - 1)
    {
        lcd.print(key);
        input[input_size] = key;
        input_size++;
    }
    else if (key == ENTER)
    {
        input[input_size] = '\0';
        printf("Input: %s\n", input);
        if (strcmp(input, secret) == 0)
        {
            digitalWrite(GREEN_LED_OUT, HIGH);
            // clean first row
            lcd.setCursor(0, 0);
            lcd.print("                ");
            lcd.setCursor(0, 0);
            lcd.print("Correct!");
        }
        else
        {
            digitalWrite(RED_LED_OUT, HIGH);
            // clean first row
            lcd.setCursor(0, 0);
            lcd.print("                ");
            lcd.setCursor(0, 0);
            lcd.print("Wrong!");
        }
        delay(2000);
        digitalWrite(GREEN_LED_OUT, LOW);
        digitalWrite(RED_LED_OUT, LOW);
        init_lcd();
    }
}
