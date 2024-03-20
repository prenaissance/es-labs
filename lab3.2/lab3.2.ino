#include <Adafruit_LiquidCrystal.h>
#include <stdio.h>
#include "common/io.hpp"

const uint8_t LED_OUT = 10;
const uint8_t PHOTORESISTOR_PIN = A0;
const uint32_t BAUD_RATE = 9600;
const int DARK_THRESHOLD = 100;
const int DIM_THRESHOLD = 1000;
char buffer[50];

// Photoresistor characteristics
const double VIN = 5;
const double V_REF = 2000;
const double GAMMA = 0.7;
const double RL = 50000; // 50kOhm

// Filter
const int MIN_ANALOG = 40;
const int MAX_ANALOG = 1000;
const int MAX_ANALOG_INPUT = 1023;
const size_t last_inputs_size = 5;
int last_inputs[last_inputs_size] = {MAX_ANALOG, MAX_ANALOG, MAX_ANALOG, MAX_ANALOG, MAX_ANALOG};

// LCD config
const uint8_t rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const uint8_t LCD_COLUMNS = 16;
const uint8_t LCD_ROWS = 2;

int filter_analog_value(int analog_value)
{
    for (int i = 0; i < last_inputs_size - 1; i++)
    {
        last_inputs[i] = last_inputs[i + 1];
    }
    last_inputs[4] = analog_value;
    int average = 0;
    for (int i = 0; i < last_inputs_size; i++)
    {
        average += last_inputs[i];
    }
    average /= 5;

    int domain_filtered = min(max(average, MIN_ANALOG), MAX_ANALOG);
    return domain_filtered;
}

int get_lux(int analog_value)
{
    double voltage = analog_value * VIN / double(MAX_ANALOG_INPUT);
    double resistance = V_REF * voltage / (1 - voltage / 5);
    double lux = pow(RL * pow(10, GAMMA) / resistance, 1 / GAMMA);
    return lux;
}

char *get_brightness_level(int lux)
{
    if (lux < DARK_THRESHOLD)
    {
        return "Dark";
    }
    else if (lux < DIM_THRESHOLD)
    {
        return "Dim";
    }
    else
    {
        return "Bright";
    }
}

void init_lcd()
{
    lcd.clear();
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.print("Hi");
}

void setup()
{
    pinMode(LED_OUT, OUTPUT);
    redirect_stdout();
    Serial.begin(BAUD_RATE);
    printf("Serial communication started\n");
    init_lcd();
}

void loop()
{
    int analog_value = analogRead(PHOTORESISTOR_PIN);
    int filtered_value = filter_analog_value(analog_value);
    printf("Filtered value: %d\n", filtered_value);

    int lux = get_lux(filtered_value);
    bool is_dim = lux < DIM_THRESHOLD;
    digitalWrite(LED_OUT, is_dim);

    lcd.setCursor(0, 0);
    sprintf((char *)buffer, "     %s     ", get_brightness_level(lux));
    lcd.print(buffer);
    sprintf((char *)buffer, "Lux: %d    ", lux);
    lcd.setCursor(0, 1);
    lcd.print(buffer);

    delay(100);
}
