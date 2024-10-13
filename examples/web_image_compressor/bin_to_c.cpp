//
// Convert binary data to C hex statements
//
#include <Arduino.h>
//
// Convert from bin to hex using the same buffer
//
int bin2c(char *leafname, uint8_t *pSrc, int iSrcSize, int w, int h)
{
int i, j, k, iCount, iSize = 0;
char szTemp[128];
uint8_t *p, *pTemp;
const uint8_t u8Hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  pTemp = (uint8_t *)malloc(iSrcSize);
  if (!pTemp) return 0; // something went wrong
  memcpy(pTemp, pSrc, iSrcSize); // keep original binary data here
  sprintf(szTemp, "//\n// %d x %d 1-bpp G5 compressed image\n", w, h);
  i = strlen(szTemp);
  memcpy(&pSrc[iSize], szTemp, i);
  iSize += i;
  sprintf(szTemp, "// data size = %d bytes\n//\n", iSrcSize);
  i = strlen(szTemp);
  memcpy(&pSrc[iSize], szTemp, i);
  iSize += i;
  sprintf(szTemp, "// for non-Arduino builds...\n#ifndef PROGMEM\n#define PROGMEM\n#endif\nconst uint8_t %s[] PROGMEM = {\n", leafname); // start of data array
  i = strlen(szTemp);
  memcpy(&pSrc[iSize], szTemp, i);
  iSize += i;
  // convert the data
    iCount = 0;
    p = pTemp;
    for (i=0; i<iSrcSize>>4; i++) { // do lines of 16 bytes
        pSrc[iSize++] = '\t';
        for (j=0; j<16; j++) {
            pSrc[iSize++] = '0';
            pSrc[iSize++] = 'x';
            pSrc[iSize++] = u8Hex[p[(i*16)+j]>>4];
            pSrc[iSize++] = u8Hex[p[(i*16)+j] & 0xf];
            if (iCount != iSrcSize-1) { // add a comma
               pSrc[iSize++] = ',';
            }
            iCount++;
        }
        pSrc[iSize++] = '\n';
    } // for each data byte
    p += (iSrcSize & 0xfff0); // point to last section
    if (iSrcSize & 0xf) // any remaining characters?
    {
        pSrc[iSize++] = '\t';
        for (j=0; j<(iSrcSize & 0xf); j++) {
            pSrc[iSize++] = '0';
            pSrc[iSize++] = 'x';
            pSrc[iSize++] = u8Hex[p[(i*16)+j]>>4];
            pSrc[iSize++] = u8Hex[p[(i*16)+j] & 0xf];
            if (iCount != iSrcSize-1) { // add a comma
               pSrc[iSize++] = ',';
            }
            iCount++;
        }
    }
    // finalize
    pSrc[iSize++] = '}';
    pSrc[iSize++] = ';';
    pSrc[iSize++] = '\n'; // final closing brace

  return iSize;
} /* bin2c() */

