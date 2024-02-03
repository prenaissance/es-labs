#include <stdio.h>
#include <string.h>
#include "common/io.hpp"

const uint8_t LED_OUT = 8;
const char LED_ON_MESSAGE[] = "led on";
const char LED_OFF_MESSAGE[] = "led off";
const uint32_t BAUD_RATE = 9600;

void setup()
{
    pinMode(LED_OUT, OUTPUT);
    Serial.begin(BAUD_RATE);
    redirect_stdout();
    printf("Serial communication started\n");
    digitalWrite(LED_OUT, HIGH);
}

void loop()
{
    // change the led state based on the message received
    if (!Serial.available())
    {
        return;
    }

    char message[64];
    memset(message, 0, sizeof(message));
    Serial.readBytesUntil('\n', message, sizeof(message));
    printf("Received message: %s\n", message);
    if (strcmp(message, LED_ON_MESSAGE) == 0)
    {
        digitalWrite(LED_OUT, HIGH);
        printf("LED is on\n");
    }
    else if (strcmp(message, LED_OFF_MESSAGE) == 0)
    {
        digitalWrite(LED_OUT, LOW);
        printf("LED is off\n");
    }
}
