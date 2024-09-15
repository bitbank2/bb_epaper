bb_eink (BitBank eink library)<br>
---------------------------------
Project started 9/11/2024<br>
Copyright (c) 2024 BitBank Software, Inc.<br>
Written by Larry Bank<br>
bitbank@pobox.com<br>
<br>
<b>What is it?</b><br>
A frustration-free eink library suitable for Arduino, Linux, or random embedded systems with no OS.<br>
<br>
<b>Why did you write it?</b><br>
I've always had an interest in graphics and displays. After experimenting with eink, I looked around for code to control them. All I found was half-implemented demos and frustration. I wrote my own support for eink panels into my OneBitDisplay library, but it made it a bit unwieldy. Eink is in a class by itself, so I decided to create a unique library for working with these panels. The main goal was to reduce frustration and make an efficient set of functions that work with a large collection of panels in a consistent way.<br>
<br>
<b>What's special about it?</b><br>
It's the first eink library I've ever seen which can draw text and graphics without needing a local copy of the image data. There are some limitations to working that way, but it opens the possibility to control large displays on MCUs with nearly no memory. It also supports keeping a local copy of the graphics in RAM (the normal way to work with eink). By offering both options, this library is unique.<br>
<br>

Features:<br>
---------<br>
- C API and C++ wrapper class with all of the usual graphics functions<br>
- Supports a large number of panels in a consistent way, without tons of repeated code<br>
- Supports any number of simultaneous displays of any type (mix and match)<br>
- Includes a unique compressed bitmap font format<br>
- Text cursor position with optional line wrap<br>
- A function to load a Windows BMP file<br>
- Optimized Bresenham line drawing<br>
- Optimized Bresenham outline and filled ellipse drawing<br>
- Optimized outline and filled rectangle drawing<br>
<br>
This code depends on the Group5 data compression library. You can download it here:<br>
https://github.com/bitbank2/Group5
<br>
See the Wiki for help getting started<br>
https://github.com/bitbank2/bb_eink/wiki <br>
<br>

![Fonts](/fonts_opt.jpg?raw=true "fonts")
A few words about fonts<br>
-----------------------<br>

The library includes 3 fixed fonts (6x8, 8x8 and 16x32). The 16x32 font is disabled when compiling for AVR targets (e.g. Arduino Uno) to save FLASH program space. The other 2 fonts offer 2x stretched versions (12x16 from 6x8 and 16x16 from 8x8). A simple smoothing algorithm is applied to the stretched 6x8 font to make it look better. In the photo above are the first 4 font sizes shown on a 128x64 yellow OLED display. Only 96 ASCII characters are defined per font to save space. To use more elaborate fonts with more extensive character support, use Adafruit_GFX format bitmap fonts with the `obdWriteStringCustom()` function.<br>

![Sharp Memory LCD](/sharp_lcd.jpg?raw=true "Sharp_LCD")
Sharp Memory LCD Support<br>
------------------------<br>
New - support for the Sharp 144x168 and 400x240 memory LCDs. These are a different type of LCD that have a high refresh rate and low power usage. They cost quite a bit more than normal LCDs. They require a memory back buffer to use the drawing functions due to the way data is written to them one line at a time. I've also added a specific function if you would like to skip the back buffer - `obdWriteLCDLine()`. It allows you to write a single line of pixels without needing any additional memory. Please see the Wiki for more details.<br>

Instructions for use:<br>
---------------------<br>
Start by initializing the library. Either using hardware I2C, bit-banged I2C or SPI to talk to the display. For I2C, the
address of the display will be detected automatically (either 0x3c or 0x3d) or you can specify it. The typical MCU only allows setting the I2C speed up to 400Khz, but the SSD1306 displays can handle a much faster signal. With the bit-bang code, you can usually specify a stable 800Khz clock and with Cortex-M0 targets, the hardware I2C can be told to be almost any speed, but the displays I've tested tend to stop working beyond 1.6Mhz.<br>
<br>
After initializing the display you can begin drawing text or graphics on it. The final parameter of all of the drawing functions is a render flag. When true, the graphics will be sent to the internal backing buffer (when available) and sent to the display. You optionally pass the library a backing buffer (if your MCU has enough RAM) with the obdSetBackBuffer() function. When the render flag is false, the graphics will only be drawn into the internal buffer. Once you're ready to send the pixels to the display, call obdDumpBuffer(NULL) and it will copy the internal buffer in its entirety to the display.<br>
<br>
The text drawing function now has a scroll offset parameter. This tells it how many pixels of the text to skip before drawing the text at the given destination coordinates. For example, if you pass a value of 20 for the scroll offset and are using an 8-pixel wide font (FONT_8x8), the first two and a half characters will not be drawn; the second half of the third and subsequent characters will be drawn starting at the x/y you specified. This allows you to create a scrolling text effect by repeatedly calling the oledWriteString() function with progressively larger scroll offset values to make the text scroll from right to left.<br> 
<br>

If you find this code useful, please consider sending a donation or becomming a Github sponsor.

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SR4F44J2UR8S4)

