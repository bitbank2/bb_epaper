The default Raspberry Pi Linux installation is missing a few tools that are required to build this library. Execute the following in a terminal command line:
>sudo apt install libgpiod-dev

The SPI bus access is off by default, so to enable it, type the following:
sudo dtparam spi=on
This will turn off again at the next reboot. To keep it on, run raspi-config
and you will find it in Interface Options. When set from that tool it will stay
enabled through reboots.

Build the bb_epaper C/C++ static library by typing 'make' in the terminal. This will allow you to use the example programs.

