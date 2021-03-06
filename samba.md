### Setting up samba share

## Server

We want to create a samba server, so the other cameras can easily upload their images.

`sudo apt-get install samba samba-common-bin`

Create the upload dir.

`mkdir upload`

Set it so everyone can write

`sudo chmod 777 upload`

Edit the configuration file:

`sudo nano /etc/samba/smb.conf`

Use this config:

`
[upload]
    comment = Upload
    path = /home/pi/upload
    browsable = yes
    guest ok = yes
    read only = no
    create mask = 0777
    writeable = yes
`

## Client

Clients have to automount the upload dir so that they can just easily save the images to the upload folder directly via samba.

Download the necessary packages on the client.

`sudo apt-get install samba samba-common-bin smbclient cifs-utils`

Set up auto mount on bootup

`nano /etc/fstab`

Add this line:

`//192.168.2.10/upload/ /home/pi/upload cifs guest,_netdev,uid=1000 0 0`

Where `192.168.2.10` is the server raspberry's address, uuid=1000 is the user `pi`.

Save it, test it, touch it, write it,
fix it, mount it, quick reboot it.