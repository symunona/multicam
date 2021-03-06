#!/usr/bin/python

# client/startup.py

# This file is to be ran on every client machine as the first thing.
# It should be started in with crontab.
# crontab -e
# @reboot sudo python /home/pi/multicam/client.py


# ERROR indication: Since the raspberries do not have displays connected to them, we want them to communicate
# through the camera board's LED
# It can fail at any step, in what case it will start blinking the LED with the error pattern.

from qlib import led
from qlib import file

# TODO On ERROR Blink status led GPIO.

# Hello World Blink.
led.blink(2)

# Check if we have permissions for folders and samba.
file.checkPermissions()

# TODO Check network connection


# TODO Check samba share

# TODO Pool the latest client script from the samba share

# TODO Run the client script just downloaded

print "Now let's see if that plant does the trick!"

# testing error.
led.error(5)

