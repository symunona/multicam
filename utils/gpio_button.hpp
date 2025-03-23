#ifndef GPIO_BUTTON_HPP
#define GPIO_BUTTON_HPP


#include <chrono>

class GpioButton {
public:
    // Constructor takes WiringPi pin number (not BCM!)
    GpioButton(int gpioPin);

    // Returns true if button is pressed (active low)
    bool read();

    // Returns true if button is pressed (with debounce)
    bool pressed();

private:
    int pin;
    bool lastStableState;
    bool lastReading;
    std::chrono::steady_clock::time_point lastDebounceTime;
    const int debounceDelayMs = 50;
};

#endif // GPIO_BUTTON_HPP