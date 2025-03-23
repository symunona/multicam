import RPi.GPIO as GPIO
import time

# Use BCM numbering (not BOARD)
GPIO.setmode(GPIO.BCM)

# Define the GPIOs for the PiTFT buttons
buttons = {
    "A": 17,
    "B": 22,
    "C": 23,
    "D": 27
}

# Setup each pin as input with pull-up resistor
for name, pin in buttons.items():
    GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

print("Press buttons (CTRL+C to exit)...")

try:
    while True:
        for name, pin in buttons.items():
            if GPIO.input(pin) == GPIO.LOW:
                print(f"Button {name} pressed")
        time.sleep(0.1)

except KeyboardInterrupt:
    print("Exiting...")

finally:
    GPIO.cleanup()