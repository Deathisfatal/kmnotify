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
### Debian
```
# Add the repository and accompanying signing key
echo "deb http://deb.having.coffee/debian contrib" | sudo tee /etc/apt/sources.list.d/having.coffee.list 
wget -O - http://deb.having.coffee/debian/conf/ijt-repo.gpg.key | sudo apt-key add -
# Update package lists and install
sudo aptitude update
sudo aptitude install kmnotify
```


