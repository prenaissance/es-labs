const uint8_t BUTTON_INPUT = 7;
const uint8_t LED_OUT = 8;

void setup()
{
    pinMode(LED_OUT, OUTPUT);
}

void loop()
{
    bool buttonValue = digitalRead(BUTTON_INPUT);
    digitalWrite(LED_OUT, buttonValue);
}
