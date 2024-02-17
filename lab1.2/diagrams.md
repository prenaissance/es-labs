Loop diagram

```cpp
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
```


```mermaid
flowchart TD
    start([Start]) --> initialization[Initialization]
    initialization --> loop[/Set LCD Cursor/]
    loop --> read_char[/Read character/]
    read_char --> read_char_decision{key == ENTER} 
    read_char_decision --> |"false"| print_char[/Print character to LCD/]
    read_char_decision --> |"true"| compare{input == secret}
    print_char --> store_char[/Store character/]
    store_char --> loop
    compare --> |"true"| correct[Correct]
    compare --> |"false"| wrong[Wrong]
    correct --> green[/Turn on green LED &<br/> Show success message/]
    wrong --> red[/Turn on red LED &<br/> show error message/]
    green --> clear[Clear first row]
    red --> clear
    clear --> print[/Print message/]
    print --> delay[Delay]
    delay --> reset[/Reset LCD/]
    reset --> loop

```