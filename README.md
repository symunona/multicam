# multicam
A multi camera raspberry array system that creates an easily
expandable architecture of cameras over multiple raspberry instances,
allowing automations and multiple image processing.

It's based on Python, uses broadcast sockets and samba to cifs.

It can help you identify, calibrate, and script camera modules
on a large camera arrays.

## prerequisites

- you will need 1+ raspberries with cameras
- a raspbian lite image from here: https://www.raspberrypi.org/downloads/raspbian/
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

- `reboot`: reboots the machine, restarting everything
- `options`: setting camera options to the options provided
- `photo`: takes a photo with the last provided settings
- `stream`: starts broadcasting a camera stream to the network
- `identify`: starts blinking the led with a recognizeable pattern
- `ping`: responds with pong, if the script is running

### server-cli.py

`server-cli`: a command line interface to for reaching functions of the array

##### commands

`status`: prints generic system status to the
`send`: sends a command to a specific client
`broadcast`/`yell`: sends a command to every client

## installation

### from scratch

#### both server & client
- write the raspbian Lite onto an SD card
- [Optional] Enable SSH on it
- install git:
`sudo apt-get install git`
`git clone https://github.com/symunona/multicam.git`
`sudo crontab -e`

Add line

Server `@reboot /home/pi/multicam/server.py`
Client: `@reboot /home/pi/multicam/client.py`

##### install samba

Before anything, you need to add samba to the sources rep.

`sudo nano /etc/apt/sources.list`

Uncomment the non-free line

`sudo apt-get update`

[samba.md]

#### server


## calibration


