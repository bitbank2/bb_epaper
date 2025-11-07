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
#include <JPEGDEC.h>
#include "cJSON.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#define SHOW_DETAILS

BBEPAPER bbep;
int iAdapter, iMode;
int iPanel1Bit, iPanel2Bit;
int iInvert = 0; // assume not inverted

typedef struct tagAdapter
{
  uint8_t u8DC, u8RST, u8BUSY, u8CS, u8PWR, u8SPI;
} ADAPTER;
const char *szAdapters[] = {"framebuffer", "pimoroni", "waveshare_2", "waveshare_2_opi_rv2", NULL};
const char *szModes[] = {"full", "fast", "partial", NULL};
const char *szPanels[] = {
    "EP_PANEL_UNDEFINED","EP42_400x300","EP42B_400x300", // 0-2
    "EP213_122x250", "EP213B_122x250", "EP293_128x296", // 3-5
    "EP294_128x296", "EP295_128x296", "EP295_128x296_4GRAY", // 6-8
    "EP266_152x296", "EP102_80x128", "EP27B_176x264", // 9-11
    "EP29R_128x296", "EP122_192x176", "EP154R_152x152", // 12-14
    "EP42R_400x300", "EP42R2_400x300", "EP37_240x416", // 15-17
    "EP37B_240x416", "EP213_104x212", "EP75_800x480", // 18-20
    "EP75_800x480_GEN2", "EP75_800x480_4GRAY", "EP75_800x480_4GRAY_GEN2", // 21-23 
    "EP75_800x480_4GRAY_V2", "EP29_128x296", "EP29_128x296_4GRAY", // 24-26
    "EP213R_122x250", "EP154_200x200", "EP154B_200x200", // 27-29
    "EP266YR_184x360", "EP29YR_128x296", "EP29YR_168x384", // 30-32
    "EP583_648x480", "EP296_128x296", "EP26R_152x296", // 33-35
    "EP73_800x480", "EP73_SPECTRA_800x480", "EP74R_640x384", // 36-38
    "EP583R_600x448", "EP75R_800x480", "EP426_800x480", // 39-41
    "EP426_800x480_4GRAY", "EP29R2_128x296", "EP41_640x400", // 42-44
    "EP81_SPECTRA_1024x576", "EP7_960x640", "EP213R2_122x250", // 45-47
    "EP29Z_128x296", "EP29Z_128x296_4GRAY", "EP213Z_122x250", // 48-50
    "EP213Z_122x250_4GRAY", "EP154Z_152x152", "EP579_792x272", // 51-53
    "EP213YR_122x250", "EP37YR_240x416", "EP35YR_184x384", // 54-56
    "EP397YR_800x480", "EP154YR_200x200", "EP266YR2_184x360", // 57-59
    "EP42YR_400x300", "EP215YR_160x296", // 60-62
    NULL // must be last entry
};
// DC, RST, BUSY, CS, PWR
ADAPTER adapters[] = {{22, 27, 17, 8, 0xff, 0}, // Pimoroni
		       {25, 17, 24, 8, 18, 0}, // Waveshare 2.x
                       {49, 71, 92, 76, 70, 3}, // Waveshare 2.x on OPi RV2
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
JPEGDEC jpg;
int iWidth, iHeight, iBpp, iPixelType;
uint8_t *pBitmap, *pPalette=NULL;

const char *szPNGErrors[] = {"Success", "Invalid Parameter", "Decoding", "Out of memory", "No buffer allocated", "Unsupported feature", "Invalid file", "Too big", "Quit early"};
const char *szJPEGErrors[] = {"Success", "Invalid Parameter", "Decoding", "Unsupported feature", "Invalid file", "Out of memory"};

//
// Find the index value of a string within a list
// The list must be terminated with a NULL pointer
// Returns a value 0-N or -1 for not found
//
int FindItemName(const char **pList, const char *pName, const char *szLabel)
{
int i = 0;
    while (pList[i] != NULL && strcasecmp(pName, pList[i]) != 0) {
	    i++;
    }
    if (pList[i] == NULL) {
        printf("Invalid %s; must be one of: ", szLabel); 
        // Print the list of valid values
        i = 0;
        while (pList[i] != NULL) {
            printf("%s, ", pList[i]);
            i++; 
        }
        printf("\b\b  \n"); // erase the last comma
        return -1; // not found
    }
    return i;
} /* FindItemName() */
//
// Match the given pixel to black (00), white (01), or red (1x)
//
unsigned char GetBWRPixel(int r, int g, int b)
{
    uint8_t ucOut=BBEP_BLACK;
    int gr;

    gr = (b + r + g*2)>>2; // gray
    // match the color to closest of black/white/red
    if (r > g && r > b) { // red is dominant
        if (gr < 100 && r < 80) {
            // black
        } else {
            if (r-b > 32 && r-g > 32) {
                // is red really dominant?
                ucOut = BBEP_RED; // red (can be 2 or 3, but 3 is compatible w/BWYR)
            } else { // yellowish should be white
                // no, use white instead of pink/yellow
                ucOut = BBEP_WHITE;
            }
        }
    } else { // check for white/black
        if (gr >= 128) {
            ucOut = BBEP_WHITE; // white
        } else {
            // black
        }
    }
    return ucOut;
} /* GetBWRPixel() */
//
// Match the given pixel to black (00), white (01), yellow (10), or red (11)
// returns 2 bit value of closest matching color
//
unsigned char GetBWYRPixel(int r, int g, int b)
{
    uint8_t ucOut=BBEP_BLACK;
    int gr;

    gr = (b + r + g*2)>>2; // gray
    // match the color to closest of black/white/yellow/red
    if (r > b || g > b) { // red or yellow is dominant
        if (gr < 90 && r < 80 && g < 80) {
            // black
        } else {
            if (r-b > 32 && r-g > r/2) {
                // is red really dominant?
                ucOut = BBEP_RED; // red
            } else if (r-b > 32 && g-b > 32) {
                // yes, yellow
                ucOut = BBEP_YELLOW;
            } else {
                ucOut = BBEP_WHITE; // gray/white
            }
        }
    } else { // check for white/black
        if (gr >= 100) {
            ucOut = BBEP_WHITE; // white
        } else {
            // black
        }
    }
    return ucOut;
} /* GetBWYRPixel() */
//
// The user passed a file which has 2 or more bits per pixel
// convert it to 1 or 2-bpp grayscale
//
int ConvertBpp(uint8_t *pBMP, int w, int h, int iBpp, uint8_t *palette)
{
    int gray, r=0, g=0, b=0, x, y, iDelta, iPitch, iDestPitch, iDestBpp;
    uint8_t *s, *d, *pPal, u8, count;

    if (iPanel2Bit == -1) { // only 1 bit panel available
        iDestBpp = 1;
    } else {
        iDestBpp = 2;
    }
    if (iDestBpp == 1) {
        iDestPitch = (w+7)/8;
    } else {
        iDestPitch = (w+3)/4;
    }
    // The bits per pixel info from PNG files is per color channel
    // Convert the value into a true bits per pixel
    switch (iPixelType) {
        case PNG_PIXEL_INDEXED:
            break;
        case PNG_PIXEL_TRUECOLOR:
	    if (iBpp <= 8) {
                iBpp *= 3;
	    }
            palette = NULL;
            break;
        case PNG_PIXEL_TRUECOLOR_ALPHA:
	    if (iBpp <= 8) {
                iBpp *= 4;
	    }
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
                           s++;
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
            // 3 and 4-color epaper need the colors translated
            // through custom color tables because different panels use
            // different bit patterns to mean different things.
            if (bbep.capabilities() & BBEP_3COLOR) { // B/W/R
                bbep.drawPixel(x, y, GetBWRPixel(r, g, b));
            } else if (bbep.capabilities() & BBEP_4COLOR) { // B/W/Y/R
                bbep.drawPixel(x, y, GetBWYRPixel(r, g, b));
            } else { // Assume 1 or 2 bit grayscale
                // Convert the source rgb into gray with a simple formula which favors green
                gray = (r + g*2 + b)/4;
                u8 |= gray >> (8-iDestBpp); // pack 1 or 2 bit gray pixels into a destination byte
            }
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
    return iDestBpp;
} /* ConvertBpp() */
//
// Decode the BMP file
//
int DecodeBMP(uint8_t *pData, int iSize)
{
    int iOffBits; // offset to bitmap data
    int y, iDestPitch=0, iPitch;
    uint8_t bFlipped = 0;
    uint8_t *s, *d;

    iWidth = *(int16_t *)&pData[18];
    iHeight = *(int16_t *)&pData[22];
    if (iHeight < 0) {
        iHeight = -iHeight;
    } else {
	bFlipped = 1;
    }
    iBpp = *(int16_t *)&pData[28];
    iOffBits = *(uint16_t *)&pData[10];
    switch (iBpp) {
        case 1:
	    iDestPitch = ((iWidth+7)>>3);
	    iPixelType = PNG_PIXEL_INDEXED;
            break;
	case 4:
	    iDestPitch = ((iWidth+1)>>1);
	    iPixelType = PNG_PIXEL_INDEXED;
	    break;
	case 8:
	    iDestPitch = iWidth;
	    iPixelType = PNG_PIXEL_INDEXED;
	    break;
	case 24:
	    iDestPitch = iWidth*3;
	    iPixelType = PNG_PIXEL_TRUECOLOR;
	    break;
	case 32:
	    iDestPitch = iWidth*4;
	    iPixelType = PNG_PIXEL_TRUECOLOR_ALPHA;
	    break;
    } // switch on bpp
    iPitch = (iDestPitch + 3) & 0xfffc; // must be DWORD aligned
    if (bFlipped)
    {
        iOffBits += ((iHeight-1) * iPitch); // start from bottom
        iPitch = -iPitch;
    }
    pBitmap = (uint8_t *)malloc(iHeight * iDestPitch);
    s = &pData[iOffBits];
    d = pBitmap;
    for (y=0; y<iHeight; y++) { // copy the bitmap to the common format
        memcpy(d, s, iDestPitch);
	s += iPitch;
	d += iDestPitch;
    }
    // Adjust the palette for 3-byte entries (if there is one)
    if (iBpp <= 8) {
	int iColors = 1<<iBpp;
	d = pPalette = pData;
        iOffBits = *(uint16_t *)&pData[10];
        s = &pData[iOffBits - (4 * iColors)];
        for (y=0; y<iColors; y++) {
            d[0] = s[0]; d[1] = s[1]; d[2] = s[2];
	    s += 4;
	    d += 3;
	}
    } else {
        pPalette = NULL;
    }
    return PNG_SUCCESS; // re-use this return code
} /* DecodeBMP() */
//
// Decode the JPEG file into an uncompressed bitmap
//
int DecodeJPEG(uint8_t *pData, int iSize)
{
int rc, iPitch;
    rc = jpg.openRAM(pData, iSize, NULL);
    if (!rc) {
        rc = jpg.getLastError();
        printf("JPEG open returned error: %s\n", szJPEGErrors[rc]);
        return -1; // only show the error once
    }
    if (iAdapter !=0 && (jpg.getWidth() > bbep.width() || jpg.getHeight() > bbep.height())) {
        printf("Requested image is too large for the EPD.\n");
        printf("Image Size: %d x %d\nEPD size: %d x %d\n", png.getWidth(), png.getHeight(), bbep.width(), bbep.height());
        return -1;
    }
    iWidth = jpg.getWidth();
    iHeight = jpg.getHeight();
    iBpp = jpg.getBpp();
    if (iBpp == 8) {
        iPixelType = PNG_PIXEL_GRAYSCALE;
        jpg.setPixelType(EIGHT_BIT_GRAYSCALE);
        iPitch = iWidth;
    } else {
        iPixelType = PNG_PIXEL_TRUECOLOR_ALPHA;
        jpg.setPixelType(RGB8888);
        iPitch = iWidth*4;
        iBpp = 32; // output is 32-bits
    }
    pBitmap = (uint8_t *)malloc(iPitch * (iHeight+15));
    jpg.setFramebuffer(pBitmap);
    jpg.decode(0, 0, 0);
    return jpg.getLastError();
} /* DecodeJPEG() */

//
// Decode the PNG file into an uncompressed bitmap
//
int DecodePNG(uint8_t *pData, int iSize)
{
int rc;
    rc = png.openRAM(pData, iSize, NULL);
    if (rc != PNG_SUCCESS) {
        printf("PNG open returned error: %s\n", szPNGErrors[rc]);
        return -1; // only show the error once
    }
    if (iAdapter != 0 && (png.getWidth() > bbep.width() || png.getHeight() > bbep.height())) {
        printf("Requested image is too large for the EPD.\n");
        printf("Image Size: %d x %d\nEPD size: %d x %d\n", png.getWidth(), png.getHeight(), bbep.width(), bbep.height());
        return -1;
    }
    iWidth = png.getWidth();
    iHeight = png.getHeight();
    iBpp = png.getBpp();
    pPalette = png.getPalette();
    iPixelType = png.getPixelType();
    if (iPixelType != PNG_PIXEL_INDEXED) pPalette = NULL; // tell other code that there's no palette present
    pBitmap = (uint8_t *)malloc(png.getBufferSize());
    png.setBuffer(pBitmap);
    rc = png.decode(NULL, 0);
    return rc;
} /* DecodePNG() */
//
// Prepare the decoded image for the internal memory layout of the EPD
// For 1-bit images, just memcpy since PNG uses the same layout.
// For 2-bit images, the EPD splits the memory into 2x 1-bit planes.
void PrepareImage(void)
{
	int x, y, iPlaneOffset, iSrcPitch, iDestPitch;
	uint8_t *s, *d, uc=0;
	s = pBitmap;
	d = (uint8_t *)bbep.getBuffer();
	iDestPitch = (bbep.width()+7)/8;
        // Convert the source bitmap to 1 or 2-bit grayscale
        if (iBpp >= 2) {
            iBpp = ConvertBpp(s, iWidth, iHeight, iBpp, pPalette);
        }
	if (iBpp == 1) {
	    iSrcPitch = (iWidth+7)/8;
	    for (y=0; y<iHeight; y++) {
                if (bbep.capabilities() & BBEP_4COLOR) {
                // memory layout is different
                    for (x=0; x<iWidth; x++) {
                        if ((x & 7) == 0) uc = s[x>>3];
                        if (!(uc & 0x80)) bbep.drawPixel(x, y, BBEP_BLACK); // background is already white
                        uc <<= 1;
                    }
                } else {
		    memcpy(d, s, iSrcPitch);
		    if (iWidth & 7) { // fill partial byte with white
                        d[iWidth>>3] |= (0xff >> (iWidth & 7));
		    }
                }
		s += iSrcPitch;
		d += iDestPitch;
	    }
	} else if (!(bbep.capabilities() & (BBEP_3COLOR | BBEP_4COLOR))) { // >=2 bpp
	    iPlaneOffset = iDestPitch * iHeight; // offset to 2nd memory plane
	    iSrcPitch = (iWidth+3)/4; // every source pixel depth will become 2-bpp
	    for (y=0; y<iHeight; y++) {
	    // Split the 2-bit packed pixels into 2 bit planes for the EPD
		for (x=0; x<iWidth/4; x+=2) { // work with pairs of bytes
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
	free(pBitmap);
} /* PrepareImage() */
//
// Draw the current image on the Linux framebuffer
//
void UpdateFramebuffer(void)
{
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
uint8_t *fbp;
int x, y, iScreenWidth, iScreenHeight, iNewWidth, iNewHeight;
int fbfd;
uint32_t u32Frac, u32AccX, u32AccY; // fractional scaling
int iCenterX, iCenterY;
uint8_t ucTemp[768]; // temporary palette for grayscale

    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error opening framebuffer device; try running as sudo\n");
        return;
    }
    // Get the fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
       printf("Error reading the framebuffer fixed information.\n");
       close(fbfd);
       return;
    }
      // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
       printf("Error reading the framebuffer variable information.\n");
       close(fbfd);
       return;
    }
    printf("%dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
    // Map framebuffer to user memory
    iScreenWidth = vinfo.xres;
    iScreenHeight = vinfo.yres;
    // Calculate the image scale
    u32AccX = (iWidth << 16) / iScreenWidth;
    u32AccY = (iHeight << 16) / iScreenHeight;
    u32Frac = u32AccX;
    if (u32AccY > u32AccX) u32Frac = u32AccY; // aspect-fit
    iNewHeight = (iHeight << 16) / u32Frac;
    iNewWidth = (iWidth << 16) / u32Frac;
    iCenterX = (iScreenWidth - iNewWidth)/2;
    iCenterY = (iScreenHeight - iNewHeight)/2;
    fbp = (uint8_t*)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((intptr_t)fbp == -1) {
       printf("Failed to mmap the framebuffer.\n");
       close(fbfd);
       return;
    }
    if (!pPalette) { // create a grayscale palette if needed
       int iDelta, iCount = 1<<iBpp;
       int iGray=0;
       iDelta = 255/(iCount-1);
       for (x=0; x<iCount; x++) {
          ucTemp[x*3] = (uint8_t)iGray;
          ucTemp[x*3+1] = (uint8_t)iGray;
          ucTemp[x*3+2] = (uint8_t)iGray;
          iGray += iDelta;
       }
       pPalette = ucTemp;
    } else {
    }
    // Clear the display to black
    //memset(fbp, 0, finfo.smem_len);
    if (vinfo.bits_per_pixel == 16) {
        uint16_t *d, u16, r, g, b;
        uint8_t *s;
        int iSrcPitch, tx;
        iSrcPitch = (iWidth * iBpp)/8;
        u32AccY = 0;
        for (y=0; y<iNewHeight; y++) {
            u32AccX = 0;
            s = pBitmap + ((u32AccY >> 16)*iSrcPitch);
            d = (uint16_t *)fbp;
            d += (y+iCenterY) * iScreenWidth;
            d += iCenterX;
            switch(iBpp) {
                case 1:
                {
                uint8_t uc;
                tx = 0;
                uc = *s++;
                for (x=0; x<iNewWidth; x++) {
                    if (uc & 0x80) {
                       r = pPalette[3];
                       g = pPalette[4];
                       b = pPalette[5];
                    } else {
                       r = pPalette[0];
                       g = pPalette[1];
                       b = pPalette[2];
                    }
                    *d++ = ((r & 0xf8)<<8) | ((g & 0xfc) << 3) | (b >> 3);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        uc <<= 1;
                        tx++;
                        if ((tx & 7) == 0) uc = *s++;
                    }
                } // for x
                }
                break;
                case 2:
                {
                uint8_t c, uc;
                tx = 0;
                uc = *s++;
                for (x=0; x<iNewWidth; x++) {
                    c = uc >> 6;
                    r = pPalette[c*3];
                    g = pPalette[c*3+1];
                    b = pPalette[c*3+2];
                    *d++ = ((r & 0xf8)<<8) | ((g & 0xfc) << 3) | (b >> 3);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        uc <<= 2;
                        tx++;
                        if ((tx & 3) == 0) uc = *s++;
                    }
                } // for x
                }
                break;
                case 4:
                {
                uint8_t c, uc;
                tx = 0;
                uc = *s++;
                for (x=0; x<iNewWidth; x++) {
                    c = uc >> 4;
                    r = pPalette[c*3];
                    g = pPalette[c*3+1];
                    b = pPalette[c*3+2];
                    *d++ = ((r & 0xf8)<<8) | ((g & 0xfc) << 3) | (b >> 3);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        uc <<= 4;
                        tx++;
                        if ((tx & 1) == 0) uc = *s++;
                    }
                } // for x
                }
                break;
                case 8:
                {
                uint8_t uc;
                tx = 0;
                uc = *s++;
                for (x=0; x<iNewWidth; x++) {
                    r = pPalette[uc*3];
                    g = pPalette[uc*3+1];
                    b = pPalette[uc*3+2];
                    *d++ = ((r & 0xf8)<<8) | ((g & 0xfc) << 3) | (b >> 3);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        tx++;
                        uc = *s++;
                    }
                } // for x
                }
                break;
                case 24:
                case 32:
                {
                for (x=0; x<iNewWidth; x++) {
                    u16 = (s[2] & 0xf8)<<8; // R
                    u16 |= (s[1] & 0xfc) << 3; // G
                    u16 |= (s[0] >> 3); // B
                    *d++ = u16; 
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        s += (iBpp/8);
                    }
                } // for x
                }
                break;
            } // switch on bpp
            u32AccY += u32Frac;
        } // for y
    } else if (vinfo.bits_per_pixel == 32) {
        uint32_t *d, r, g, b;
        uint8_t *s;
        int iSrcPitch, tx;
        iSrcPitch = (iWidth * iBpp)/8;
        u32AccY = 0;
        for (y=0; y<iNewHeight; y++) {
            u32AccX = 0;
            s = pBitmap + ((u32AccY >> 16)*iSrcPitch);
            d = (uint32_t *)fbp;
            d += (y+iCenterY) * iScreenWidth;
            d += iCenterX;
            switch(iBpp) {
                case 1:
                {
                uint8_t uc;
                tx = 0;
                uc = *s++;
                for (x=0; x<iNewWidth; x++) {
                    if (uc & 0x80) {
                       r = pPalette[3];
                       g = pPalette[4];
                       b = pPalette[5];
                    } else {
                       r = pPalette[0];
                       g = pPalette[1];
                       b = pPalette[2];
                    }
                    *d++ = 0xff000000 | r | (g << 8) | (b << 16);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        uc <<= 1;
                        tx++;
                        if ((tx & 7) == 0) uc = *s++;
                    }
                } // for x
                }
                break;
                case 2:
                {
                uint8_t c, uc;
                tx = 0;
                uc = *s++;
                for (x=0; x<iNewWidth; x++) {
                    c = uc >> 6;
                    r = pPalette[c*3];
                    g = pPalette[c*3+1];
                    b = pPalette[c*3+2];
                    *d++ = 0xff000000 | r | (g << 8) | (b << 16);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        uc <<= 2;
                        tx++;
                        if ((tx & 3) == 0) uc = *s++;
                    }
                } // for x
                }
                break;
                case 4:
                {
                uint8_t c, uc;
                tx = 0;
                uc = *s++;
                for (x=0; x<iNewWidth; x++) {
                    c = uc >> 4;
                    r = pPalette[c*3];
                    g = pPalette[c*3+1];
                    b = pPalette[c*3+2];
                    *d++ = 0xff000000 | r | (g << 8) | (b << 16);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        uc <<= 4;
                        tx++;
                        if ((tx & 1) == 0) uc = *s++;
                    }
                } // for x
                }
                break;
                case 8:
                {
                uint8_t uc;
                tx = 0;
                uc = *s++;
                for (x=0; x<iNewWidth; x++) {
                    r = pPalette[uc*3];
                    g = pPalette[uc*3+1];
                    b = pPalette[uc*3+2];
                    *d++ = 0xff000000 | r | (g << 8) | (b << 16);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        tx++;
                        uc = *s++;
                    }
                } // for x
                }
                break;
                case 24:
                case 32:
                {
                for (x=0; x<iNewWidth; x++) {
                    *d++ = 0xff000000 | s[0] | (s[1] << 8) | (s[2] << 16);
                    u32AccX += u32Frac;
                    if (u32AccX >= 65536) {
                        u32AccX -= 65536;
                        s += (iBpp/8);
                    }
                } // for x
                }
                break;
            } // switch on bpp
            u32AccY += u32Frac;
        } // for y
    }
    // Cleanup
    munmap(fbp, finfo.smem_len);
    close(fbfd);
} /* UpdateFramebuffer() */

