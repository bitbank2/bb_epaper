//
// PNG to Group5 Image Compressor tool
// Written by Larry Bank
// Copyright (c) 2025 BitBank Software, Inc.
//
#include <stdio.h>
#define MAX_IMAGE_FLIPS 256
#include "../src/Group5.h"
#include "../src/g5enc.inl"
#include "PNGdec.h"

PNG png; // static instance of class
uint8_t u8Temp[1024];

enum {
    MODE_BW = 0,
    MODE_BWR,
    MODE_BWYR,
    MODE_4GRAY
};
//
// Create the comments and const array boilerplate for the hex data bytes
//
void StartHexFile(FILE *f, int iLen, int w, int h, const char *fname, int iMode)
{
    int i;
    char szTemp[256];
    fprintf(f, "//\n// Created with imageconvert, written by Larry Bank\n");
    if (iMode == MODE_BW) {
        fprintf(f, "// %d x %d x 1-bit per pixel\n", w, h);
    } else {
        fprintf(f, "// %d x %d x 2-bits per pixel (split into 2 1-bit planes)\n", w, h);
    }
    fprintf(f, "// compressed image data size = %d bytes\n//\n", iLen);
    strcpy(szTemp, fname);
    i = strlen(szTemp);
    if (szTemp[i-2] == '.') szTemp[i-2] = 0; // get the leaf name for the data
    fprintf(f, "const uint8_t %s[] = {\n", szTemp);
} /* StartHexFile() */
//
// Add N bytes of hex data to the output
// The data will be arranged in rows of 16 bytes each
//
void AddHexBytes(FILE *f, void *pData, int iLen, int bLast)
{
    static int iCount = 0; // number of bytes processed so far
    int i;
    uint8_t *s = (uint8_t *)pData;
    for (i=0; i<iLen; i++) { // process the given data
        fprintf(f, "0x%02x", *s++);
        iCount++;
        if (i < iLen-1 || !bLast) fprintf(f, ",");
        if ((iCount & 15) == 0) fprintf(f, "\n"); // next row of 16
    }
    if (bLast) {
        fprintf(f, "};\n");
    }
} /* AddHexBytes() */
//
// Match the given pixel to black (00), white (01), or red (1x)
//
unsigned char GetBWRPixel(int r, int g, int b)
{
    uint8_t ucOut=0;
    int gr;
    
    gr = (b + r + g*2)>>2; // gray
    // match the color to closest of black/white/red
    if (r > g && r > b) { // red is dominant
        if (gr < 100 && r < 80) {
            // black
        } else {
            if (r-b > 32 && r-g > 32) {
                // is red really dominant?
                ucOut |= 3; // red (can be 2 or 3, but 3 is compatible w/BWYR)
            } else { // yellowish should be white
                // no, use white instead of pink/yellow
                ucOut |= 1;
            }
        }
    } else { // check for white/black
        if (gr >= 100) {
            ucOut |= 1; // white
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
    uint8_t ucOut=0;
    int gr;

    gr = (b + r + g*2)>>2; // gray
    // match the color to closest of black/white/yellow/red
    if (r > b || g > b) { // red or yellow is dominant
        if (gr < 90 && r < 80 && g < 80) {
            // black
        } else {
            if (r-b > 32 && r-g > 32) {
                // is red really dominant?
                ucOut = 3; // red
            } else if (r-b > 32 && g-b > 32) {
                // yes, yellow
                ucOut = 2;
            } else {
                ucOut = 1; // gray/white
            }
        }
    } else { // check for white/black
        if (gr >= 100) {
            ucOut = 1; // white
        } else {
            // black
        }
    }
    return ucOut;
} /* GetBWYRPixel() */
//
// The user passed a file with the wrong bit depth (not 1)
// convert to 1-bpp by converting each color to 0 or 1 based on the gray level
//
void ConvertBpp(uint8_t *pBMP, int iMode, int w, int h, int iBpp, uint8_t *palette)
{
    int gray, r, g, b, x, y, iDelta, iPitch, iDestPitch, iDestBpp;
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
            } else if (iMode == MODE_BWR) {
                u8 |= GetBWRPixel(r, g, b); // match the closest colors
            } else {
                u8 |= GetBWYRPixel(r, g, b);
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

int main(int argc, const char * argv[]) {
    int w, h, y, rc, bpp;
    uint8_t *s, *pOut, *pData;
    int iPitch, iOutSize, iDataSize, iMode;
    FILE *ihandle;
    uint8_t *pPalette;
    G5ENCIMAGE g5enc;
    BB_BITMAP bbbm;
    int bHFile; // flag indicating if the output will be a .H file of hex data
    const char *szModes[] = {"BW", "BWR", "BWYR", "4GRAY", NULL};
    
    printf("PNG to Group5 image conversion tool\n");
    if (argc != 4) {
        printf("Usage: ./pngconvert <PNG image> <g5 compressed image> <mode>\n");
        printf("valid modes: BW, BWR, BWYR, 4GRAY (case insensitive)\n");
        return -1;
    }
    iMode = 0;
    while (szModes[iMode] && strcasecmp(szModes[iMode], argv[3]) != 0) {
        iMode++;
    }
    if (szModes[iMode] == NULL) {
        printf("Invalid mode\n");
        return -1;
    }
    pOut = (uint8_t *)argv[2] + strlen(argv[2]) - 1;
    bHFile = (pOut[0] == 'H' || pOut[0] == 'h'); // output an H file?
    ihandle = fopen(argv[1],"rb"); // open input file
    if (ihandle == NULL)
    {
        fprintf(stderr, "Unable to open file: %s\n", argv[1]);
        return -1; // bad filename passed
    }
    fseek(ihandle, 0L, SEEK_END); // get the file size
    iDataSize = (int)ftell(ihandle);
    fseek(ihandle, 0, SEEK_SET);
    pData = (uint8_t *)malloc(iDataSize);
    fread(pData, 1, iDataSize, ihandle);
    fclose(ihandle);
    
    rc = png.openRAM(pData, iDataSize, NULL);
    if (rc == PNG_SUCCESS) {
        w = png.getWidth();
        h = png.getHeight();
        printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", w, h, png.getBpp(), png.getPixelType());
        png.setBuffer((uint8_t *)malloc(png.getBufferSize()));
        rc = png.decode(NULL, 0); //PNG_CHECK_CRC);
        png.close();
        if (rc != PNG_SUCCESS) {
             printf("Error decoding PNG file = %d\n", rc);
             free(png.getBuffer());
             return -1;
        }
        pPalette = NULL;
        switch (png.getPixelType()) {
            case PNG_PIXEL_INDEXED:
            case PNG_PIXEL_GRAYSCALE:
                if (png.getPixelType() == PNG_PIXEL_INDEXED) {
                    pPalette = png.getPalette();
                }
                bpp = png.getBpp();
                break;
            case PNG_PIXEL_TRUECOLOR:
                bpp = 24;
                break;
            case PNG_PIXEL_TRUECOLOR_ALPHA:
                bpp = 32;
                break;
        } // switch
        if (iMode != MODE_BW || (iMode == MODE_BW && bpp != 1)) { // need to convert it to 1 or 2-bpp
            printf("Converting pixels to %s\n", szModes[iMode]);
            ConvertBpp(png.getBuffer(), iMode, w, h, bpp, pPalette);
        }
        iPitch = (w+7) >> 3;
        pOut = (uint8_t *)malloc(iPitch * h * 2);
        if (iMode == MODE_BW) {
            s = png.getBuffer();
            rc = g5_encode_init(&g5enc, w, h, pOut, iPitch * h);
            for (y=0; y<h && rc == G5_SUCCESS; y++) {
                rc = g5_encode_encodeLine(&g5enc, s);
                s += iPitch;
            }
        } else { // split the bit planes
            uint8_t *d, src, ucMask, uc = 0;
            s = png.getBuffer();
            rc = g5_encode_init(&g5enc, w, h*2, pOut, iPitch * h * 2);
            // encode plane 0 first
            for (y=0; y<h*2 && rc == G5_SUCCESS; y++) {
                if (y == h) {
                    s = png.getBuffer(); // restart from the top
                }
                src = *s++;
                d = u8Temp;
                ucMask = (y < h) ? 0x40 : 0x80; // lower or upper source bit
                for (int x=0; x<w; x++) {
                    uc <<= 1;
                    if (src & ucMask) {
                        uc |= 1;
                    }
                    src <<= 2;
                    if ((x & 3) == 3 && x != w-1) { // new input byte
                        src = *s++;
                    }
                    if ((x & 7) == 7) { // new output byte
                        *d++ = uc;
                    }
                } // for x
                *d = uc; // store last partial byte
                rc = g5_encode_encodeLine(&g5enc, u8Temp);
            }
        }
    if (rc == G5_ENCODE_COMPLETE) {
        FILE *f;
        iOutSize = g5_encode_getOutSize(&g5enc);
        if (iMode != MODE_BW) iPitch *= 2;
        printf("Input data size:  %d bytes, compressed size: %d bytes\n", iPitch*h, iOutSize);
        printf("Compression ratio: %2.1f:1\n", (float)(iPitch*h) / (float)iOutSize);
        bbbm.u16Marker = (iMode == MODE_BW) ? BB_BITMAP_MARKER : BB_BITMAP2_MARKER;
        bbbm.width = w;
        bbbm.height = h;
        bbbm.size = iOutSize;
        f = fopen(argv[2], "w+b");
        if (!f) {
            printf("Error opening: %s\n", argv[2]);
        } else {
            if (bHFile) { // generate HEX file to include in a project
                StartHexFile(f, iOutSize+sizeof(BB_BITMAP), w, h, argv[2], iMode);
                AddHexBytes(f, &bbbm, sizeof(BB_BITMAP), 0);
                AddHexBytes(f, pOut, iOutSize, 1);
                printf(".H file created successfully!\n");
            } else { // generate a binary file
                fwrite(&bbbm, 1, sizeof(BB_BITMAP), f);
                fwrite(pOut, 1, iOutSize, f);
                printf("Binary file created successfully!\n");
            }
            fflush(f);
            fclose(f);
        }
    } else {
        printf("Error encoding image: %d\n", rc);
    }
    free(png.getBuffer());
    }
    return 0;
}
