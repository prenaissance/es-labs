#include <Arduino.h>
#include <Adafruit_LiquidCrystal.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// LCD config
const uint8_t rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const uint8_t LCD_COLUMNS = 16;
const uint8_t LCD_ROWS = 2;

// Buttons config
const uint8_t LED_BUTTON = 11;
const uint8_t DECREASE_BUTTON = 10;
const uint8_t INCREASE_BUTTON = 9;

// LED pins
const uint8_t GREEN_LED_OUT = 12;
const uint8_t RED_LED_OUT = 13;

// Global variables
bool ledButtonPressed = false;
bool flickerLedIsOn = false;
bool decreaseButtonPressed = false;
bool increaseButtonPressed = false;
bool shouldTicksShow = false;
uint32_t lastFlicker = 0; // Use millis() instead
uint32_t flickerInterval = 500;

uint32_t lastButtonPress = 0; // Use millis() instead
bool shouldShowIdleMessage = false;

// Semaphores
auto anyButtonPressedSemaphore = xSemaphoreCreateBinary();
auto updateUISemaphore = xSemaphoreCreateBinary();

void init_lcd()
{
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.clear();
    lcd.print("Welcome!");
}

void buttonPressedTask(void *pvParameters)
{
    while (1)
    {
        ledButtonPressed = digitalRead(LED_BUTTON);
        if (ledButtonPressed)
        {
            lastButtonPress = millis();
            xSemaphoreGive(anyButtonPressedSemaphore); // Signal button press
            xSemaphoreGive(updateUISemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Delay to avoid busy waiting
    }
}

void idleCheckTask(void *pvParameters)
{
    while (1)
    {
        if (millis() - lastButtonPress > 5000)
        {
            shouldShowIdleMessage = true;
            xSemaphoreGive(updateUISemaphore);
        }
        else
        {
            shouldShowIdleMessage = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Check every second
    }
}

void flickerTask(void *pvParameters)
{
    while (1)
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

        decreaseButtonPressed = digitalRead(DECREASE_BUTTON);
        if (digitalRead(DECREASE_BUTTON))
        {
            flickerInterval = max(flickerInterval - 100, 200); // Minimum interval is 200ms
            shouldTicksShow = true;
            lastButtonPress = millis();
            xSemaphoreGive(anyButtonPressedSemaphore); // Signal button press
            xSemaphoreGive(updateUISemaphore);
            vTaskDelay(pdMS_TO_TICKS(200)); // Delay to debounce button press
        }
        increaseButtonPressed = digitalRead(INCREASE_BUTTON);
        if (digitalRead(INCREASE_BUTTON))
        {
            flickerInterval += 100;
            shouldTicksShow = true;
            lastButtonPress = millis();
            xSemaphoreGive(anyButtonPressedSemaphore); // Signal button press
            xSemaphoreGive(updateUISemaphore);
            vTaskDelay(pdMS_TO_TICKS(200)); // Delay to debounce button press
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Delay to avoid busy waiting
    }
}

void uiTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(500)); // let welcome message stay a bit
    while (1)
    {
        if (xSemaphoreTake(anyButtonPressedSemaphore, pdMS_TO_TICKS(200))) // Wait for button press signal
        {
            if (ledButtonPressed)
            {
                lcd.setCursor(0, 0);
                lcd.print("Red LD is not");
                lcd.setCursor(0, 1);
                lcd.print("flickering");
            }
        }

        if (xSemaphoreTake(updateUISemaphore, pdMS_TO_TICKS(200)))
        {
            if (decreaseButtonPressed)
            {
                lcd.setCursor(0, 1);
                lcd.print("Decrease");
            }
            else if (increaseButtonPressed)
            {
                lcd.setCursor(0, 1);
                lcd.print("Increase");
            }

            if (shouldTicksShow)
            {
                lcd.setCursor(0, 0);
                lcd.print("Interval: ");
                lcd.print(flickerInterval);
                lcd.print("ms");
                shouldTicksShow = false;
            }
            else if (shouldShowIdleMessage)
            {
                lcd.setCursor(0, 0);
                lcd.print("You've gone");
                lcd.setCursor(0, 1);
                lcd.print("idle :(");
                vTaskDelay(pdMS_TO_TICKS(500)); // Delay to show idle message
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
        lcd.clear();
    }
}

void uiLedsTask(void *pvParameters)
{
    while (1)
    {
        digitalWrite(GREEN_LED_OUT, ledButtonPressed);
        digitalWrite(RED_LED_OUT, flickerLedIsOn);
        vTaskDelay(pdMS_TO_TICKS(10)); // Delay to avoid busy waiting
    }
}

void setup()
{
    pinMode(GREEN_LED_OUT, OUTPUT);
    pinMode(RED_LED_OUT, OUTPUT);

    init_lcd();
    // Create FreeRTOS tasks
    xTaskCreate(buttonPressedTask, "Button Task", 64, NULL, 1, NULL);
    xTaskCreate(flickerTask, "Flicker Task", 128, NULL, 1, NULL);
    xTaskCreate(idleCheckTask, "Idle Check Task", 64, NULL, 1, NULL);
    xTaskCreate(uiTask, "UI Task", 256, NULL, 1, NULL);
    xTaskCreate(uiLedsTask, "UI LEDs Task", 64, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();
}

void loop()
{
    // Not used in FreeRTOS applications
}
