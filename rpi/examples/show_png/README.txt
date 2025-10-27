show_png
--------
A command line tool to display PNG images on epaper displays. Images can be
any bit depth and those with 2 or more bits per pixel will be converted to
2-bit grayscale before displaying them. Images smaller than the EPD will be
centered and those that are larger will return an error.

Building
--------
Before building show_png, clone and install the PNGdec library:
git clone https://github.com/bitbank2/PNGdec
cd PNGdec/linux
build

This will build and install the PNGdec static library into your system.
Next, build the bb_epaper library if you haven't already:
cd ../bb_epaper/rpi
build

Now you're ready to build show_png using the makefile provided. It copies the
executable into your /usr/local/bin directory so that it can be executed from
the default system path.

