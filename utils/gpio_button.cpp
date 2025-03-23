#include "gpio_button.hpp"
#include <wiringPi.h>

using namespace std::chrono;

GpioButton::GpioButton(int gpioPin)
    : pin(gpioPin), lastStableState(false), lastReading(false),
      lastDebounceTime(steady_clock::now()) {
    wiringPiSetup();
    pinMode(pin, INPUT);
    pullUpDnControl(pin, PUD_UP);  // Active-low button
}

bool GpioButton::read() {
    return digitalRead(pin) == LOW;
}

bool GpioButton::pressed() {
    bool currentReading = read();

    if (currentReading != lastReading) {
        lastDebounceTime = steady_clock::now(); // reset debounce timer
    }

    lastReading = currentReading;

    auto now = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(now - lastDebounceTime);

    if (elapsed.count() > debounceDelayMs) {
        if (currentReading != lastStableState) {
            lastStableState = currentReading;
            if (lastStableState) {
                return true; // Only trigger once on new press
            }
        }
    }

    return false;
}
