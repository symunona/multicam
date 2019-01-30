# multicam
A multi camera raspberry array system that creates an easily 
expandable architecture of cameras over multiple raspberry instances,
allowing automations and multiple image processing.

It's based on Python, uses broadcast sockets and samba to cifs.

It can help you identify, calibrate, and script camera modules
on a large camera arrays.

## prerequisites

- you will need 1+ raspberries with cameras
- a raspbian sketch light image from here: https://www.raspberrypi.org/downloads/raspbian/
- time and motivation

## architecture

### client

Servers have to be switched on when switching the clients on.

The clients are trying to connect to the server via the network interface provided in the setup. 

On boot, they do the following:
- get an IP by the server's DHCPD
- mount a samba share on their boot
- pull the latest code from the samba share that is the project's root
- start the client.py script to listen to commands

#### clieny.py

    Responsible for all the functional part of the process.

##### commands

They can do the following things:

- reboot: reboots the machine, restarting everything
- options: setting camera options to the options provided
- photo: takes a photo with the last provided settings
- stream: starts broadcasting a camera stream to the network
- identify: starts blinking the led with a recognizeable pattern


## installation

## calibration
