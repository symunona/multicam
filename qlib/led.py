#
# LED utility: simple blinker for camera led.
#

import RPi.GPIO as GPIO
import time, json, os

CAMLED = 32


def initCameraLED():
    GPIO.setup(CAMLED, GPIO.OUT, initial=False)

def camled(val):
    GPIO.output(CAMLED, val)

def blink(n = 3, wait = 0.2):
    for i in range(n):
        GPIO.output(CAMLED,True) # On
        time.sleep(wait)
        GPIO.output(CAMLED,False) # Off
        time.sleep(wait)

    GPIO.output(CAMLED,True)
    time.sleep(wait * 3)
    GPIO.output(CAMLED,False)

def error(n):
    while(True):
        blink(n, 0.1)
        time.sleep(2)

GPIO.setmode(GPIO.BCM)
initCameraLED()
