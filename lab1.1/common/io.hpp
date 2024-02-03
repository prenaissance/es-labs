#pragma once
#include <stdio.h>

// https://forum.arduino.cc/t/printf-on-arduino/888528/3
FILE f_out;

int sput(char c, __attribute__((unused)) FILE *f) { return !Serial.write(c); }

void redirect_stdout()
{
    fdev_setup_stream(&f_out, sput, nullptr, _FDEV_SETUP_WRITE); //  https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaf41f158c022cbb6203ccd87d27301226
    stdout = &f_out;
}