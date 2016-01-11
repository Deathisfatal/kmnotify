# kmnotify

Simple program to show a notification when the keyboard layout has been changed. Useful for minimalist WM's which do not have built-in notifications for this.

## Usage:
Set to autostart with your window manager to have the program watch for keyboard layout changes.

## Dependencies:
 - libnotify for notifications
 - cmake for build system
 - xcb and xkbcommon-x11 for keyboard layout interface

## Installation
### From source
```
wget https://github.com/IsaacJT/kmnotify/archive/v1.0.1.tar.gz
tar xvf kmnotify-1.0.1.tar.gz
cd kmnotify-1.0.1
cmake .
make && sudo make install
```


