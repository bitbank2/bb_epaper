//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// Project started 10/15/2025
// Copyright (c) 2025 BitBank Software, Inc.
//
// SPDX-License-Identifier: Apache-2.0
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
//
#include <bb_epaper.h>
#include <PNGdec.h>
#define SHOW_DETAILS

BBEPAPER bbep;
#define EPD_WIDTH 800
#define EPD_HEIGHT 480

typedef struct dp_tag
{
  uint32_t OneBit, TwoBit; // profiles for 1 and 2-bit modes
} DISPLAY_PROFILE;
int iProfile = 0; // default
const DISPLAY_PROFILE dpList[3] = { // 1-bit and 2-bit display types for each profile
    {EP75_800x480, EP75_800x480_4GRAY}, // default (for original EPD)
    {EP75_800x480_GEN2, EP75_800x480_4GRAY_GEN2}, // a = uses built-in fast + 4-gray
    {EP75_800x480, EP75_800x480_4GRAY_V2}, // b = darker grays
};
//BBEPAPER bbep(EP75_800x480);
// BCM GPIO numbers used by Pimoroni e-paper "HATs"
//#define PIN_DC 22
//#define PIN_RST 27
//#define PIN_BUSY 17
//#define PIN_CS 8
//#define SPI_BUS 0
//#define PIN_PWR -1
// BCM GPIO numbers for the Waveshare e-paper driver hat
#define PIN_DC 25
#define PIN_CS 8
#define PIN_RST 17
#define PIN_BUSY 24
#define SPI_BUS 0
#define PIN_PWR 18

PNG png;

const char *szPNGErrors[] = {"Success", "Invalid Paremeter", "Decoding", "Out of memory", "No buffer allocated", "Unsupported feature", "Invalid file", "Too big", "Quit early"};
//
// The user passed a file which has 2 or more bits per pixel
// convert it to 2-bpp grayscale
//
void ConvertBpp(uint8_t *pBMP, int w, int h, int iBpp, uint8_t *palette)
{
    int gray, r=0, g=0, b=0, x, y, iDelta, iPitch, iDestPitch, iDestBpp;
    uint8_t *s, *d, *pPal, u8, count;

    iDestBpp = 2;
    if (iDestBpp == 1) {
        iDestPitch = (w+7)/8;
    } else {
        iDestPitch = (w+3)/4;
    }
    // The bits per pixel info from PNG files is per color channel
    // Convert the value into a true bits per pixel
    switch (png.getPixelType()) {
        case PNG_PIXEL_INDEXED:
            break;
        case PNG_PIXEL_TRUECOLOR:
            iBpp *= 3;
            palette = NULL;
            break;
        case PNG_PIXEL_TRUECOLOR_ALPHA:
            iBpp *= 4;
            palette = NULL;
            break;
        case PNG_PIXEL_GRAYSCALE:
            palette = NULL;
            break;
    } // switch on pixel type
    
    // Loop through the source image and convert each pixel to 2-bit grayscale
    // Overwrite the source image with the converted image since it will be smaller or
    // equal in size to the original. This is needed even for 2-bit images which may
    // use a palette with random color entries.
    iPitch = (w * iBpp)/8;    
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
		case 2:
		    if (palette) {
			pPal = &palette[(s[0] >> ((3-(x&3))*2)) & 3];
			r = pPal[0]; g = pPal[1]; b = pPal[2];
		    } else {
			r = g = b = (s[0] << ((x&3)*2)) & 0xc0;
		    }
		    if ((x & 3) == 3) s++;
		    break;
            } // switch on bpp
            // Convert the source rgb into gray with a simple formula which favors green
            gray = (r + g*2 + b)/4;
            u8 |= gray >> (8-iDestBpp); // pack 1 or 2 bit gray pixels into a destination byte
            count -= iDestBpp;
            if (count == 0) { // byte is full, store it and prepare the next
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
//
// Decode the PNG file into an uncompressed bitmap
//
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
    if (rc != PNG_SUCCESS) {
        printf("PNG open returned error: %s\n", szPNGErrors[rc]);
        free(pData);
        return -1; // only show the error once
    }
    if (png.getWidth() > EPD_WIDTH || png.getHeight() > EPD_HEIGHT) {
        printf("Requested image is too large for the EPD.\n");
        printf("Image Size: %d x %d\nEPD size: %d x %d\n", png.getWidth(), png.getHeight(), EPD_WIDTH, EPD_HEIGHT);
        free(pData);
        return -1;
    }
    png.setBuffer((uint8_t *)malloc(png.getBufferSize()));
    rc = png.decode(NULL, 0);
    free(pData);
    return rc;
} /* DecodePNG() */
//
// Prepare the decoded image for the framebuffer layout of the EPD
// For 1-bit images, just memcpy since PNG uses the same layout.
// For 2-bit images, the EPD splits the memory into 2x 1-bit planes.
void PrepareImage(void)
{
	int x, y, iPlaneOffset, iSrcPitch, iDestPitch;
	uint8_t *s, *d;
	s = png.getBuffer();
	d = (uint8_t *)bbep.getBuffer();
	iDestPitch = (bbep.width()+7)/8;
	if (png.getBpp() == 1) {
	    iSrcPitch = (png.getWidth()+7)/8;
	    for (y=0; y<png.getHeight(); y++) {
		memcpy(d, s, iSrcPitch);
		s += iSrcPitch;
		d += iDestPitch;
	    }
	} else { // >=2 bpp
	    iPlaneOffset = iDestPitch * png.getHeight(); // offset to 2nd memory plane
	    iSrcPitch = (png.getWidth()+3)/4; // every source pixel depth will become 2-bpp
	    // Convert the source bitmap to 2-bit grayscale
	    ConvertBpp(s, png.getWidth(), png.getHeight(), png.getBpp(), png.getPalette());
	    for (y=0; y<png.getHeight(); y++) {
	    // Split the 2-bit packed pixels into 2 bit planes for the EPD
		for (x=0; x<png.getWidth()/4; x+=2) { // work with pairs of bytes
		    uint8_t s0 = ~s[x]; // grayscale is inverted on the EPD
		    uint8_t s1 = ~s[x+1];
		    uint8_t u8Mask = 0x80, d0=0, d1=0;
		    for (int bit=0; bit<4; bit++) {
		        if (s0 & u8Mask) d1 |= (0x80 >> bit);
		        if (s0 & (u8Mask>>1)) d0 |= (0x80 >> bit);
		        if (s1 & u8Mask) d1 |= (0x8 >> bit);
		        if (s1 & (u8Mask>>1)) d0 |= (0x8 >> bit);
		        u8Mask >>= 2; 
		    } // for each bit
		    d[x/2] = d0; // plane 0
		    d[(x/2) + iPlaneOffset] = d1; // plane 1
		} // for x
		s += iSrcPitch;
		d += iDestPitch;
	    } // for y
	} // >= 2bpp
	free(png.getBuffer());
} /* PrepareImage() */

