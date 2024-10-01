//
// Group5 Image Compressor tool
// Written by Larry Bank
// Copyright (c) 2024 BitBank Software, Inc.
//

#include <stdio.h>
#define MAX_IMAGE_FLIPS 256
#include "../src/Group5.h"
#include "../src/g5enc.inl"
//
// Read a Windows BMP file into memory
//
uint8_t * ReadBMP(const char *fname, int *width, int *height, int *bpp, unsigned char *pPal)
{
    int y, w, h, bits, offset;
    uint8_t *s, *d, *pTemp, *pBitmap;
    int pitch, bytewidth;
    int iSize, iDelta;
    FILE *infile;
    
    infile = fopen(fname, "r+b");
    if (infile == NULL) {
        printf("Error opening input file %s\n", fname);
        return NULL;
    }
    // Read the bitmap into RAM
    fseek(infile, 0, SEEK_END);
    iSize = (int)ftell(infile);
    fseek(infile, 0, SEEK_SET);
    pBitmap = (uint8_t *)malloc(iSize);
    pTemp = (uint8_t *)malloc(iSize);
    fread(pTemp, 1, iSize, infile);
    fclose(infile);
    
    if (pTemp[0] != 'B' || pTemp[1] != 'M' || pTemp[14] < 0x28) {
        free(pBitmap);
        free(pTemp);
        printf("Not a Windows BMP file!\n");
        return NULL;
    }
    w = *(int32_t *)&pTemp[18];
    h = *(int32_t *)&pTemp[22];
    bits = *(int16_t *)&pTemp[26] * *(int16_t *)&pTemp[28];
    if (bits <= 8 && pPal != NULL) { // it has a palette, copy it
        uint8_t *p = pPal;
        for (int i=0; i<(1<<bits); i++)
        {
           *p++ = pTemp[54+i*4];
           *p++ = pTemp[55+i*4];
           *p++ = pTemp[56+i*4];
        }
    }
    offset = *(int32_t *)&pTemp[10]; // offset to bits
    if (bits == 1) {
        bytewidth = (w+7) >> 3;
    } else {
        bytewidth = (w * bits) >> 3;
    }
    pitch = (bytewidth + 3) & 0xfffc; // DWORD aligned
// move up the pixels
    d = pBitmap;
    s = &pTemp[offset];
    iDelta = pitch;
    if (h > 0) {
        iDelta = -pitch;
        s = &pTemp[offset + (h-1) * pitch];
    } else {
        h = -h;
    }
    for (y=0; y<h; y++) {
        if (bits == 32) {// need to swap red and blue
            for (int i=0; i<bytewidth; i+=4) {
                d[i] = s[i+2];
                d[i+1] = s[i+1];
                d[i+2] = s[i];
                d[i+3] = s[i+3];
            }
        } else {
            memcpy(d, s, bytewidth);
        }
        d += bytewidth;
        s += iDelta;
    }
    *width = w;
    *height = h;
    *bpp = bits;
    free(pTemp);
    return pBitmap;
    
} /* ReadBMP() */

int main(int argc, const char * argv[]) {
    uint8_t *pBMP, *pOut;
    int rc, w, h, y, bpp;
    int iOutSize, iPitch;
    G5ENCIMAGE g5enc;
    BB_BITMAP bbbm;
 
    printf("Group5 image conversion tool\n");
    if (argc != 3) {
        printf("Usage: ./imgconvert <WinBMP image> <g5 compressed image>\n");
        return -1;
    }
    pBMP = ReadBMP(argv[1], &w, &h, &bpp, NULL);
    if (bpp == 1) {
        uint8_t *s = pBMP;
        
        printf("bmp size %d x %d\n", w, h);
        iPitch = (w+7) >> 3;
        pOut = (uint8_t *)malloc(iPitch * h);
        rc = g5_encode_init(&g5enc, w, h, pOut, iPitch * h);
        for (y=0; y<h && rc == G5_SUCCESS; y++) {
            rc = g5_encode_encodeLine(&g5enc, s);
            s += iPitch;
        }
        if (rc == G5_ENCODE_COMPLETE) {
            FILE *f;
            printf("Encode succeeded!\n");
            iOutSize = g5_encode_getOutSize(&g5enc);
            printf("Input size:  %d bytes, output size: %d bytes\n", iPitch*h, iOutSize);
            printf("Compression ratio: %2.1f:1\n", (float)(iPitch*h) / (float)iOutSize);
            bbbm.u16Marker = BB_BITMAP_MARKER;
            bbbm.width = w;
            bbbm.height = h;
            bbbm.size = iOutSize;
            f = fopen(argv[2], "w+b");
            if (!f) {
                printf("Error opening: %s\n", argv[2]);
            } else {
                fwrite(&bbbm, 1, sizeof(BB_BITMAP), f);
                fwrite(pOut, 1, iOutSize, f);
                fflush(f);
                fclose(f);
            }
        } else {
            printf("Error encoding image: %d\n", rc);
        }
    } else {
        printf("Only 1-bpp images are supported.\n");
    }
    free(pBMP);
    return 0;
} /* main() */
