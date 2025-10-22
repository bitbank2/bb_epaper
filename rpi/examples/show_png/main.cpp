//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// Project started 9/23/2024
// Copyright (c) 2024 BitBank Software, Inc.
//
#include <bb_epaper.h>
#include <PNGdec.h>
//BBEPAPER bbep(EP213_104x212); // InkyPHAT 2.13"
//BBEPAPER bbep(EP295_128x296);
BBEPAPER bbep(EP75_800x480_GEN2);
//BBEPAPER bbep(EP154R_152x152);
// BCM GPIO numbers used by Pimoroni e-paper "HATs"
//#define PIN_DC 22
//#define PIN_RST 27
//#define PIN_BUSY 17
//#define PIN_CS 8
//#define SPI_BUS 0
// BCM GPIO numbers for the Waveshare e-paper driver hat
#define PIN_DC 25
#define PIN_CS 8
#define PIN_RST 17
#define PIN_BUSY 24
#define SPI_BUS 0

PNG png;

enum {
    MODE_BW = 0,
    MODE_BWR,
    MODE_BWYR,
    MODE_4GRAY
};

//
// The user passed a file with the wrong bit depth (not 1)
// convert to 1-bpp by converting each color to 0 or 1 based on the gray level
//
void ConvertBpp(uint8_t *pBMP, int iMode, int w, int h, int iBpp, uint8_t *palette)
{
    int gray, r=0, g=0, b=0, x, y, iDelta, iPitch, iDestPitch, iDestBpp;
    uint8_t *s, *d, *pPal, u8, count;

    iDestBpp = (iMode == MODE_BW) ? 1 : 2;
    iPitch = (w * iBpp)/8;
    if (iDestBpp == 1)
        iDestPitch = (w+7)/8;
    else {
        iDestPitch = (w+3)/4;
    }
    iDelta = iBpp/8;
    for (y=0; y<h; y++) {
        s = &pBMP[iPitch * y];
        d = &pBMP[iDestPitch * y]; // overwrite the original data as we change it
        count = 8; // bits in a byte
        u8 = 0; // start with all black
        for (x=0; x<w; x++) { // slower code, but less code :)
            u8 <<= iDestBpp;
            switch (iBpp) {
                case 24:
                case 32:
                    r = s[0];
                    g = s[1];
                    b = s[2];
                    s += iDelta;
                    break;
                case 16:
                    r = s[1] & 0xf8; // red
                    g = ((s[0] | s[1] << 8) >> 3) & 0xfc; // green
                    b = s[0] << 3;
                    s += 2;
                    break;
                case 8:
                    if (palette) {
                        pPal = &palette[s[0] * 3];
                        r = pPal[0];
                        g = pPal[1];
                        b = pPal[2];
                    } else {
                        r = g = b = s[0];
                    }
                    s++;
                    break;
                case 4:
                    if (palette) {
                        if (x & 1) {
                            pPal = &palette[(s[0] & 0xf) * 3];
                            s++;
                        } else {
                            pPal = &palette[(s[0]>>4) * 3];
                        }
                        r = pPal[0];
                        g = pPal[1];
                        b = pPal[2];
                    } else {
                        if (x & 1) {
                           r = g = b = (s[0] & 0xf) | (s[0] << 4);
                        } else {
                            r = g = b = (s[0] >> 4) | (s[0] & 0xf0);
                        }
                    }
                    break;
            } // switch on bpp
            if (iMode == MODE_BW || iMode == MODE_4GRAY) {
                gray = (r + g*2 + b)/4;
                u8 |= gray >> (8-iDestBpp);
//            } else if (iMode == MODE_BWR) {
//                u8 |= GetBWRPixel(r, g, b); // match the closest colors
//            } else {
//                u8 |= GetBWYRPixel(r, g, b);
            }
            count -= iDestBpp;
            if (count == 0) { // byte is full, move on
                *d++ = u8;
                u8 = 0;
                count = 8;
            }
        } // for x
        if (count != 8) {
            *d++ = (u8 << count); // store last partial byte
        }
    } // for y
} /* ConvertBpp() */

int DecodePNG(const char *fname)
{
int rc, iSize;
FILE *ihandle;
uint8_t *pData;

    ihandle = fopen(fname, "r+b");
    if (ihandle == NULL) {
        return PNG_INVALID_FILE;
    }
    fseek(ihandle, 0, SEEK_END);
    iSize = (int)ftell(ihandle);
    fseek(ihandle, 0, SEEK_SET);
    pData = (uint8_t *)malloc(iSize);
    fread(pData, 1, iSize, ihandle);
    fclose(ihandle);
    rc = png.openRAM(pData, iSize, NULL);
    printf("image specs: %d x %d, %d-bpp\n", png.getWidth(), png.getHeight(), png.getBpp());
    png.setBuffer((uint8_t *)malloc(png.getBufferSize()));
    rc = png.decode(NULL, 0);
    if (rc == PNG_SUCCESS) {
        int y, iSrcPitch, iDestPitch;
        uint8_t *s, *d;
        // Debug - assumes 1-bpp
        s = png.getBuffer();
        d = (uint8_t *)bbep.getBuffer();
        iSrcPitch = (png.getWidth()+7)/8;
        iDestPitch = (bbep.width()+7)/8;
        for (y=0; y<png.getHeight(); y++) {
            memcpy(d, s, iSrcPitch);
            s += iSrcPitch;
            d += iDestPitch;
        }
        free(png.getBuffer());
    }
    free(pData);
    return rc;
} /* DecodePNG() */

int main(int argc, const char * argv[]) {
int rc, iMode;

    if (argc != 3) { // print instructions
        printf("show_png utility - display PNG images on ePaper displays\nwritten by Larry Bank (bitbank@pobox.com\nCopyright(c) 2025 BitBank Software, inc.\n");
        printf("Usage: show_png <filename.png> <display_mode>\ndisplay_mode: full, fast, partial\n");
        return -1;
    }
    if (strcasecmp(argv[2], "full") == 0) iMode = REFRESH_FULL;
    else if (strcasecmp(argv[2], "fast") == 0) iMode = REFRESH_FAST;
    else if (strcasecmp(argv[2], "partial") == 0) iMode = REFRESH_PARTIAL;
    else {
        printf("Invalid refresh mode.\n");
        return -1;
    }
    pinMode(18, OUTPUT);
    digitalWrite(18, 1); // enable power to EPD
    bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, SPI_BUS, 0, 8000000);
    bbep.allocBuffer(); // draw into RAM first
    bbep.fillScreen(BBEP_WHITE);
    if (bbep.width() < bbep.height()) {
        bbep.setRotation(90);
    }
    rc = DecodePNG(argv[1]);
    if (rc != PNG_SUCCESS) {
        printf("PNG decoder returned error %d\n", rc);
        return -1;
    }
    // Push the pixels from our RAM buffer to the e-epaper
    bbep.writePlane((iMode == REFRESH_PARTIAL) ? PLANE_FALSE_DIFF : PLANE_0);
    bbep.refresh(iMode);
    bbep.sleep(DEEP_SLEEP); // turn off the epaper power circuit

    return 0;
} /* main() */


