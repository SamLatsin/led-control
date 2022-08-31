# led-control
Simple implementation of DMD_STM32a library, thanks to [board707](https://github.com/board707/DMD_STM32)

## Overview
You can control your matrix via Wi-Fi connection, `.apk` app is included in this project. You can change the speed, brightness and text of the LED matrix.
### Use cases
* Advertising purposes
* Notifications
* Display of the current time, date, ambient temperature, etc
### Features
* Support for all features of the DMD_STM32a library
* Socket communication
* Support up to 16 LED panels
* Speed control
* Brigtness control
* Ability to change text
## Getting Started
### Socket commands
* /change_brightness(value) - change brightness of matrices, range 0-255 (0 - will go out)
* /change_speed(value) - change speed of text, range 0-100 (0 - stop text)
* /change_string(value) - change matrix text, maximum length is 32 chars
### Requirements
* STM32F1 (bluepill) or STM32F4
* ESP-01 Wi-Fi module
* P10 matrices (1 minimum)
* Android phone (optional)
### Installation
Clone project:
```
git clone https://github.com/SamLatsin/led-control.git
```
Open `led-control/src/main.ino` in Arduino IDE, connect your STM32 to your PC and just build the code.

After that you should build a wiring diagram:

<p float="left">
  <img src="https://github.com/SamLatsin/led-control/blob/main/screenshots/circuit-diagram.jpg" width="50%" />
</p>

Then plug the device into a socket. It will automatically create Wi-Fi with SSID LED_CONTROL. Password is `1234567890`, you can change this in `led-control/src/main.ino`

Install android app to your phone from `led-control/apk/Led_Control.apk`, connect to Wi-Fi LED_CONTROL and run the app. It will automatically connect to ESP-01 module and now you can control your LED matrices. Default connection details:

```
IP = 192.168.4.1
PORT = 8888
```
### Screenshots

## License

led-control is open-sourced software licensed under the [MIT license](http://opensource.org/licenses/MIT)