void ShowHelp(void)
{
    printf("show_png utility - display PNG images on ePaper displays\nwritten by Larry Bank (bitbank@pobox.com)\nCopyright(c) 2025 BitBank Software, inc.\n");
    printf("Usage: show_png <filename.png> <display_mode> <optional panel profile>\nValid display modes: full, fast, partial\nValid profiles: (empty=A), B, C\n");
    printf("Color images and bit depths greater than 2-bpp will be\nautomatically converted to 2-bit (4 grays).\n");
} /* ShowHelp() */
//
// Main program entry point
//
int main(int argc, const char * argv[]) {
int rc, iMode;

    if (argc < 3 || argc > 4) { // print instructions
        ShowHelp();
        return -1;
    }
    if (strcasecmp(argv[2], "full") == 0) iMode = REFRESH_FULL;
    else if (strcasecmp(argv[2], "fast") == 0) iMode = REFRESH_FAST;
    else if (strcasecmp(argv[2], "partial") == 0) iMode = REFRESH_PARTIAL;
    else {
        printf("Invalid refresh mode.\n");
        ShowHelp();
        return -1;
    }
    if (argc == 4) { // user specified a profile
        uint8_t c = argv[3][0] & 0x5f; // force upper case
        if (c != 'A' && c != 'B' && c != 'C') {
            printf("Invalid profile.\n");
            ShowHelp();
            return -1;
        }
        iProfile = c - 'A';
    }
    if (PIN_PWR >= 0) {
        pinMode(PIN_PWR, OUTPUT);
        digitalWrite(PIN_PWR, 1); // enable power to EPD
    }
    bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, SPI_BUS, 0, 8000000);
#ifdef SHOW_DETAILS
    printf("Decoding PNG...\n");
#endif
    rc = DecodePNG(argv[1]);
    if (rc != PNG_SUCCESS) {
        if (rc > 0) {
            printf("PNG decode returned error: %s\n", szPNGErrors[rc]);
        }
        return -1;
    }
#ifdef SHOW_DETAILS
    printf("image specs: %d x %d, %d-bpp\n", png.getWidth(), png.getHeight(), png.getBpp());
#endif

    if (png.getBpp() == 1) {
        bbep.setPanelType(dpList[iProfile].OneBit);
        bbep.allocBuffer(); // draw into RAM first
        bbep.fillScreen(BBEP_WHITE);
    } else {
        bbep.setPanelType(dpList[iProfile].TwoBit);
        bbep.allocBuffer(); // draw into RAM first
        bbep.fillScreen(BBEP_GRAY3);
    }
    if (bbep.width() < bbep.height()) {
        bbep.setRotation(90); // assume landscape mode for all images
    }
    // convert+copy the image into the local EPD framebuffer
#ifdef SHOW_DETAILS
    printf("Preparing image for EPD as %d-bpp...\n", (bbep.getPanelType() == (int)dpList[iProfile].OneBit) ? 1 : 2);
#endif
    PrepareImage();
    // Push the pixels from our RAM buffer to the e-epaper
#ifdef SHOW_DETAILS
    printf("Writing data to EPD...\n");
#endif
    if (png.getBpp() == 1) {
        bbep.writePlane((iMode == REFRESH_PARTIAL) ? PLANE_FALSE_DIFF : PLANE_0);
        bbep.refresh(iMode);
    } else { // 4 gray mode
        bbep.writePlane();
        bbep.refresh(REFRESH_FULL);
    }
#ifdef SHOW_DETAILS
    printf("Refresh complete, shutting down...\n");
#endif
    bbep.sleep(DEEP_SLEEP); // turn off the epaper power circuit
    if (PIN_PWR > 0) {
        digitalWrite(PIN_PWR, 0); // disable power to EPD
    }
    return 0;
} /* main() */


