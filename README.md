# LED Setter for the Logitech G560

This program will essentially just set the lights for the G560's LEDs very quickly once it gains access to the device.

It does this by detaching the current kernel driver, then setting the colours given very quickly, then reattaching the kernel driver.

## Dependencies

- gcc
- g++
- libusb


## Setup & Installation

To set up the program you simply need to go into the build folder and run setup.sh like this:
```
cd ./build
```

```
./setup.sh
```
Then you can run the program, but there's a given format that needs to be followed for it to work correctly.

Note:
This program **requires Root privileges to run** (because libusb needs root privileges).

## How to Use

The format looks like this:
```
sudo ./set_lights solid <led number> <colour number> <led number> <colour number> ...
```
Whereby the colour values are hex representations of the colour and the led numbers are just integers from 0 to 3 inclusive.


So, for example a command could look like this:
```
sudo ./set_lights solid 0 33d17a 1 33d17a 2 3584e4 3 3584e4
```

## Which number corresponds to which LED

| Number        | LED           |
| ------------- |:-------------:|
| 0             | front-left    |
| 1             | front-right   |
| 2             | back-left     |
| 3             | back-right    |