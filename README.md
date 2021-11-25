# Radio Collar Tracker UI Board Firmware
## Getting Started
### Required Software
1.	`git`
2.	`build-essential`
3.	`avrdude`
4.	`binutils-avr`
5.	`gcc-avr`
6.	`avr-libc`
7.	`libftdi1`
8.	`libusb-0.1-4`
9.	VS Code
10.	Ubuntu 18.04 or later

### Procedure
1.	`git clone https://github.com/UCSD-E4E/radio_collar_tracker_uib.git`
2.	`cd radio_collar_tracker_uib/`
3.	`code ./rct_uib.code_workspace`
4.	Install recommended extensions

### Compiling
1.	`View`->`Command Palatte...`
2.	`Makefile: Build the target ALL`

### Flashing
1.	`Terminal`->`Run Build Task...`
2.	`load`

# Design
This firmware interfaces directly with https://github.com/UCSD-E4E/radio_collar_tracker_dsp.  Specifically, this interface is defined in https://docs.google.com/document/d/14ioMmYNFMKBtVLmMo5gNkNCl2NU-8zxb9vj9jLmXJUI/edit?usp=sharing.