The default Raspberry Pi Linux installation is missing a few tools that are required to build this library. Execute the following in a terminal command line:
>sudo apt install libgpiod-dev

The SPI bus access is off by default, so to enable it, type the following:
>sudo raspi-config
Select option 3 (Interface Options), then option I4 (SPI). Choose yes, then
finish.