void ShowHelp(void)
{
    printf("show_png utility - display PNG (and BMP) images on ePaper displays\nwritten by Larry Bank (bitbank@pobox.com)\nCopyright(c) 2025 BitBank Software, inc.\n");
    printf("A JSON file in the current directory (epaper.json) can contain the setup\nor the parameters can be passed on the command line (in any order):\n");
    printf("file=<filename> any PNG or BMP file\nmode=<update mode> can be full, fast or partial\nadapter=<epaper PCB> can be waveshare_2 or pimoroni\npanel_1bit=<bb_epaper panel name>\npanel_2bit=<bb_epaper panel name>\n");
    printf("Color images and bit depths greater than 2-bpp will be\nautomatically converted to 2-bit (4 grays).\n");
    printf("example: ./show_png file=\"/home/me/test.png\" mode=fast panel_1bit=EP75_800x480 adapter=waveshare_2\n");
} /* ShowHelp() */
//
// Main program entry point
//
int main(int argc, const char * argv[]) {
int rc, iSize;
FILE *ihandle;
uint8_t *pData;
char szJSON[256]; // current dir
cJSON *pJSON, *pItem;
char szFile[256];

    iAdapter = iPanel1Bit = iPanel2Bit = iMode = -1;
    szFile[0] = 0;

    getcwd(szJSON, sizeof(szJSON));
    strcat(szJSON, "/epaper.json"); // name of local config file
    //printf("config name: %s\n", szJSON);
    ihandle = fopen(szJSON, "r+b");
    if (ihandle) {
#ifdef SHOW_DETAILS
	    printf("epaper.json found!\n");
#endif
	    fseek(ihandle, 0, SEEK_END);
            iSize = (int)ftell(ihandle);
	    fseek(ihandle, 0, SEEK_SET);
	    pData = (uint8_t *)malloc(iSize);
	    fread(pData, 1, iSize, ihandle);
	    fclose(ihandle);
	    pJSON = cJSON_ParseWithLength((const char *)pData, iSize);
	    if (pJSON) {
#ifdef SHOW_DETAILS
		    printf("JSON parsed successfully!\n");
#endif
                    if (cJSON_HasObjectItem(pJSON, "invert")) {
                         pItem = cJSON_GetObjectItem(pJSON, "invert");
                         iInvert = !strcmp(pItem->valuestring, "true");
#ifdef SHOW_DETAILS
                         printf("invert = %s\n", (iInvert) ? "true" : "false");
#endif
                    }
		    if (cJSON_HasObjectItem(pJSON, "adapter")) {
                         pItem = cJSON_GetObjectItem(pJSON, "adapter");
			 iAdapter = FindItemName(szAdapters, pItem->valuestring, "adapter");
                         if (iAdapter >= 0) {
#ifdef SHOW_DETAILS
                             printf("Adapter = %d\n", iAdapter);
#endif
                         }
		    }
		    if (cJSON_HasObjectItem(pJSON, "panel_1bit")) {
		         pItem = cJSON_GetObjectItem(pJSON, "panel_1bit");
			 iPanel1Bit = FindItemName(szPanels, pItem->valuestring, "1-bit panel");
                         if (iPanel1Bit >= 0) {
#ifdef SHOW_DETAILS
                             printf("panel1bit = %d\n", iPanel1Bit);
#endif
                         }
		    }
		    if (cJSON_HasObjectItem(pJSON, "panel_2bit")) {
			 pItem = cJSON_GetObjectItem(pJSON, "panel_2bit");
			 iPanel2Bit = FindItemName(szPanels, pItem->valuestring, "2-bit panel");
                         if (iPanel2Bit >= 0) {
#ifdef SHOW_DETAILS          
                             printf("panel2bit = %d\n", iPanel2Bit);
#endif
                         }
		    }
		    if (cJSON_HasObjectItem(pJSON, "mode")) {
			 pItem = cJSON_GetObjectItem(pJSON, "mode");
			 iMode = FindItemName(szModes, pItem->valuestring, "update mode");
                         if (iMode >= 0) {
#ifdef SHOW_DETAILS
			     printf("mode = %d\n", iMode);
#endif
                         }
		    }
		    if (cJSON_HasObjectItem(pJSON, "file")) {
                         pItem = cJSON_GetObjectItem(pJSON, "file");
			 strcpy(szFile, pItem->valuestring);
		    }
		    cJSON_Delete(pJSON);
	    } else {
		    printf("Error parsing JSON!\n");
	    }
	    free(pData);
    } // if epaper.json file exists

    for (int i=1; i<argc; i++) {
        char *pName, *pValue, *saveptr;
        pName = strtok_r((char *)argv[i], "=", &saveptr);
	pValue = strtok_r(NULL, "=", &saveptr);
	printf("%d: %s %s\n", i, pName, pValue); 
        if (strcmp(pName, "mode") == 0) {
		iMode = FindItemName(szModes, pValue, "update mode");
	} else if (strcmp(pName, "file") == 0) {
		strcpy(szFile, pValue);
	} else if (strcmp(pName, "panel_1bit") == 0) {
		iPanel1Bit = FindItemName(szPanels, pValue, "1-bit panel");
	} else if (strcmp(pName, "panel_2bit") == 0) {
		iPanel2Bit = FindItemName(szPanels, pValue, "2-bit panel");
	} else if (strcmp(pName, "adapter") == 0) {
		iAdapter = FindItemName(szAdapters, pValue, "adapter");
	} else if (strcmp(pName, "invert") == 0) {
                iInvert = !strcmp(pValue, "true");
        }
    }

    if (szFile[0] == 0 || iAdapter == -1 || iMode == -1 || (iPanel1Bit == -1 && iPanel2Bit == -1)) { // print instructions
        ShowHelp();
        return -1;
    }

    if (iAdapter == 0) { // framebuffer
    } else {
	    if (adapters[iAdapter].u8PWR != 0xff) {
		pinMode(adapters[iAdapter].u8PWR, OUTPUT);
		digitalWrite(adapters[iAdapter].u8PWR, 1); // enable power to EPD
	    }
	    // Make sure SPI is enabled; if not, we can enable it from here
	    // (at least on Raspberry Pi SBCs)
	    {
		DIR *pDir;
		struct dirent *pDE;
		int bFound = 0;

		pDir = opendir("/dev");
		if (!pDir) {
		    printf("Error searching /dev directory; try running as sudo. Aborting...\n");
		    return -1;
		}
		// Search all names for "spidev"
		while ((pDE = readdir(pDir)) != NULL) {
		    if (memcmp(pDE->d_name, "spidev", 6) == 0) { // found one!
		        bFound = 1;
		        break;
		    }
		} // while searching
		if (!bFound) { // SPI is disabled, enable it
		    printf("Enabling the SPI bus...\n");
		    system("sudo dtparam spi=on");
		    usleep(1000000); // allow time for it to start
		}
	    }
	    // This MUST be set before initializing the I/O so that the initial
	    // command sequence is sent to properly prepare the EPD for receiving data
	    bbep.setPanelType((iPanel1Bit == -1) ? iPanel2Bit : iPanel1Bit);
	    bbep.initIO(adapters[iAdapter].u8DC, adapters[iAdapter].u8RST, adapters[iAdapter].u8BUSY, adapters[iAdapter].u8CS, adapters[iAdapter].u8SPI, 0, 8000000);
	    if (bbep.width() < bbep.height()) {
		    bbep.setRotation(270);
	    }
    } // use epaper as output
#ifdef SHOW_DETAILS
    printf("Decoding image...\n");
#endif

    // Read the file into RAM
    ihandle = fopen(szFile, "r+b");
    if (ihandle == NULL) {
        printf("Error opening file %s\n", szFile);
        return -1;
    }
    fseek(ihandle, 0, SEEK_END);
    iSize = (int)ftell(ihandle);
    fseek(ihandle, 0, SEEK_SET);
    pData = (uint8_t *)malloc(iSize);
    fread(pData, 1, iSize, ihandle);
    fclose(ihandle);
    if (iSize < 64) { // invalid file
        printf("Invalid image file\n");
	return -1;
    }
    if (pData[0] == 'B' && pData[1] == 'M') { // it's a BMP file
        rc = DecodeBMP(pData, iSize);
    } else if (pData[0] == 0xff && pData[1] == 0xd8) { // JPEG
        rc = DecodeJPEG(pData, iSize);
        if (rc != JPEG_SUCCESS) {
            if (rc > 0) {
                printf("JPEG decode returned error: %s\n", szJPEGErrors[rc]);
            }
            return -1;
        }
    } else {
        rc = DecodePNG(pData, iSize);
        if (rc != PNG_SUCCESS) {
            if (rc > 0) {
                printf("PNG decode returned error: %s\n", szPNGErrors[rc]);
            }
            return -1;
        }
    }
#ifdef SHOW_DETAILS
    printf("image specs: %d x %d, %d-bpp\n", iWidth, iHeight, iBpp);
#endif

    if (iAdapter == 0) { // framebuffer
        UpdateFramebuffer();
    } else { // e-paper
        if (iBpp == 1 || iPanel2Bit == -1) {
            bbep.allocBuffer(); // draw into RAM first
            bbep.fillScreen(BBEP_WHITE);
        } else {
            bbep.setPanelType(iPanel2Bit);
            bbep.allocBuffer(); // draw into RAM first
            if (bbep.capabilities() & (BBEP_3COLOR | BBEP_4COLOR)) {
                bbep.fillScreen(BBEP_WHITE);
            } else {
                bbep.fillScreen(BBEP_GRAY3);
            }
        }
        if (bbep.width() < bbep.height()) {
            bbep.setRotation(270); // assume landscape mode for all images
        }
        // convert+copy the image into the local EPD framebuffer
#ifdef SHOW_DETAILS
        printf("Preparing image for EPD as %d-bpp...\n", (bbep.getPanelType() == iPanel1Bit) ? 1 : 2);
#endif
        PrepareImage();
    // Push the pixels from our RAM buffer to the e-epaper
#ifdef SHOW_DETAILS
        printf("Writing data to EPD...\n");
#endif
        if (iBpp == 1 && bbep.getPanelType() == iPanel1Bit) {
            bbep.writePlane((iMode == REFRESH_PARTIAL) ? PLANE_FALSE_DIFF : PLANE_0, iInvert);
            bbep.refresh(iMode);
        } else { // 3-color, 4-color, or 4 gray mode
            bbep.writePlane(PLANE_BOTH, iInvert);
            bbep.refresh(iMode); // some 4-color panels support fast update
        }
#ifdef SHOW_DETAILS
        printf("Refresh complete, shutting down...\n");
#endif
        bbep.sleep(DEEP_SLEEP); // turn off the epaper power circuit
        if (adapters[iAdapter].u8PWR != 0xff) {
            digitalWrite(adapters[iAdapter].u8PWR, 0); // disable power to EPD
        }
    } // e-paper update
    return 0;
} /* main() */


