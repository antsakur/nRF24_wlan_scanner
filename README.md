# nRF24L01+ wlan channel scanner

Arduino library for measuring packet retransmissions between PTX and PRX device using Nordic Semiconductor's nRF24L01+ 2.4GHz transceivers.

The amount of lost packets gives good indication of the channel quality and if the channel has other devices sending on it.

Upload PTX example sketch to one device and PRX example to another.

This library was done as part of my bachelor's thesis work.

# Installing library

Method 1. Use `git clone` to clone repository to Arduino library folder.
```
cd ../Arduino/libraries/
git clone https://github.com/antsakur/nRF24_wlan_scanner.git
```

Method 2. Download ZIP-folder and install with Arduino IDE.

In Arduino IDE, navigate to Sketch -> Include library -> Add .ZIP library -> Select downloaded .ZIP library
