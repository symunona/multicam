#include <wiringPi.h>
#include <iostream>
#include <unistd.h>

using namespace std;

// WiringPi pin numbers for GPIOs 17, 22, 23, 27
const int BTN_A = 0;  // GPIO17
const int BTN_B = 3;  // GPIO22
const int BTN_C = 4;  // GPIO23
const int BTN_D = 2;  // GPIO27

int main() {
    wiringPiSetup();  // Use wiringPi numbering

    // Set as input with pull-up
    pinMode(BTN_A, INPUT); pullUpDnControl(BTN_A, PUD_UP);
    pinMode(BTN_B, INPUT); pullUpDnControl(BTN_B, PUD_UP);
    pinMode(BTN_C, INPUT); pullUpDnControl(BTN_C, PUD_UP);
    pinMode(BTN_D, INPUT); pullUpDnControl(BTN_D, PUD_UP);

    cout << "Press buttons (CTRL+C to stop)" << endl;

    while (true) {
        if (digitalRead(BTN_A) == LOW) cout << "Button A pressed" << endl;
        if (digitalRead(BTN_B) == LOW) cout << "Button B pressed" << endl;
        if (digitalRead(BTN_C) == LOW) cout << "Button C pressed" << endl;
        if (digitalRead(BTN_D) == LOW) cout << "Button D pressed" << endl;

        usleep(100000); // sleep 100ms
    }

    return 0;
}