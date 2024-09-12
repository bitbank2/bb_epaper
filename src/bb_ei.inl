//
// bb_eink
// Copyright (c) 2024 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
// Project started 9/11/2024
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// bb_ei.inl
// display interfacing/control code for bb_eink library
//
#if defined(_LINUX_) || defined(ARDUINO_ARCH_MCS51)
#define memcpy_P memcpy
#endif
void initSPI(OBDISP *pBBEI, int iSpeed, int iMOSI, int iCLK, int iCS);
void _delay(int iDelay);
static void EPDWaitBusy(OBDISP *pBBEI, int bQuick);
static void EPDSleep(OBDISP *pBBEI, int bDeep);
#ifndef WIMPY_MCU
static void EPDWriteImage4bpp(OBDISP *pBBEI, uint8_t ucCMD, int x, int y, int w, int h);
static void EPDWriteImage2bpp(OBDISP *pBBEI, uint8_t ucCMD, int x, int y, int w, int h);
static void EPDWriteImage(OBDISP *pBBEI, uint8_t ucCMD, uint8_t *pBits, int x, int y, int w, int h, int bInvert);
#endif
void EPD213_Begin(OBDISP *pBBEI, int x, int y, int w, int h, int bPartial);
void EPD_CMD2(OBDISP *pBBEI, uint8_t cmd, uint8_t param);
void obdSetDCMode(OBDISP *pBBEI, int iMode);
void InvertBytes(uint8_t *pData, uint8_t bLen);
void SPI_BitBang(OBDISP *pBBEI, uint8_t *pData, int iLen, uint8_t iMOSIPin, uint8_t iSCKPin);
static void RawWrite(OBDISP *pBBEI, unsigned char *pData, int iLen);
void RawWriteData(OBDISP *pBBEI, unsigned char *pData, int iLen);
// EPD look up tables

// 2.7" 176x264 LUTs
const uint8_t lut_fast_vcom_dc[] PROGMEM = {
    0x00, 0x00,
    0x00, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
const uint8_t lut_fast_ww[] PROGMEM = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x40, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x80, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
const uint8_t lut_fast_bw[] PROGMEM = {
    0xA0, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x90, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0xB0, 0x04, 0x10, 0x00, 0x00, 0x05,
    0xB0, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0xC0, 0x23, 0x00, 0x00, 0x00, 0x01
};
const uint8_t lut_fast_bb[] PROGMEM = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x40, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x80, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
const uint8_t lut_fast_wb[] PROGMEM = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x20, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x10, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

const uint8_t lut_vcom_dc[] PROGMEM =
{
    0x00, 0x00,
    0x00, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x32, 0x32, 0x00, 0x00, 0x02,
    0x00, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const uint8_t lut_ww[] PROGMEM =
{
    0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
     0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const uint8_t lut_bw[] PROGMEM =
{
    0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
     0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};
const uint8_t lut_bb[] PROGMEM =
{
    0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
      0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
      0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const uint8_t lut_wb[] PROGMEM =
{
    0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
     0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_vcom0_full[] PROGMEM =
{
  0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x17, 0x17, 0x00, 0x00, 0x02,
  0x00, 0x0A, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x0E, 0x0E, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut42_4gray_vcom[] PROGMEM = {
0x00    ,0x0A   ,0x00   ,0x00   ,0x00   ,0x01,
0x60    ,0x14   ,0x14   ,0x00   ,0x00   ,0x01,
0x00    ,0x14   ,0x00   ,0x00   ,0x00   ,0x01,
0x00    ,0x13   ,0x0A   ,0x01   ,0x00   ,0x01,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00
};
const unsigned char lut42_4gray_ww[] PROGMEM = {
0x40    ,0x0A   ,0x00   ,0x00   ,0x00   ,0x01,
0x90    ,0x14   ,0x14   ,0x00   ,0x00   ,0x01,
0x10    ,0x14   ,0x0A   ,0x00   ,0x00   ,0x01,
0xA0    ,0x13   ,0x01   ,0x00   ,0x00   ,0x01,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00
};
const unsigned char lut42_4gray_bw[] PROGMEM = {
0x40    ,0x0A   ,0x00   ,0x00   ,0x00   ,0x01,
0x90    ,0x14   ,0x14   ,0x00   ,0x00   ,0x01,
0x00    ,0x14   ,0x0A   ,0x00   ,0x00   ,0x01,
0x99    ,0x0C   ,0x01   ,0x03   ,0x04   ,0x01,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00
};
const unsigned char lut42_4gray_bb[] PROGMEM = {
0x80    ,0x0A   ,0x00   ,0x00   ,0x00   ,0x01,
0x90    ,0x14   ,0x14   ,0x00   ,0x00   ,0x01,
0x20    ,0x14   ,0x0A   ,0x00   ,0x00   ,0x01,
0x50    ,0x13   ,0x01   ,0x00   ,0x00   ,0x01,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00
};
const unsigned char lut42_4gray_wb[] PROGMEM = {
0x40    ,0x0A   ,0x00   ,0x00   ,0x00   ,0x01,
0x90    ,0x14   ,0x14   ,0x00   ,0x00   ,0x01,
0x00    ,0x14   ,0x0A   ,0x00   ,0x00   ,0x01,
0x99    ,0x0B   ,0x04   ,0x04   ,0x01   ,0x01,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00,
0x00    ,0x00   ,0x00   ,0x00   ,0x00   ,0x00
};

#ifndef WIMPY_MCU
const uint8_t epd29b_bwyr_init_sequence_full[] PROGMEM = {
    0x02, 0x4d, 0x78, 
    0x03, 0x00, 0x0f, 0x29, // PSR
    0x03, 0x01, 0x07, 0x00, // PWRR
    0x04, 0x03, 0x10, 0x54, 0x44, // POFS
    0x08, 0x06, 0x05, 0x00, 0x3f, 0x0a, 0x25, 0x12, 0x1a, // BTST_P
    0x02, 0x50, 0x37, // CDI
    0x03, 0x60, 0x02, 0x02, // TCON
    0x05, 0x61, 0x00, 128, 0x01, 296-256, // TRES
    0x02, 0xe7, 0x1c, 
    0x02, 0xe3, 0x22,
    0x04, 0xb4, 0xd0, 0xb5, 0x03,
    0x02, 0xe9, 0x01,
    0x02, 0x30, 0x08,
    0x01, 0x04, // PON
    BUSY_WAIT,
    0x00
};    
const uint8_t epd29_bwyr_init_sequence_full[] PROGMEM = {
    0x02, 0x4d, 0x78, 
    0x03, 0x00, 0x0f, 0x29, // PSR
    0x03, 0x01, 0x07, 0x00, // PWRR
    0x04, 0x03, 0x10, 0x54, 0x44, // POFS
    0x08, 0x06, 0x05, 0x00, 0x3f, 0x0a, 0x25, 0x12, 0x1a, // BTST_P
    0x02, 0x50, 0x37, // CDI
    0x03, 0x60, 0x02, 0x02, // TCON
    0x05, 0x61, 0x00, 168, 0x01, 384-256, // TRES
    0x02, 0xe7, 0x1c, 
    0x02, 0xe3, 0x22,
    0x04, 0xb4, 0xd0, 0xb5, 0x03,
    0x02, 0xe9, 0x01,
    0x02, 0x30, 0x08,
    0x01, 0x04, // PON
    BUSY_WAIT,
    0x00
};    
const uint8_t epd266_bwyr_init_sequence_full[] PROGMEM = {
    0x02, 0x4d, 0x78,
    0x03, 0x00, 0x0f, 0x29, // PSR
    0x03, 0x01, 0x07, 0x00, // PWRR
    0x04, 0x03, 0x10, 0x54, 0x44, // POFS
    0x08, 0x06, 0x05, 0x00, 0x3f, 0x0a, 0x25, 0x12, 0x1a, // BTST_P
    0x02, 0x50, 0x37, // CDI
    0x03, 0x60, 0x02, 0x02, // TCON
    0x05, 0x61, 0x00, 184, 0x01, 360-256, // TRES
    0x02, 0xe7, 0x1c,
    0x02, 0xe3, 0x22,
    0x04, 0xb4, 0xd0, 0xb5, 0x03,
    0x02, 0xe9, 0x01,
    0x02, 0x30, 0x08,
    0x01, 0x04, // PON
    BUSY_WAIT,
    0x00
};
const uint8_t epd236_bwyr_init_sequence_full[] PROGMEM = {
    0x05, 0x66, 0x49, 0x55, 0x13, 0x5d,
    0x03, 0x66, 0x49, 0x55,
    0x02, 0xb0, 0x03, // boost
    0x03, 0x00, 0x4f, 0x6b,
    0x02, 0x03, 0x00, 
    0x06, 0xf0, 0xf6, 0x0d, 0x00, 0x00, 0x00,
    0x04, 0x06, 0xcf, 0xdf, 0x0f,
    0x02, 0x41, 0x00, 
    0x02, 0x50, 0x30,
    0x03, 0x60, 0x0c, 0x05,
    0x04, 0x61, 0xa8, 0x01, 0x28, // resolution
    0x02, 0x84, 0x01,
    0x00
};    
const uint8_t epd164_bwyr_init_sequence_full[] PROGMEM = {
    0x05, 0x66, 0x49, 0x55, 0x13, 0x5d,
    0x03, 0x66, 0x49, 0x55,
    0x02, 0xb0, 0x03, // boost
    0x03, 0x00, 0x4f, 0x6b,
    0x02, 0x03, 0x00,
    0x06, 0xf0, 0xf6, 0x0d, 0x00, 0x00, 0x00,
    0x04, 0x06, 0xcf, 0xdf, 0x0f,
    0x02, 0x41, 0x00,
    0x02, 0x50, 0x30,
    0x03, 0x60, 0x0c, 0x05,
    0x04, 0x61, 0xa8, 0x00, 0xa8, // resolution
    0x02, 0x84, 0x01,
    0x00
};    
const uint8_t epd30_bwyr_init_sequence_full[] PROGMEM = {
    0x07, 0x66, 0x49, 0x55, 0x13, 0x5d, 0x05, 0x10,
    0x02, 0xb0, 0x00, // boost
    0x03, 0x01, 0x0f, 0x00,
    0x03, 0x00, 0x4f, 0x6b,
    0x04, 0x06, 0xd7, 0xde, 0x12,
    0x05, 0x61, 0x00, 0xa8, 0x01, 0x90, // resolution
    0x02, 0x50, 0x37,
    0x03, 0x60, 0x0c, 0x05,
    0x02, 0xe3, 0xff,
    0x02, 0x84, 0x00,
    0x00
};

const uint8_t epd583r_init_sequence_full[] PROGMEM = {
    0x03, 0x01, 0x37, 0x00, // power setting
    0x03, 0x00, 0xcf, 0x08, // panel setting
    0x04, 0x06, 0xc7, 0xcc, 0x28, // boost
    0x02, 0x30, 0x3a, // PLL 15s refresh
    0x02, 0x41, 0x00, // temperature
    0x02, 0x50, 0x77, // VCOM
    0x02, 0x60, 0x22, // TCON
    0x05, 0x61, 0x02, 0x58, 0x01, 0xc0, // RES
    0x02, 0x82, 0x28, // temp range
    0x02, 0xe5, 0x03, // flash mode
    0x01, 0x04, // power on
    BUSY_WAIT,
    0x00
};
const uint8_t epd74r_init_sequence_full[] PROGMEM = {
    0x02, 0x65, 0x01,
    0x01, 0xab,
    0x02, 0x65, 0x00,
    0x01, 0x04,
    BUSY_WAIT,
    0x03, 0x01, 0x37, 0x00, // 0x05, 0x05,
    0x03, 0x00, 0xcf, 0x08,
//    0x02, 0xe5, 0x03,
//    0x02, 0x03, 0x00,
    0x04, 0x06, 0xc7, 0xcc, 0x28,
    0x02, 0x30, 0x3c,
    0x02, 0x41, 0x00,
    0x02, 0x50, 0x77,
    0x02, 0x60, 0x22,
    0x05, 0x61, 0x02, 0x80, 0x01, 0x80,
//    0x02, 0x65, 0x01,
//    0x02, 0x65, 0x00,
//    0x01, 0x40,
//    BUSY_WAIT,
//    0x02, 0x8d, 0x80,
    0x02, 0x82, 0x1e,
    0x02, 0xe5, 0x03,
//    0x01, 0x02,
//    BUSY_WAIT,
    0x01, 0x04,
    BUSY_WAIT,
//    0x02, 0x65, 0x01,
//    0x02, 0x65, 0x00,
//    0x01, 0x40,
//    BUSY_WAIT,
//    0x02, 0x8d, 0xc0,
//    0x02, 0x30, 0x2a,
    0x00
};
#endif // !WIMPY_MCU

const uint8_t epd35r_init_sequence_full[] PROGMEM = {
    0x01, 0x12, // SW RESET
    BUSY_WAIT,
    0x02, 0x3c, 0x05,
    0x02, 0x18, 0x80,
    0x02, 0x22, 0xb1,
    0x01, 0x20,
    BUSY_WAIT,
    0x03, 0x1b, 0x17, 0x80, // reading temp, delay 5ms
    BUSY_WAIT,
    0x02, 0x4e, 0x00, // ram counter start
    0x03, 0x4f, 0x7f, 0x01,
    0x00
};

// init sequence for GDEW042Z15
const uint8_t epd42r2_init_sequence_full[] PROGMEM = {
    0x03, UC8151_PSR, 0xcf, 0x8d,
   0x06, UC8151_PWR, 0x03, 0x10, 0x3f, 0x3f, 0x0d,
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x05, UC8151_TRES, 0x01, 0x90, 1, 0x2c, // resolution
    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
//    0x02, 0x00, 0x0f, // LUT from OTP
    0x02, UC8151_CDI, 0x5c, // inverted, white border
    0x00
};

// init sequence for 2.13" Random screen double connector
const uint8_t EPD213R_104x212_d_init_sequence_full[] PROGMEM = {
    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x02, 0x00, 0x0f, // LUT from OTP
    0x02, UC8151_CDI, 0x5c, // inverted, white border
    0x00
};

const uint8_t epd37_init_sequence_full[] PROGMEM = {
    0x01, UC8151_PON, // Power on
    BUSY_WAIT,
    0x02, 0x00, 0x1f, // flip x
    0x02, 0x50, 0x97, // VCOM
    0x00
};

const uint8_t epd37_init_sequence_part[] PROGMEM = {
    0x01, UC8151_PON, // Power on
    BUSY_WAIT,
    0x02, 0x00, 0x1f, // flip x
    0x02, 0xe0, 0x02,
    0x02, 0xe5, 0x6e,
    0x02, 0x50, 0xd7, // VCOM
    0x00
};

// initialization sequence for 3.7" 240x416 e-paper
const uint8_t epd37xx_init_sequence_full[] PROGMEM = {
    0x03, UC8151_PSR, 0xdf, 0x8d,
    0x06, UC8151_PWR, 0x03, 0x10, 0x3f, 0x3f, 0x0d,
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x04, UC8151_TRES, 0xf0, 1, 0xa0, // resolution

    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
    0x02, UC8151_PFS, 0x00, // FRAMES_1
    0x02, UC8151_TSE, 0x00, // TEMP_INTERNAL | OFFSET_0
    0x02, UC8151_TCON, 0x22,
    0x02, UC8151_CDI, 0xd7, // inverted, white border
    0x02, UC8151_PLL, 0x09, // HZ_50
    0x00 // end of table
};
// initialization sequence for 2.9" 296x128 e-paper
const uint8_t epd29_init_sequence_full[] PROGMEM = {
    0x02, UC8151_PSR, 0x80 | 0x00 | 0x10 | 0x08 | 0x04 | 0x02 | 0x01, // RES_128x296, LUT_OTP, FORMAT_BW, SHIFT_LEFT, BOOSTER_ON, RESET_NONE
    0x06, UC8151_PWR, 0x03, 0x00, 0x2b, 0x2b, 0x2b,
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
    0x02, UC8151_PFS, 0x00, // FRAMES_1
    0x04, UC8151_TRES, 0x80, 1, 0x28, // resolution
    0x02, UC8151_TSE, 0x00, // TEMP_INTERNAL | OFFSET_0
    0x02, UC8151_TCON, 0x22,
    0x02, UC8151_CDI, 0x9c, // inverted, white border
    0x02, UC8151_PLL, 0x3a, // HZ_100
    0x00 // end of table
};

const uint8_t epd42r_init_sequence[] PROGMEM = {
    0x01, 0x12, // soft reset
    BUSY_WAIT,
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart
    0x04, 0x01, 0x2b, 0x01, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x31, // RAM X start/end
    0x05, 0x45, 0,0,0x2b, 0x01, // RAM Y start/end
    0x02, 0x3c, 0x01, // border (0=bk,1=wh,2=red)
    0x02, 0x18, 0x80, // temp sensor = internal
    0x02, 0x21, 0x00, // display update ctrl 1
    0x02, 0x22, 0xb1, // display update ctrl 2
    0x01, 0x20, // master activation
    BUSY_WAIT,
    0x02, 0x4e, 0x00, // RAM X counter
    0x03, 0x4f, 0x2b, 0x01, // RAM Y counter
    0x00
};
const uint8_t epd29r_init_sequence[] PROGMEM = {
    0x01, 0x12, // soft reset
    BUSY_WAIT,
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart
    0x04, 0x01, 0x27, 0x01, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x0f, // RAM X start/end
    0x05, 0x45, 0,0,0x27, 0x01, // RAM Y start/end
    0x02, 0x3c, 0x01, // border (0=bk,1=wh,2=red)
    0x02, 0x18, 0x80, // temp sensor = internal
    0x02, 0x21, 0x00, // display update ctrl 1
    0x02, 0x22, 0xb1, // display update ctrl 2
    0x01, 0x20, // master activation
    BUSY_WAIT,
    0x02, 0x4e, 0x00, // RAM X counter
    0x03, 0x4f, 0x27, 0x01, // RAM Y counter
    0x00
};

// for 152x152 BWR
const uint8_t epd29r_init_sequence_152[] PROGMEM = {
    0x01, 0x12, // soft reset
    BUSY_WAIT,
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart
    0x04, 0x01, 0x97, 0x00, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x12, // RAM X start/end
    0x05, 0x45, 0,0,0x97, 0x00, // RAM Y start/end
    0x02, 0x3c, 0x01, // border (0=bk,1=wh,2=red)
    0x02, 0x18, 0x80, // temp sensor = internal
    0x02, 0x21, 0x00, // display update ctrl 1
    0x02, 0x22, 0xb1, // display update ctrl 2
    0x01, 0x20, // master activation
    BUSY_WAIT,
    0x02, 0x4e, 0x00, // RAM X counter
    0x03, 0x4f, 0x97, 0x00, // RAM Y counter
    0x00
};
const uint8_t epd75_init_sequence_partial[] PROGMEM = {
    0x06, UC8151_PWR, 0x17, 0x17, 0x3f, 0x3f, 0x11,
    0x02, 0x82, 0x26, // VCOM DC
 //   0x05, UC8151_BTST, 0x27,0x27,0x2f,0x17,
 //   0x02, 0x30, 0x06, // oscillator
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x02, UC8151_PSR, 0x3f, // from register
 //   0x05, 0x61, 0x03, 0x20, 0x01, 0xe0, // resolution
 //   0x02, 0x15, 0x00, // SPI mode
    0x03, 0x50, 0x39, 0x07, // VCOM Data interval
 //   0x02, 0x60, 0x22, // TCON
 //   0x05, 0x65, 0x00, 0x00, 0x00, 0x00, // resolution
//#ifdef FUTURE
//    0x02, UC8151_PLL, 0x3a, // HZ_100
//    0x02, UC8151_VDCS, 0x12, // VCOM DC setting
//    0x02, UC8151_CDI, 0x97,
    0x2d, UC8151_LUT_VCOM, // VCOM LUT
        0x00, 30, 5, 30, 5, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        ,0x00, 0x00,
    0x2b, UC8151_LUT_WW, // WW LUT
        0x00, 30, 5, 30, 5, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_BW,
        0x5a, 30, 5, 30, 5, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_WB,
        0x84, 30, 5, 30, 5, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_BB,
        0x00, 30, 5, 30, 5, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_VCOM2,
    0x00, 30, 5, 30, 5, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//#endif // FUTURE
    0x00 // end of table
};
// 1.54" 152x152
const uint8_t epd154a_init_sequence_full[] PROGMEM =
{
    1, UC8151_PON,
    BUSY_WAIT,
    2, UC8151_PSR, 0xdf, // panel setting
    4, UC8151_TRES, 0x98, 0, 0x98, // resolution
    2, UC8151_CDI, 0x97, // VCOM
    0
};
const uint8_t epd154a_init_sequence_part[] PROGMEM =
{
    1, UC8151_PON,
    BUSY_WAIT,
    2, UC8151_PSR, 0x3f, // panel setting
    6, UC8151_PWR, 0x03, 0x00, 0x21, 0x21, 0x03,
    4, UC8151_TRES, 0x98, 0, 0x98, // resolution
    2, UC8151_VDCS, 0x12,
    2, UC8151_CDI, 0x17, // VCOM
    37, 0x20, 0x00, 10, 0, 60, 10, 1, // VCOM LUT
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
    37, 0x21, 0x00, 10, 0, 60, 10, 1,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
    37, 0x22, 0x5a, 10, 0, 60, 10, 1,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
    37, 0x23, 0x84, 10, 0, 60, 10, 1,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
    37, 0x24, 0x00, 10, 0, 60, 10, 1,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,
    0
};

// 1.54" 200x200
const uint8_t epd154_init_sequence_full[] PROGMEM =
{
    0x01, 0x12, // sw reset
    BUSY_WAIT,
    0x04, 0x01, 199, 0x00, 0x00, // driver output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x18,
    0x05, 0x45, 0x00, 0x00, 0xc7, 0x00,
    0x02, 0x3c, 0x05, // border waveform
    0x02, 0x18, 0x80, // read temp sensor
//    0x03, 0x21, 0x00, 0x80, // display update control
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
};

const uint8_t epd154_init_sequence_part[] PROGMEM =
{
    0x02, 0x11, 0x03,
    0x03, 0x44, 0x00, 0x18,
    0x05, 0x45, 0x00, 0x00, 199, 0x00,
    0x02, 0x3c, 0x80, // border waveform
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    0
};

// partial (no flash) updates
const uint8_t epd154b_init_sequence_part[] PROGMEM =
{  
    0x02, 0x11, 0x03, 
    0x02, 0x3c, 0x80, // border waveform 
    0x03, 0x44, 0x00, 0x18,
    0x05, 0x45, 0x00, 0x00, 199, 0x00,
    0x02, 0x4e, 0x00, 
    0x03, 0x4f, 0x00, 0x00,
      
    154, 0x32, // LUT
    0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0xF,0x0,0x0,0x0,0x0,0x0,0x0,
    0x1,0x1,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,

    2, 0x3f, 0x2, // ??
    2, 0x03, 0x17, // gate voltage
    4, 0x04, 0x41, 0xb0, 0x32, // source voltage
    2, 0x2c, 0x28, // VCOM
    11, 0x37, 0,0,0,0,0,0x40,0,0,0,0, // ??
    2, 0x3c, 0x80, // VCOM ??
    
    0x00 // end of table
};

const uint8_t epd213b_init_sequence_full[] PROGMEM =
{
    0x01, 0x12, // sw reset
    BUSY_WAIT,
    0x04, 0x01, 0xf9, 0x00, 0x00,
    0x02, 0x11, 0x03,
    0x03, 0x44, 0x00, 0x0f,
    0x05, 0x45, 0x00, 0x00, 0xf9, 0x00,
    0x02, 0x3c, 0x05, // border waveform
    0x03, 0x21, 0x00, 0x80, // display update control
    0x02, 0x18, 0x80,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
};

const uint8_t epd213b_init_sequence_part[] PROGMEM =
{
    0x02, 0x11, 0x03, // data direction
    0x03, 0x44, 0x00, 0x0f, // x start/end
    0x05, 0x45, 0x00, 0x00, 0xf9, 0x00, // y start/end
    0x02, 0x3c, 0x80, // border waveform
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    0x00 // end of table
};

const uint8_t epd294_init_sequence_full[] PROGMEM =
{   
//    0x02, 0x74, 0x54,
//    0x02, 0x7e, 0x3b,
//    0x03, 0x2b, 0x04, 0x63,
//    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f,
    0x04, 0x01, 0x27, 0x01, 0x00,
    0x02, 0x11, 0x03,
    0x03, 0x44, 0x00, 0x0f,
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x03, 0x21, 0x00, 0x80,
    0x02, 0x3c, 0xc0,
    0x02, 0x18, 0x80,
    0x02, 0x22, 0xb1,
    0x01, 0x20,
    BUSY_WAIT,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    0x00 // end of table
}; /* epd294_init_sequence_full[] */

const uint8_t epd102_init_sequence_full[] PROGMEM =
{
  2, 0x00, 0x5f, // panel setting
  3, 0x2a, 0x00, 0x00, // IC hidden instructions
  1, 0x04, // power on
  BUSY_WAIT,
  2, 0x50, 0x97, // VCOM
  0
};
const uint8_t epd102_init_sequence_part[] PROGMEM =
{
  0x2, 0xd2, 0x3f,
  0x2, 0x00, 0x6f, // panel setting
  0x5, 0x01, 0x03, 0x00, 0x26, 0x26, // power
  0x2, 0x06, 0x3f,
  0x3, 0x2a, 0x00, 0x00,
  0x2, 0x30, 0x13, // 50Hz
  0x2, 0x50, 0xf2,
  0x2, 0x60, 0x22,
  0x2, 0x82, 0x12, // -0.1v
  0x2, 0xe3, 0x33,
// send LUTs
  43, 0x23,       // white
    0x60  ,0x01 ,0x01 ,0x00 ,0x00 ,0x01 ,
    0x80  ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
  43, 0x24,       // black
    0x90  ,0x01 ,0x01 ,0x00 ,0x00 ,0x01 ,
    0x40  ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
  0x1, 0x4, // power on
  BUSY_WAIT,
  0
};

const uint8_t epd293_init_sequence_full[] PROGMEM =
{
    0x01, SSD1608_SW_RESET,
    BUSY_WAIT,

    0x04, 0x01, 0x27, 0x01, 0x00, // driver output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x0f, // ram start/end
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x02, 0x3c, 0xc0, // border waveform
    0x03, 0x21, 0x00, 0x80, // display update control
    0x02, 0x18, 0x80, // read built-in temp sensor

    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
};
// less flashing (fast) updates
const uint8_t epd293_init_sequence_fast[] PROGMEM =
{
    0x01, SSD1608_SW_RESET,
    BUSY_WAIT,
    0x02, 0x18, 0x80, // read built-in temp sensor
    0x02, 0x22, 0xb1, // load temp value
    0x01, 0x20, // execute
    BUSY_WAIT,
    0x03, 0x1a, 0x64, 0x00, // write temp register
    0x02, 0x22, 0x91, // load temp value
    0x01, 0x20, // execute
    BUSY_WAIT,
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x0f, // ram start/end
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
};

// partial (no flash) updates
const uint8_t epd293_init_sequence_part[] PROGMEM =
{
    0x02, 0x11, 0x03,
    0x02, 0x3c, 0x80, // border waveform
    0x03, 0x44, 0x00, 0x0f,
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    0x00 // end of table
};

// partial (no flash) updates
const uint8_t epd295_init_sequence_part[] PROGMEM =
{   
    0x02, 0x11, 0x03, 
    0x02, 0x3c, 0x80, // border waveform
    0x03, 0x44, 0x00, 0x0f,
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,

    91, 0x32, // LUT
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0A, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,

    0x00 // end of table
}; 

const uint8_t epd266_init_sequence_full[] PROGMEM =
{
    0x01, SSD1608_SW_RESET,
    BUSY_WAIT,

    0x04, 0x01, 0x27, 0x01, 0x00, // driver output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x12, // ram start/end
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x02, 0x3c, 0x05, // border waveform
    0x03, 0x21, 0x00, 0x80, // display update control
    0x02, 0x18, 0x80, // read built-in temp sensor

    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
}; /* epd266_init_sequence_full[] */

const uint8_t epd266_init_sequence_part[] PROGMEM =
{
    0x02, 0x11, 0x03,
    0x02, 0x3c, 0x80, // border waveform
    0x03, 0x44, 0x00, 0x12,
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,

    0x00 // end of table
};

const uint8_t epd27_init_sequence_full[] PROGMEM =
{
    0x04, 0x01, 0x07, 0x01, 0x00, // driver output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x15, // ram address
    0x05, 0x45, 0x00, 0x00, 0x07, 0x01,
    0x02, 0x3c, 0x80, // border color
    0x02, 0x18, 0x80, // read built-in temp sensor
    0x02, 0x4e, 0x00, // ram counter x
    0x03, 0x4f, 0x00, 0x00, // ram counter y
    0x02, 0x22, 0xb1,
    0x01, 0x20,
    BUSY_WAIT,
    0x00 // end of table
};

const uint8_t epd27_init_sequence_part[] PROGMEM =
{
    0x02, 0x11, 0x03, // data entry mode
    0x02, 0x3c, 0x80, // border color
    0x03, 0x44, 0x00, 0x15, // ram address
    0x05, 0x45, 0x00, 0x00, 0x07, 0x01,
    0x02, 0x4e, 0x00, // ram counter x
    0x03, 0x4f, 0x00, 0x00, // ram counter y
    
    0x00 // end of table
};

const uint8_t epd42b_init_sequence_full[] PROGMEM =
{   
    0x01, SSD1608_SW_RESET,
    BUSY_WAIT,

    0x04, 0x01, 0x2b, 0x01, 0x00, // driver output control
    0x03, 0x21, 0x40, 0x00, // display update control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x31, // ram start/end
    0x05, 0x45, 0x00, 0x00, 0x2b, 0x01,
    0x02, 0x3c, 0x05, // border waveform
    0x02, 0x18, 0x80, // read built-in temp sensor

    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
};

const uint8_t epd42b_init_sequence_fast[] PROGMEM =
{
    0x01, SSD1608_SW_RESET,
    BUSY_WAIT,
    3, 0x21, 0x40, 0x00,
    2, 0x3c, 0x05,
    2, 0x1a, 0x6e, // temp register
    2, 0x22, 0x91, // load temp
    1, 0x20,
    BUSY_WAIT,
    2, 0x11, 0x3, // data entry mode
    0x03, 0x44, 0x00, 0x31, // ram start/end
    0x05, 0x45, 0x00, 0x00, 0x2b, 0x01,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0
};
const uint8_t epd42b_init_sequence_part[] PROGMEM =
{
    0x03, 0x21, 0x00, 0x00,
    0x02, 0x11, 0x03, // data entry mode
    0x02, 0x3c, 0x80, // border color
    0x03, 0x44, 0x00, 0x31, // ram address
    0x05, 0x45, 0x00, 0x00, 0x2b, 0x01,
    0x02, 0x4e, 0x00, // ram counter x
    0x03, 0x4f, 0x00, 0x00, // ram counter y
    
    0x00 // end of table
};

const uint8_t epd579_init_sequence_full[] PROGMEM =
{
    2, 0x11, 0x03, // data format
    2, 0x3c, 0x80, // border color
    0x00 // end of table
}; /* epd579_init_sequence_full[] */

const uint8_t epd29b_init_sequence_full[] PROGMEM = {
    1, SSD1608_SW_RESET,
    BUSY_WAIT,
    2, 0x74, 0x54,
    2, 0x7e, 0x3b,
    4, 0x01, 39, 1, 0, // gate setting (height -1)
    2, 0x11, 0x3, // data entry mode
    2, 0x3c, 0x03, // black border
    2, 0x2c, 0x55, // VCOM
    2, 0x03, 0x17, // gate drive voltage
    4, 0x04, 0x41, 0xac, 0x32, // source driving voltage
    2, 0x3a, 0x07, // dummy line period
    2, 0x3b, 0x04, // get line width
    2, 0x4e, 0x00, // ram x start
    3, 0x4f, 0x00, 0x00, // ram y start
    BUSY_WAIT,
    0
};
const uint8_t epd213_104x212_init_sequence_full[] PROGMEM = {
    1, SSD1608_SW_RESET,
    BUSY_WAIT,
    2, 0x74, 0x54,
    2, 0x7e, 0x3b,
    4, 0x01, 211, 0, 0, // gate setting (height -1)
    2, 0x11, 0x3, // data entry mode
    2, 0x3c, 0x03, // black border
    2, 0x2c, 0x55, // VCOM
    2, 0x03, 0x17, // gate drive voltage
    4, 0x04, 0x41, 0xac, 0x32, // source driving voltage
    2, 0x3a, 0x07, // dummy line period
    2, 0x3b, 0x04, // get line width
    2, 0x4e, 0x00, // ram x start
    3, 0x4f, 0x00, 0x00, // ram y start
    BUSY_WAIT,
    0
};

const uint8_t epd213_122x250_init_sequence_full[] PROGMEM = {
    1, SSD1608_SW_RESET,
    BUSY_WAIT,
    2, 0x74, 0x54,
    2, 0x7e, 0x3b,
    4, 0x01, 249, 0, 0, // gate setting (height -1)
    2, 0x11, 0x3, // data entry mode
    2, 0x3c, 0x03, // black border
    2, 0x2c, 0x55, // VCOM
    2, 0x03, 0x15, // gate drive voltage
    4, 0x04, 0x41, 0xa8, 0x32, // source driving voltage
    2, 0x3a, 0x30, // dummy line period
    2, 0x3b, 0x0a, // get line width
    2, 0x4e, 0x00, // ram x start
    3, 0x4f, 0x00, 0x00, // ram y start
    BUSY_WAIT,
    0
};

const uint8_t epd213_122x250_init_sequence_part[] PROGMEM = {
    2, 0x3c, 0x80, // border
    2, 0x11, 0x3, // data entry mode
    3, 0x44, 0x00, 0xf,
    5, 0x45, 0x00, 0x00, 0xf9, 0x00,
    2, 0x4e, 0x00, // ram x start
    3, 0x4f, 0x00, 0x00, // ram y start
    
    0
};
const uint8_t epd75_init_sequence_full[] PROGMEM = {
    6, UC8151_PWR, 0x07, 0x07, 0x3f, 0x3f, 0x03,
    1, UC8151_PON,
    BUSY_WAIT,
    2, UC8151_PSR, 0x1f,
    5, UC8151_TRES, 0x03, 0x20, 0x01, 0xe0,
    2, 0x15, 0x00,
    3, UC8151_CDI, 0x29, 0x07,
    2, UC8151_TCON, 0x22,
//    4, UC8151_BTST, 0x17, 0x17, 0x17,
//    2, UC8151_PFS, 0x00,
//    2, UC8151_TSE, 0x00,
//    2, UC8151_PLL, 0x3a,
//    2, UC8151_VDCS, 0x12,
    0
};

const uint8_t epd75_init_sequence_fast[] PROGMEM = {
   2, 0x00, 0x1f, // panel setting
   3, 0x50, 0x10, 0x07, // VCOM
   1, 0x04, // Power On
   BUSY_WAIT,
   5, 0x06, 0x27, 0x27, 0x18, 0x17, // booster soft start
   2, 0xe0, 0x02,
   2, 0xe5, 0x5a,
   0
};

const uint8_t epd583_init_sequence_full[] PROGMEM = {
    2, UC8151_PSR, 0x9f,
    6, UC8151_PWR, 0x03, 0x00, 0x2b, 0x2b, 0x2b,
    1, UC8151_PON,
    BUSY_WAIT,
    4, UC8151_BTST, 0x17, 0x17, 0x17,
    2, UC8151_PFS, 0x00,
    2, UC8151_TSE, 0x00,
    2, UC8151_TCON, 0x22,
    2, UC8151_CDI, 0xd7,
    2, UC8151_PLL, 0x3a,
    5, UC8151_TRES, 0x02, 0x88, 0x01, 0xe0,
    2, UC8151_VDCS, 0x12,
    0
};

const uint8_t epd583_init_sequence_part[] PROGMEM = {
    1, UC8151_PON,
    BUSY_WAIT,
    3, 0x00, 0x1f, 0x0b, // panel setting
    2, 0x50, 0x97, // VCOM
    5, 0x01, 0x03,0x00,0x2b,0x2b, // power setting
    4, 0x06, 0x17,0x17,0x17, // boost soft start
    1, 0x04, // power on
    BUSY_WAIT,
    3, 0x00, 0xbf, 0x0b, // panel setting
    2, 0x30, 0x3c, // 3A 100HZ
    5, 0x61, 0x02, 0x88, 0x01, 0xe0, // resolution 648x480
    2, 0x82, 0x12, // vcom_DC
    2, 0x50, 0x47, // mode
    45, 0x20, // VCOM LUT
  0x00, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    43, 0x21, // WW LUT
  0x00, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    43, 0x22, // BW LUT
  0x20, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    43, 0x23, // WB LUT
  0x10, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    43, 0x24, // BB LUT
  0x00, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00 // end of table
};const uint8_t epd42_4gray_init_sequence_full[] PROGMEM = {
    6, UC8151_PWR, 0x03, 0x00, 0x2b, 0x2b, 0x13,
    4, UC8151_BTST, 0x17, 0x17, 0x17,
    1, UC8151_PON,
    BUSY_WAIT,
    2, UC8151_PSR, 0x3f,
    2, UC8151_PLL, 0x3c,
    5, UC8151_TRES, 0x01, 0x90, 0x01, 0x2c,
    2, UC8151_VDCS, 0x12,
    2, UC8151_CDI, 0x97,
    0   
};          

const uint8_t epd42_init_sequence_full[] PROGMEM = {
    1, UC8151_PON,
    BUSY_WAIT,
    3, 0x00, 0x1f, 0x0b, // panel setting
    5, UC8151_TRES, 0x01, 0x90, 0x01, 0x2c,
    2, 0x50, 0x97, // VCOM
    0
};

const uint8_t epd42_init_sequence_part[] PROGMEM = {
    1, UC8151_PON,
    BUSY_WAIT,
    3, 0x00, 0x1f, 0x0b, // panel setting
    2, 0x50, 0x97, // VCOM
    5, 0x01, 0x03,0x00,0x2b,0x2b, // power setting
    4, 0x06, 0x17,0x17,0x17, // boost soft start
    1, 0x04, // power on
    BUSY_WAIT,
    3, 0x00, 0xbf, 0x0b, // panel setting
    2, 0x30, 0x3c, // 3A 100HZ
    5, 0x61, 0x01, 0x90, 0x01, 0x2c, // resolution 400x300
    2, 0x82, 0x12, // vcom_DC
    2, 0x50, 0x47, // mode
    45, 0x20, // VCOM LUT
  0x00, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    43, 0x21, // WW LUT
  0x00, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    43, 0x22, // BW LUT
  0x20, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    43, 0x23, // WB LUT
  0x10, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    43, 0x24, // BB LUT
  0x00, 0x01, 0x20, 0x01, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00 // end of table
};
#ifndef WIMPY_MCU
const uint8_t st7302_wenting[] PROGMEM PROGMEM = {
    0x02, 0xEB, 0x02, // Enable OTP
    0x02, 0xD7, 0x68, // OTP Load Control
    0x02, 0xD1, 0x01, // Auto Power Control
    0x02, 0xC0, 0x40, // Gate Voltage setting
    0x07, 0xC1, 0x22, 0x22, 0x22, 0x22, 0x14, 0x00, // VSH Setting
    0x05, 0xC2, 0x00, 0x00, 0x00, 0x00, // VSL Setting
    0x02, 0xCB, 0x0E, // VCOMH Setting
    0x0B, 0xB4, 0xE5, 0x66, 0x85, 0xFF, 0xFF, 0x52, 0x85, 0xFF, 0xFF, 0x52, // Gate EQ
    0x03, 0xC7, 0xA6, 0xE9, // OSC Setting
    0x02, 0xB0, 0x3F, // Duty Setting
    0x03, 0xB2, 0x00, 0x00, // Frame Rate Setting (lowest for both HPM/LPM)
    0x02, 0x36, 0x00, // Memory Access Mode
    0x02, 0x3A, 0x11, // Data Format
    0x02, 0xB9, 0x23, // Source Setting
    0x02, 0xB8, 0x09, // Source Setting
    0x01, 0x11, // Sleep out
    0x02, 0xff, 100, // delay 100
    0x02, 0xD0, 0x1F, // enable auto power down
    0x01, 0x39, // Low power mode
    0x01, 0x29, // display on
    0x00
};

const uint8_t st7302_hpm_init[] PROGMEM = {
    0x02, 0xEB, 0x02, // Enable OTP
    0x02, 0xD7, 0x68, // OTP Load Control
    0x02, 0xD1, 0x01, // Auto Power Control
    0x02, 0xC0, 0xba , // Gate Voltage setting
    0x02, 0xCB, 0x00, // VCOMH Setting
    0x02, 0xB3, 0x94, // VCOMEQ enable
    0x07, 0xC1, 0x28, 0x28, 0x28, 0x28, 0x14, 0x00, // source voltage VSH
    0x05, 0xC2, 0x00, 0x00, 0x00, 0x00, // VSL Setting
    0x03, 0xB2, 0x01, 0x05, // Frame Rate Setting (lowest for both HPM/LPM)
    0x0B, 0xB4, 0xA5, 0x66, 0xFD, 0xBF, 0x42, 0x55, 0x81, 0xBE, 0x42, 0x55, // Gate EQ
    0x02, 0xB7, 0x01, // Source eq enable
    0x01, 0x11, // Sleep out
    0x02, 0xff, 100, // delay 100
    0x02, 0x72, 0x13, // gate setting
    0x02, 0xB0, 0x3F, // Duty Setting
    0x03, 0xC7, 0xA6, 0xE9, // OSC Setting
    0x02, 0xD6, 0x00, // VSHLSEL source voltage select
    0x02, 0x36, 0x00, // Memory Access Mode
    0x02, 0x3A, 0x11, // Data Format
    0x02, 0xB9, 0x23, // Source Setting
    0x02, 0xB8, 0x09, // Source Setting
    0x02, 0x35, 0x00, // tearing effect line on
//    0x02, 0xD0, 0x1F, // enable auto power down
    0x01, 0x38, // High power mode
    0x01, 0x29, // display on
    0x00
};
#endif // !WIMPY_MCU

// Definitions for each supported panel
// The order tracks that of the enumerated panel types
// ** ONLY ADD NEW PANELS TO THE END OF THE LIST **
//
const EPD_PANEL panelDefs[] = {
    {400, 300, epd42_init_sequence_full, NULL, epd42_init_sequence_part, 0, BBEI_CHIP_UC8151}, // EPD42_400x300
    {400, 300, epd42b_init_sequence_full, epd42b_init_sequence_fast, epd42b_init_sequence_part, 0, BBEI_CHIP_SSD16xx}, // EPD42B_400x300
    {122, 250, epd213b_init_sequence_full, NULL, epd213b_init_sequence_part, 0, BBEI_CHIP_SSD16xx}, // EPD213B_122x250
    {128, 296, epd293_init_sequence_full, epd293_init_sequence_fast, epd293_init_sequence_part, 0, BBEI_CHIP_SSD16xx}, // EPD293_128x296
    {128, 296, epd294_init_sequence_full, NULL, NULL, 0, BBEI_CHIP_SSD16xx}, // EPD294_128x296
    {128, 296, epd293_init_sequence_full, epd293_init_sequence_fast, epd295_init_sequence_part, 0, BBEI_CHIP_SSD16xx}, // EPD295_128x296
    {152, 296, epd266_init_sequence_full, NULL, epd266_init_sequence_part, 0, BBEI_CHIP_SSD16xx}, // EPD266_152x296
    {80, 128, epd102_init_sequence_full, NULL, epd102_init_sequence_part, 0, BBEI_CHIP_UC8151}, // EPD102_80x128
    {176, 264, epd27_init_sequence_full, NULL, epd27_init_sequence_part, 0, BBEI_CHIP_SSD16xx}, // EPD27B_176x264
};
int bbeiSetPanelType(BBEIDISP *pBBEI, int iPanel)
{
    if (pBBEI == NULL || iPanel <= EPD_PANEL_UNDEFINED || iPanel >= EPD_PANEL_COUNT)
        return BBEI_ERROR_BAD_PARAMETER;
    
    memset(&pBBEI, 0, sizeof(BBEIDISP));
    pBBEI->native_width = pBBEI->width = panelDefs[iPanel].width;
    pBBEI->native_height = pBBEI->height = panelDefs[iPanel].height;
    pBBEI->chip_type = panelDefs[iPanel].chip_type;
    pBBEI->iFlags = panelDefs[iPanel].flags;
    pBBEI->pInitFull = panelDefs[iPanel].pInitFull;
    pBBEI->pInitFast = panelDefs[iPanel].pInitFast;
    pBBEI->pInitPart = panelDefs[iPanel].pInitPart;
    return BBEI_SUCCESS;
} /* bbeiSetPanelType() */

void bbeiSetPosition(OBDISP *pBBEI, int x, int y, int w, int h)
{
    uint8_t uc[12];
    int i, tx, ty;

            tx = x/8; // round down to next lower byte
            ty = y;
            cx = (cx + 7) & 0xfff8; // make width an even number of bytes
    if (pBBEI->chip_type == EEBI_CHIP_UC8151) {
        bbeiWriteCmd(pBBEI, UC8151_PTIN); // partial in
        bbeiWriteCmd(pBBEI, UC8151_PTL); // partial window
        i = 0;
        tx *= 8;
        if (iWidth >= 256) { // need 2 bytes per x
            uc[i++] = (uint8_t)(tx>>8); // start x
            uc[i++] = (uint8_t)tx;
            uc[i++] = (uint8_t)((tx+cx-1)>>8); // end x
            uc[i++] = (uint8_t)((tx+cx-1) | 7);
        } else {
            uc[i++] = tx; // start x
            uc[i++] = (tx+cx-1) | 7; // end x
        }
        if (iHeight >= 256) {
            uc[i++] = (uint8_t)(ty>>8); // start y
            uc[i++] = (uint8_t)ty;
            uc[i++] = (uint8_t)((ty+cy-1)>>8); // end y
            uc[i++] = (uint8_t)(ty+cy-1);
        } else {
            uc[i++] = (uint8_t)ty;
            uc[i++] = (uint8_t)(ty+cy-1);
        }
        uc[i++] = 1; // refresh whole screen (0=refresh partial window only)
        bbeiWriteData(pBBEI, uc, i);
        //       EPDWriteCmd(UC8151_PTOU); // partial out
    } else { // SSD16xx
        bbeiWriteCmd(pBBEI, SSD1608_SET_RAMXPOS);
        uc[0] = tx; // start x (byte boundary)
        uc[1] = tx+((cx-1)>>3); // end x
        bbeiWriteData(pBBEI, uc, 2);
        bbeiWriteCmd(pBBEI, SSD1608_SET_RAMYPOS);
        uc[0] = (uint8_t)ty; // start y
        uc[1] = (uint8_t)(ty>>8);
        uc[2] = (uint8_t)(ty+cy-1); // end y
        uc[3] = (uint8_t)((ty+cy-1)>>8);
        bbeiWriteData(pBBEI, uc, 4);
        // set ram counter to start of this region
        bbeiCMD2(pBBEI, SSD1608_SET_RAMXCOUNT, tx);
        uc[0] = ty;
        uc[1] = (ty>>8);
        bbeiWriteCmd(pBBEI, SSD1608_SET_RAMYCOUNT);
        bbeiWriteData(pBBEI, uc, 2);
        //    EPD_CMD2(SSD1608_DATA_MODE, 0x3);
    }
} /* bbeiSetPosition() */

void obdWriteCommand(OBDISP *pBBEI, unsigned char c);
void obdWriteDataBlock(OBDISP *pBBEI, unsigned char *ucBuf, int iLen, int bRender);
const uint8_t ucMirror[256] PROGMEM =
     {0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
      8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
      4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
      12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
      2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
      10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
      6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
      14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254,
      1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241,
      9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
      5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
      13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253,
      3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243,
      11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251,
      7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
      15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255};

//
// Draw the contents of a memory buffer onto a display
// The sub-window will be clipped if it specifies too large an area
// for the destination display. The source OBDISP structure must have
// a valid back buffer defined
// The top and bottom destination edges will be drawn on byte boundaries (8 rows)
// The source top/bot edges can be on pixel boundaries
// This can be used for partial screen updates
//
void obdDumpWindow(OBDISP *pBBEISrc, OBDISP *pBBEIDest, int srcx, int srcy, int destx, int desty, int width, int height)
{
uint8_t *s,ucTemp[32]; // temp buffer to gather source pixels
int x, y, tx, i;
int iPitch;

   if (pBBEISrc == NULL || pBBEIDest == NULL || pBBEISrc->ucScreen == NULL)
      return; // invalid pointers
   if (width > pBBEIDest->width)
      width = pBBEIDest->width;
   if (height > pBBEIDest->height)
      height = pBBEIDest->height;
   iPitch = pBBEISrc->width;
   for (y=0; y<height; y+=8)
   {
      obdSetPosition(pBBEIDest, destx, (desty+y)/8, 1);
      for (x=0; x<width; x+=32)
      {
         tx = 32;
         if (width-x < 32) tx = width-x;
         s = &pBBEISrc->ucScreen[((srcy+y)/8)*iPitch + srcx+x];
     if (srcy & 7) // need to shift the bits to get 8 rows of src data
         {
            uint8_t uc, ucShift = srcy & 7;
            for (i=0; i<tx; i++)
            { // combine current and next line to capture 8 pixels
               uc = s[0] >> ucShift;
               uc |= s[iPitch] << (7-ucShift);
               ucTemp[i] = uc;
            }
            obdCachedWrite(pBBEIDest, ucTemp, tx, 1);
         }
         else
         { // simpler case
            obdCachedWrite(pBBEIDest, s, tx, 1); // just copy it
         }
      } // for x
   } // for y
   obdCachedFlush(pBBEIDest, 1);
} /* obdDumpWindow() */

//
// Write a subset of lines to a Sharp memory LCD
//
void obdWriteLCDLines(OBDISP *pBBEI, int iStart, int iCount)
{
    int x, y, j;
    uint8_t uc, c, ucMask, *s, *d, ucStart;
    uint8_t ucLineBuf[56]; // 400 pixels is max supported width = 50 bytes + 4
    int iPitch = pBBEI->width / 8;
    static int iVCOM = 0;

    if (pBBEI == NULL || pBBEI->chip_type != BBEI_CHIP_SHARP || pBBEI->ucScreen == NULL || iStart < 0 || iStart > pBBEI->native_height-1 || iStart+iCount < 0 || iStart+iCount > pBBEI->native_height-1) {
        return; // invalid request
    }
    
      digitalWrite(pBBEI->iCSPin, HIGH); // active high

      ucStart = 0x80; // write command
      iVCOM++;
      if (iVCOM & 0x40) // flip it every 64 lines
        ucStart |= 0x40; // VCOM bit
      ucLineBuf[0] = ucStart;
      RawWriteData(pBBEI, ucLineBuf, 1); // write command(01) + vcom(02)

     for (y=iStart; y<iStart+iCount; y++) {
     	d = &ucLineBuf[1];
        ucMask = 1 << (y & 7);
        s = &pBBEI->ucScreen[(y >> 3) * pBBEI->native_width];
     	ucLineBuf[0] = pgm_read_byte(&ucMirror[y+1]); // current line number
     	for (x=0; x<iPitch; x++) {
           uc = 0xff;
           for (j=7; j>=0; j--) {
               c = *s++;
               if (c & ucMask) uc ^= (1 << j);
           }
           *d++ = uc;
        } // for x
        // write this line to the display
        ucLineBuf[iPitch+1] = 0; // end of line
        RawWriteData(pBBEI, ucLineBuf, iPitch+2);
     } // for y
    ucLineBuf[0] = 0;
    RawWriteData(pBBEI, ucLineBuf, 1); // final transfer
    digitalWrite(pBBEI->iCSPin, LOW); // de-activate
} /* obdWriteLCDLines() */

//
// Turn the display on or off
//
void obdPower(OBDISP *pBBEI, int bOn)
{
uint8_t ucCMD;
    
    if (!pBBEI) return;
    switch (pBBEI->type) {
        case LCD_NOKIA5110:
            ucCMD = (bOn) ? 0x20 : 0x24;
            break;
#if !defined( WIMPY_MCU ) && !defined(__AVR__)
        case LCD_ST7302:
            ucCMD = (bOn) ? 0x29 : 0x28;
            break;
#endif
        default: // all other supported displays
            ucCMD = (bOn) ? 0xaf : 0xae;
            break;
        // e-ink panels are put to sleep after each action
    }
  obdWriteCommand(pBBEI, ucCMD);
} /* obdPower() */

// Controls the LED backlight
void obdBacklight(OBDISP *pBBEI, int bOn)
{
  if (pBBEI->iLEDPin != 0xff)
  {
     digitalWrite(pBBEI->iLEDPin, (bOn) ? HIGH:LOW);
  }
} /* obdBacklight() */

//
// Send the command sequence to power up the LCDs
static void LCDPowerUp(OBDISP *pBBEI)
{
#ifndef MEMORY_ONLY
    int iLen;
    uint8_t *s;
    u8Cache[0] = 0x00; // send everything in command mode
    if (pBBEI->type == LCD_UC1701 || pBBEI->type == LCD_UC1609)
    {
        s = (uint8_t *)uc1701_initbuf;
        iLen = sizeof(uc1701_initbuf);
    }
    else if (pBBEI->type == LCD_HX1230)
    {
        s = (uint8_t *)hx1230_initbuf;
        iLen = sizeof(hx1230_initbuf);
    }
    else if (pBBEI->type == LCD_NOKIA5110) // Nokia 5110
    {
        s = (uint8_t *)nokia5110_initbuf;
        iLen = sizeof(nokia5110_initbuf);
    }
    else if (pBBEI->type == LCD_32x64)
    {
        s = (uint8_t *)st7567_initbuf;
        iLen = sizeof(st7567_initbuf);
    }
    memcpy_P(&u8Cache[1], s, iLen);
    RawWrite(pBBEI, u8Cache, iLen+1);
    _delay(100);
    obdWriteCommand(pBBEI, 0xa5);
    _delay(100);
    obdWriteCommand(pBBEI, 0xa4);
    obdWriteCommand(pBBEI, 0xaf);
#endif // MEMORY_ONLY
} /* LCDPowerUp() */
//
// More efficient means of sending commands, data and busy-pauses
//
void EPDSendCMDSequence(OBDISP *pBBEI, const uint8_t *pSeq)
{
int iLen;
uint8_t *s;
    
    s = (uint8_t *)pSeq;
    while (pgm_read_byte(s) != 0) {
        iLen = pgm_read_byte(s++);
        if (iLen == BUSY_WAIT) {
            Serial.println("Calling busy_wait from cmd list");
            EPDWaitBusy(pBBEI, 1);
        } else {
            obdWriteCommand(pBBEI, pgm_read_byte(s));
            s++;
            if (iLen > 1) {
               memcpy_P(u8Cache, s, iLen-1);
               s += (iLen-1);
               RawWriteData(pBBEI, u8Cache, iLen-1);
            }
        }
    } // while more commands to send
} /* EPDSendCMDSequence() */

void EPD29_Init(OBDISP *pBBEI)
{
uint8_t ucTemp[4];
    
    obdWriteCommand(pBBEI, UC8151_PON); // power on
    EPDWaitBusy(pBBEI, 1);
    EPD_CMD2(pBBEI, UC8151_PSR, 0x80 | 0x10 | 0x00 | 0x02 | 0x01); // panel setting
    obdWriteCommand(pBBEI, UC8151_TRES); // resolution
    ucTemp[0] = (uint8_t)pBBEI->native_width;
    ucTemp[1] = (uint8_t)(pBBEI->native_height >> 8);
    ucTemp[2] = (uint8_t)(pBBEI->native_height & 0xff);
    RawWriteData(pBBEI, ucTemp, 3);
//    obdWriteCommand(pBBEI, UC8151_CDI);
    EPDSleep(pBBEI, 0);
} /* EPD29_Init() */

const unsigned char lut_w_full[] PROGMEM =
{
  0x60, 0x5A, 0x5A, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_b_full[] PROGMEM =
{
  0x90, 0x5A, 0x5A, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#ifdef FUTURE
const uint8_t epd213_lut[] PROGMEM =
{
	// black
	0x48, 0x50, 0x10, 0x10, 0x13, 0x00, 0x00,
	0x48, 0x50, 0x80, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x48, 0x55, 0x00, 0xbb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x10, 0x04, 0x04, 0x04, 0x04,
	0x10, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x08, 0x08, 0x10, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

static const uint8_t epd294_lut_partial[] PROGMEM = {
0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0A,0x0,0x0,0x0,0x0,0x0,0x2,  
0x1,0x0,0x0,0x0,0x0,0x0,0x0,
0x1,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,
//0x22,0x17,0x41,0xB0,0x32,0x36,
};

const uint8_t epd213_lut2_partial[] PROGMEM =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT0: BB:     VS 0 ~7
    0x80,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT1: BW:     VS 0 ~7
    0x40,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT2: WB:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT3: WW:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT4: VCOM:   VS 0 ~7

    0x0A,0x00,0x00,0x00,0x00,                       // TP0 A~D RP0
    0x00,0x00,0x00,0x00,0x00,                       // TP1 A~D RP1
    0x00,0x00,0x00,0x00,0x00,                       // TP2 A~D RP2
    0x00,0x00,0x00,0x00,0x00,                       // TP3 A~D RP3
    0x00,0x00,0x00,0x00,0x00,                       // TP4 A~D RP4
    0x00,0x00,0x00,0x00,0x00,                       // TP5 A~D RP5
    0x00,0x00,0x00,0x00,0x00,                       // TP6 A~D RP6

};

const uint8_t epd213_lut2_full[] PROGMEM =
{
	// black
	0x80,0x60,0x40,0x00,0x00,0x00,0x00,
	0x10,0x60,0x20,0x00,0x00,0x00,0x00,
	0x80,0x60,0x40,0x00,0x00,0x00,0x00,
	0x10,0x60,0x20,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x03,0x03,0x00,0x00,0x02,
	0x09,0x09,0x00,0x00,0x02,
	0x03,0x03,0x00,0x00,0x02,
	0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00
};

const uint8_t lut154_full_update[] PROGMEM =
    {
        0x80,0x48,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
        0x40,0x48,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
        0x80,   0x48,   0x40,   0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x40,   0x48,   0x80,   0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0xA,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x8,    0x1,    0x0,    0x8,    0x1,    0x0,    0x2,
        0xA,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x22,   0x22,   0x22,   0x22,   0x22,   0x22,   0x0,    0x0,    0x0
    };

const uint8_t lut154_partial_update[] PROGMEM =
{
    0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0xF,0x0,0x0,0x0,0x0,0x0,0x0,
    0x1,0x1,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0
};

//
// Initialize the display controller on an SPI bus
//
void obdSPIInit(OBDISP *pBBEI, int iType, int iDC, int iCS, int iReset, int iMOSI, int iCLK, int iLED, int bFlip, int bInvert, int bBitBang, int32_t iSpeed)
{
#ifndef MEMORY_ONLY
    uint8_t *s=NULL;
    int iLen=0;
    
    if (pBBEI == NULL)
        return;
    pBBEI->iTimeout = 5000; // 5 seconds for most e-ink panels
    pBBEI->can_flip = 1; // assume can flip 180
    pBBEI->ucScreen = NULL; // start with no backbuffer; user must provide one later
    pBBEI->x_offset = 0; // e-paper memory column offset
    pBBEI->iDCPin = iDC;
    pBBEI->iCSPin = iCS;
    pBBEI->iRSTPin = iReset;
    pBBEI->iMOSIPin = iMOSI;
    pBBEI->iCLKPin = iCLK;
    pBBEI->iLEDPin = iLED;
    pBBEI->type = iType;
    pBBEI->chip_type = BBEI_CHIP_SSD13xx; // assume common OLEDs
    pBBEI->flip = bFlip;
    pBBEI->invert = bInvert;
    pBBEI->bBitBang = bBitBang;
    pBBEI->wrap = 0; // default - disable text wrap
    pBBEI->com_mode = COM_SPI; // communication mode
    if (pBBEI->iDCPin != 0xff) // Note - not needed on Sharp Memory LCDs
    {
        pinMode(pBBEI->iDCPin, OUTPUT);
        digitalWrite(pBBEI->iDCPin, 0); // for some reason, command mode must be set or some OLEDs/LCDs won't initialize correctly even if set later
    }
    if (pBBEI->iCSPin != 0xff)
    {
        pinMode(pBBEI->iCSPin, OUTPUT);
        if (iType == SHARP_400x240 || iType == SHARP_128x128 || iType == SHARP_160x68) {
            digitalWrite(pBBEI->iCSPin, LOW);
        } else {
            digitalWrite(pBBEI->iCSPin, HIGH); // set to not-active
        }
    }
    if (bBitBang)
    {
        pinMode(iMOSI, OUTPUT);
        pinMode(iCLK, OUTPUT);
    }
    
    // Reset it
    if (pBBEI->iRSTPin != 0xff)
    {
        pinMode(pBBEI->iRSTPin, OUTPUT);
        digitalWrite(pBBEI->iRSTPin, HIGH);
        _delay(200);
        digitalWrite(pBBEI->iRSTPin, LOW);
        _delay(10);
        digitalWrite(pBBEI->iRSTPin, HIGH);
        _delay(200);
    }
    if (iLED != -1 && iLED != 0xff)
    {
        if (iType >= EPD42_400x300) // for EPD displays, LED = BUSY
            pinMode(iLED, INPUT_PULLUP);
        else
            pinMode(iLED, OUTPUT);
    }
    initSPI(pBBEI, iSpeed, iMOSI, iCLK, iCS);
    pBBEI->native_width = pBBEI->width = 128; // assume 128x64
    pBBEI->height = 64;
#if !defined( WIMPY_MCU ) && !defined(__AVR__)
    if (iType == LCD_ST7302)
    {
        pBBEI->native_width = pBBEI->width = 250;
        pBBEI->native_height = pBBEI->height = 122;
        pBBEI->can_flip = 0;
    }
#endif
    if (iType == OLED_80x128)
    {
        pBBEI->native_width = pBBEI->width = 80;
        pBBEI->native_height = pBBEI->height = 128;
    }
    else if (iType == SHARP_160x68)
    {     
        pBBEI->native_width = pBBEI->width = 160;
        pBBEI->native_height = pBBEI->height = 68;
        pBBEI->chip_type = BBEI_CHIP_SHARP;
        pBBEI->can_flip = 0;
        pBBEI->iDCPin = 0xff; // no D/C wire on this display
    } 
    else if (iType == SHARP_128x128)
    {
        pBBEI->native_width = pBBEI->width = 128;
        pBBEI->native_height = pBBEI->height = 128;
        pBBEI->chip_type = BBEI_CHIP_SHARP;
        pBBEI->can_flip = 0;
        pBBEI->iDCPin = 0xff; // no D/C wire on this display
    }
    else if (iType == SHARP_144x168)
    {
        pBBEI->native_width = pBBEI->width = 144;
        pBBEI->native_height = pBBEI->height = 168;
        pBBEI->chip_type = BBEI_CHIP_SHARP;
        pBBEI->can_flip = 0;
        pBBEI->iDCPin = 0xff; // no D/C wire on this display
    }
    else if (iType == SHARP_400x240)
    {
        pBBEI->native_width = pBBEI->width = 400;
        pBBEI->native_height = pBBEI->height = 240;
        pBBEI->chip_type = BBEI_CHIP_SHARP;
        pBBEI->can_flip = 0;
        pBBEI->iDCPin = 0xff; // no D/C wire on this display
        pBBEI->iOrientation = 0;
        return;
    }
    else if (iType == EPD583_648x480) {
        pBBEI->native_width = pBBEI->width = 648;
        pBBEI->native_height = pBBEI->height = 480;
        pBBEI->busy_idle = HIGH;
        pBBEI->can_flip = 0;
        pBBEI->chip_type = BBEI_CHIP_UC8151;
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        pBBEI->type = EPD583_648x480; // same for the rest
        pBBEI->pInitFull = epd583_init_sequence_full;
        pBBEI->pInitPart = epd583_init_sequence_part;
        return; // nothing else to do yet
    }
#ifndef __AVR__
    else if (iType == EPD75_800x480) {
        pBBEI->native_width = pBBEI->width = 800;
        pBBEI->native_height = pBBEI->height = 480;
        pBBEI->busy_idle = HIGH;
        pBBEI->can_flip = 0;
        pBBEI->chip_type = BBEI_CHIP_UC8151;
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        pBBEI->type = EPD75_800x480; // same for the rest
        pBBEI->pInitFull = epd75_init_sequence_full;
        pBBEI->pInitFast = epd75_init_sequence_fast;
        pBBEI->pInitPart = epd75_init_sequence_partial;
        return; // nothing else to do yet
    }
#endif // __AVR__
    else if (iType == EPD294_128x296)
    {
        pBBEI->native_width = pBBEI->width = 128;
        pBBEI->native_height = pBBEI->height = 296;
        pBBEI->busy_idle = LOW;
        pBBEI->can_flip = 0;
        //pBBEI->x_offset = 2; // column byte offset
        pBBEI->chip_type = BBEI_CHIP_SSD16xx;
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        pBBEI->pInitFull = epd294_init_sequence_full;
        pBBEI->type = EPD293_128x296; // same for the rest
        return; // nothing else to do yet
    }
    else if (iType == EPD102_80x128)
    {
        pBBEI->native_width = pBBEI->width = 80;
        pBBEI->native_height = pBBEI->height = 130;
        pBBEI->busy_idle = HIGH;
        pBBEI->can_flip = 0;
        pBBEI->type = iType;
        pBBEI->chip_type = BBEI_CHIP_UC8151;
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        pBBEI->pInitFull = epd102_init_sequence_full;
        pBBEI->pInitPart = epd102_init_sequence_part;
        return; // nothing else to do yet
    }
    else if (iType == EPD42B_400x300)
    {
        pBBEI->native_width = pBBEI->width = 400;
        pBBEI->native_height = pBBEI->height = 300;
        pBBEI->busy_idle = LOW;
        pBBEI->can_flip = 0;
        pBBEI->type = iType;
        pBBEI->chip_type = BBEI_CHIP_SSD16xx;
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        pBBEI->pInitFull = epd42b_init_sequence_full;
        pBBEI->pInitFast = epd42b_init_sequence_fast;
        pBBEI->pInitPart = epd42b_init_sequence_part;
        return; // nothing else to do yet
    }
    else if (iType == EPD27B_176x264)
    {
        pBBEI->native_width = pBBEI->width = 176;
        pBBEI->native_height = pBBEI->height = 264;
        pBBEI->busy_idle = LOW;
        pBBEI->can_flip = 0;
        pBBEI->chip_type = BBEI_CHIP_SSD16xx;
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        pBBEI->pInitFull = epd27_init_sequence_full;
        pBBEI->pInitPart = epd27_init_sequence_part;
        return; // nothing else to do yet
    }
    else if (iType == EPD27_176x264)
    {
        pBBEI->native_width = pBBEI->width = 176;
        pBBEI->native_height = pBBEI->height = 264;
        pBBEI->busy_idle = HIGH;
        pBBEI->can_flip = 0;
        pBBEI->chip_type = BBEI_CHIP_UC8151;
        pBBEI->iFlags |= BBEI_CS_EVERY_BYTE; // unfortunately needs this
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        return; // nothing else to do yet
    }
    else if (iType == EPD579_792x272) {
        pBBEI->native_width = pBBEI->width = 792;
        pBBEI->native_height = pBBEI->height = 272;
        pBBEI->pInitFull = epd579_init_sequence_full;
        pBBEI->busy_idle = LOW;
        pBBEI->chip_type = BBEI_CHIP_SSD16xx;
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        pBBEI->iFlags |= BBEI_CS_EVERY_BYTE;
        pBBEI->can_flip = 0;
    }
    else if (iType == EPD266_152x296 || iType == EPD266B_152x296) {
        pBBEI->native_width = pBBEI->width = 152;
        pBBEI->native_height = pBBEI->height = 296;
        pBBEI->pInitFull = epd266_init_sequence_full;
        pBBEI->pInitPart = epd266_init_sequence_part;
        pBBEI->busy_idle = LOW;
        pBBEI->chip_type = BBEI_CHIP_SSD16xx;
        pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
        pBBEI->can_flip = 0;
        pBBEI->type = iType;
      //  pBBEI->x_offset = (iType == EPD266B_152x296) ? 0:1;
        return; // nothing else to do yet
    }
    else if (iType == EPD42_400x300 || iType == EPD42_4GRAY_400x300)
    { // WFT0420CZ15
        pBBEI->native_width = pBBEI->width = 400;
        pBBEI->native_height = pBBEI->height = 300;
        pBBEI->busy_idle = HIGH;
        pBBEI->chip_type = BBEI_CHIP_UC8151;
        pBBEI->can_flip = 0;
        if (iType == EPD42_400x300) {
            pBBEI->pInitFull = epd42_init_sequence_full;
            pBBEI->pInitPart = epd42_init_sequence_part;
            pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
            pBBEI->iFlags |= BBEI_FAST_INVERTED;
        } else {
            pBBEI->pInitFull = epd42_4gray_init_sequence_full;
            pBBEI->iFlags |= BBEI_4COLOR | BBEI_4GRAY;
        }
        return; // nothing else to do yet
    }
    else if (iType == EPD293_128x296 || iType == EPD213B_122x250 || iType == EPD295_128x296)
    {
        if (iType == EPD213B_122x250) {
            pBBEI->native_width = pBBEI->width = 122;
            pBBEI->native_height = pBBEI->height = 250;
            pBBEI->pInitFull = epd213b_init_sequence_full;
            pBBEI->pInitPart = epd213b_init_sequence_part;
        } else if (iType == EPD293_128x296) {
            pBBEI->native_width = pBBEI->width = 128;
            pBBEI->native_height = pBBEI->height = 296;
            pBBEI->pInitFull = epd293_init_sequence_full;
            pBBEI->pInitFast = epd293_init_sequence_fast;
            pBBEI->pInitPart = epd293_init_sequence_part;
        } else { // EPD295_128x296
            pBBEI->native_width = pBBEI->width = 128;
            pBBEI->native_height = pBBEI->height = 296;
            pBBEI->pInitFull = epd293_init_sequence_full;
            pBBEI->pInitFast = epd293_init_sequence_fast;
            pBBEI->pInitPart = epd295_init_sequence_part;
        }
        pBBEI->busy_idle = LOW;
        pBBEI->chip_type = BBEI_CHIP_SSD16xx;
        pBBEI->iFlags |= (BBEI_HAS_PARTIAL_UPDATE | BBEI_HAS_FAST_UPDATE);
        pBBEI->can_flip = 0;
        return; // nothing else to do yet
    } else if (iType == EPD29_BWYR_128x296) {
      pBBEI->native_width = pBBEI->width = 128;
      pBBEI->native_height = pBBEI->height = 296;
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_CS_EVERY_BYTE;
      pBBEI->iFlags |= BBEI_4COLOR;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iTimeout = 25000; // 4-color need a longer timeout (20 seconds)
      pBBEI->pInitFull = epd29b_bwyr_init_sequence_full;
      return;
    } else if (iType == EPD29_BWYR_168x384) {
      pBBEI->native_width = pBBEI->width = 168;
      pBBEI->native_height = pBBEI->height = 384;
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_4COLOR;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iTimeout = 25000; // 4-color need a longer timeout (20 seconds)
      pBBEI->pInitFull = epd29_bwyr_init_sequence_full;
      return;
    } else if (iType == EPD266_BWYR_184x360) {
      pBBEI->native_width = pBBEI->width = 184;
      pBBEI->native_height = pBBEI->height = 360; 
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_4COLOR;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iTimeout = 20000; // 4-color need a longer timeout (20 seconds)
      pBBEI->pInitFull = epd266_bwyr_init_sequence_full;
      EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
      return;
    } else if (iType == EPD236_BWYR_168x296) {
      pBBEI->native_width = pBBEI->width = 168;
      pBBEI->native_height = pBBEI->height = 296; 
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_4COLOR; 
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iFlags |= BBEI_CS_EVERY_BYTE;
      pBBEI->iTimeout = 25000; // 4-color need a longer timeout (20 seconds)
      pBBEI->pInitFull = epd236_bwyr_init_sequence_full;
      return;
    } else if (iType == EPD164_BWYR_168x168) {
      pBBEI->native_width = pBBEI->width = 168;
      pBBEI->native_height = pBBEI->height = 168; 
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_4COLOR; 
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iFlags |= BBEI_CS_EVERY_BYTE;
      pBBEI->iTimeout = 25000; // 4-color need a longer timeout (20 seconds)
      pBBEI->pInitFull = epd164_bwyr_init_sequence_full;
      return;
    } else if (iType == EPD30_BWYR_168x400) {
      pBBEI->native_width = pBBEI->width = 168; 
      pBBEI->native_height = pBBEI->height = 400;
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_4COLOR;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iFlags |= BBEI_CS_EVERY_BYTE;
      pBBEI->iTimeout = 20000; // 4-color need a longer timeout (20 seconds)
      pBBEI->pInitFull = epd30_bwyr_init_sequence_full;
      return;
  } else if (iType == EPD213_122x250 || iType == EPD213_104x212 || iType == EPD29B_128x296)
    {
        if (iType == EPD213_122x250) {
            pBBEI->native_width = pBBEI->width = 122;
            pBBEI->native_height = pBBEI->height = 250;
            pBBEI->iFlags |= BBEI_HAS_FAST_UPDATE;
            pBBEI->pInitFull = epd213_122x250_init_sequence_full;
            pBBEI->pInitPart = epd213_122x250_init_sequence_part;
        } else if (iType == EPD213_104x212) {
            pBBEI->native_width = pBBEI->width = 104;
            pBBEI->native_height = pBBEI->height = 212;
            pBBEI->pInitFull = epd213_104x212_init_sequence_full;
        } else {
            pBBEI->native_width = pBBEI->width = 128;
            pBBEI->native_height = pBBEI->height = 296;
            pBBEI->pInitFull = epd29b_init_sequence_full;
        }
        pBBEI->can_flip = 0;
        pBBEI->busy_idle = LOW;
        pBBEI->iFlags |= BBEI_HAS_FAST_UPDATE;
        pBBEI->chip_type = BBEI_CHIP_SSD16xx;
        return;
#if !defined( WIMPY_MCU ) && !defined(__AVR__)
} else if (iType == EPD74R_640x384 || iType == EPD583R_600x448) {
      if (iType == EPD583R_600x448) {
          pBBEI->native_width = pBBEI->width = 600;
          pBBEI->native_height = pBBEI->height = 448;
          pBBEI->type = EPD74R_640x384;
      } else {
          pBBEI->native_width = pBBEI->width = 640;
          pBBEI->native_height = pBBEI->height = 384;
      }
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_3COLOR;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
//      pBBEI->iFlags |= BBEI_CS_EVERY_BYTE;
      pBBEI->iTimeout = 25000; // 3-color need a longer timeout (25 seconds)
      return;
#endif // !WIMPY_MCU
  } else if (iType == EPD42R2_400x300 || iType == EPD213R_104x212 || iType == EPD37Y_240x416) {
      if (iType == EPD37Y_240x416) {
          pBBEI->native_width = pBBEI->width = 240;
          pBBEI->native_height = pBBEI->height = 416;
          pBBEI->type = EPD42R2_400x300; // use the same commands
      }else if (iType == EPD213R_104x212) {
          pBBEI->native_width = pBBEI->width = 104;
          pBBEI->native_height = pBBEI->height = 212;
          pBBEI->type = EPD42R2_400x300; // use the same commands
      } else {
          pBBEI->native_width = pBBEI->width = 400;
          pBBEI->native_height = pBBEI->height = 300;
      }
      pBBEI->pInitFull = epd42r2_init_sequence_full;
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_3COLOR;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iTimeout = 25000; // 3-color need a longer timeout (25 seconds)
      EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
  } else if (iType == EPD213R_104x212_d) {
      pBBEI->native_width = pBBEI->width = 104;
      pBBEI->native_height = pBBEI->height = 212;
      pBBEI->type = EPD213R_104x212_d;
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = HIGH;
      pBBEI->iFlags |= BBEI_3COLOR;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iTimeout = 25000; // 3-color need a longer timeout (25 seconds)
  }
  else if (iType == EPD35R_184x384) {
      pBBEI->native_width = pBBEI->width = 184;
      pBBEI->native_height = pBBEI->height = 384;
      pBBEI->iTimeout = 25000;
      pBBEI->iFlags |= BBEI_3COLOR;
      pBBEI->chip_type = BBEI_CHIP_SSD16xx;
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = LOW;
      pBBEI->pInitFull = epd35r_init_sequence_full;
      return;
  }
  else if (iType == EPD29R_128x296 || iType == EPD154R_152x152 || iType == EPD42R_400x300 || iType == EPD31R_168x296 || iType == EPD154Y_152x152 || iType == EPD29Y_128x296 || iType == EPD213R2_122x250 || iType == EPD266Y_152x296 || iType == EPD42Y_400x300)
  { // BLACK/WHITE/RED (or yellow)
      if (iType == EPD266Y_152x296) {
          pBBEI->native_width = pBBEI->width = 152;
          pBBEI->native_height = pBBEI->height = 296;
          pBBEI->pInitFull = epd29r_init_sequence;
          pBBEI->x_offset = 1;
      } else  if (iType == EPD31R_168x296) {
          pBBEI->native_width = pBBEI->width = 168;
          pBBEI->native_height = pBBEI->height = 296;
          pBBEI->pInitFull = epd29r_init_sequence;
      } else if (iType == EPD29Y_128x296) {
          pBBEI->native_width = pBBEI->width = 128;
          pBBEI->native_height = pBBEI->height = 296;
          pBBEI->pInitFull = epd29r_init_sequence;
          pBBEI->x_offset = 1;
      } else if (iType == EPD29R_128x296) {
          pBBEI->native_width = pBBEI->width = 128;
          pBBEI->native_height = pBBEI->height = 296;
          pBBEI->iFlags |= BBEI_HAS_FAST_UPDATE;
          pBBEI->pInitFull = epd29r_init_sequence;
      } else if (iType == EPD154Y_152x152) {
          pBBEI->native_width = pBBEI->width = 152;
          pBBEI->native_height = pBBEI->height = 152;
          pBBEI->pInitFull = epd29r_init_sequence_152;
          pBBEI->x_offset = 1;
      } else if (iType == EPD154R_152x152){
          pBBEI->native_width = pBBEI->width = 152;
          pBBEI->native_height = pBBEI->height = 152;
          pBBEI->pInitFull = epd29r_init_sequence_152;
      } else if (iType == EPD213R2_122x250) {
          pBBEI->native_width = pBBEI->width = 122;
          pBBEI->native_height = pBBEI->height = 250;
          pBBEI->pInitFull = epd29r_init_sequence;
          pBBEI->x_offset = 1;
      } else { // EPD42R_400x300 and EPD42Y_400x300
          pBBEI->native_width = pBBEI->width = 400;
          pBBEI->native_height = pBBEI->height = 300;
          pBBEI->pInitFull = epd42r_init_sequence;
      }
      pBBEI->type = EPD29R_128x296; // use the rest of the same code
      pBBEI->iTimeout = 25000; // 3-color need a longer timeout (25 seconds)
      pBBEI->iFlags |= BBEI_3COLOR;
      pBBEI->chip_type = BBEI_CHIP_SSD16xx;
      pBBEI->can_flip = 0;
      pBBEI->busy_idle = LOW;
//      EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
      return;
  }
  else if (iType == EPD154_152x152) // GDEW0154M10
  {
      pBBEI->native_width = pBBEI->width = 152;
      pBBEI->native_height = pBBEI->height = 152;
      pBBEI->can_flip = 0; // flip display commands don't exist
      pBBEI->busy_idle = HIGH;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
//      pBBEI->iFlags |= BBEI_CS_EVERY_BYTE;
      pBBEI->pInitFull = epd154a_init_sequence_full;
      pBBEI->pInitPart = epd154a_init_sequence_part;
      return;
  }
  else if (iType == EPD154_200x200 || iType == EPD154B_200x200)
  {
      pBBEI->native_width = pBBEI->width = 200;
      pBBEI->native_height = pBBEI->height = 200;
      pBBEI->can_flip = 0; // flip display commands don't exist
      pBBEI->busy_idle = LOW;
      pBBEI->chip_type = BBEI_CHIP_SSD16xx;
      pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
      pBBEI->pInitFull = epd154_init_sequence_full;
      pBBEI->pInitPart = (iType == EPD154B_200x200) ? epd154b_init_sequence_part : epd154_init_sequence_part;
      return;
  }
  else if (iType == EPD29_128x296 || iType == EPD213B_104x212 || iType == EPD37_240x416)
  {
      pBBEI->iFlags |= BBEI_HAS_PARTIAL_UPDATE;
      pBBEI->chip_type = BBEI_CHIP_UC8151;
      pBBEI->can_flip = 0; // flip display commands don't exist
      if (iType == EPD37_240x416) {
          pBBEI->native_width = pBBEI->width = 240;
          pBBEI->native_height = pBBEI->height = 416;
          pBBEI->busy_idle = HIGH;
          pBBEI->pInitFull = epd37_init_sequence_full;
          pBBEI->pInitPart = epd37_init_sequence_part;
//          EPDSendCMDSequence(pBBEI, epd37_init_sequence_full);
      } else if (iType == EPD29_128x296) {
          pBBEI->native_width = pBBEI->width = 128;
          pBBEI->native_height = pBBEI->height = 296;
          pBBEI->busy_idle = HIGH;
          pBBEI->pInitFull = epd29_init_sequence_full;
 //         EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
      } else {
          pBBEI->native_width = pBBEI->width = 104;
          pBBEI->native_height = pBBEI->height = 212;
          pBBEI->busy_idle = LOW;
          pBBEI->type = EPD29_128x296; // tell it to use this config
//          EPD29_Init(pBBEI);
      }
      return;
  } else if (iType == EPD47_540x960) {
      pBBEI->native_width = pBBEI->width = 540;
      pBBEI->native_height = pBBEI->height = 960;
      pBBEI->can_flip = 0; // flip display commands don't exist
      pBBEI->busy_idle = HIGH;
      return;
  }
  else if (iType == LCD_32x64)
  {
      pBBEI->native_width = pBBEI->width = 32;
      pBBEI->native_height = pBBEI->height = 64;
  }
  else if (iType == LCD_UC1609)
  {
      pBBEI->native_width = pBBEI->width = 192;
      pBBEI->native_height = pBBEI->height = 64;
  }
  else if (iType == LCD_HX1230)
  {
      pBBEI->native_width = pBBEI->width = 96;
      pBBEI->native_height = pBBEI->height = 68;
      pBBEI->iDCPin = 0xff; // flag this as being 3-wire SPI
  }
  else if (iType == LCD_NOKIA5110)
  {
      pBBEI->native_width = pBBEI->width = 84;
      pBBEI->native_height = pBBEI->height = 48;
  }
  else if (iType == OLED_96x16)
  {
      pBBEI->native_width = pBBEI->width = 96;
      pBBEI->native_height = pBBEI->height = 16;
  }
  else if (iType == OLED_64x128)
  {
      // NOTE: 64x128 SH1107 doesn't seem to properly support
      // segment remap, so the init commands are A0/C0 and
      // it can't be flipped 180
      pBBEI->can_flip = 0;
      pBBEI->native_width = pBBEI->width = 64;
      pBBEI->native_height = pBBEI->height = 128;
  }
  else if (iType == OLED_128x32)
      pBBEI->native_height = pBBEI->height = 32;
  else if (iType == OLED_128x128)
      pBBEI->native_height = pBBEI->height = 128;
  else if (iType == OLED_64x48)
  {
      pBBEI->native_width = pBBEI->width = 64;
      pBBEI->native_height = pBBEI->height = 48;
  }
  else if (iType == OLED_64x32)
  {
      pBBEI->native_width = pBBEI->width = 64;
      pBBEI->native_height = pBBEI->height = 32;
  }
  else if (iType == OLED_72x40)
  {
      pBBEI->native_width = pBBEI->width = 72;
      pBBEI->native_height = pBBEI->height = 40;
  }
  if (iType == OLED_80x128)
  {
     s = (uint8_t *)oled80_initbuf;
     iLen = sizeof(oled80_initbuf);
  }
  else if (iType == OLED_128x32 || iType == OLED_96x16)
  {
     s = (uint8_t *)oled32_initbuf;
     iLen = sizeof(oled32_initbuf);
  }
  else if (iType == OLED_64x128)
  {
      s = (uint8_t *)oled64x128_initbuf;
     iLen = sizeof(oled64x128_initbuf);
  }
  else if (iType == OLED_128x128)
  {
     s = (uint8_t *)oled128_initbuf;
     iLen = sizeof(oled128_initbuf);
  }
  else if (iType == OLED_132x64) { // SH1106
     s = (uint8_t *)oled132_initbuf;
     iLen = sizeof(oled132_initbuf);
  }
  else if (iType < LCD_UC1701) // 128x64 and 64x32 oleds
  {
     s = (uint8_t *)oled64_initbuf;
     iLen = sizeof(oled64_initbuf);
  }
    // OLED
  if (iType < LCD_UC1701)
  {
      memcpy_P(u8Cache, s, iLen); // do it from RAM
      RawWrite(pBBEI, u8Cache, iLen);
      _delay(100); // on SPI display this delay is needed or the display
      // never sees the "display on" command at the end of the sequence
      obdWriteCommand(pBBEI, 0xaf); // turn on display
      if (bInvert)
      {
          u8Cache[0] = 0; // command
          u8Cache[1] = 0xa7; // invert command
          RawWrite(pBBEI, u8Cache, 2);
      }
      if (bFlip) // rotate display 180
      {
          u8Cache[0] = 0; // command
          u8Cache[1] = 0xa0;
          RawWrite(pBBEI, u8Cache, 2);
          u8Cache[0] = 0;
          u8Cache[1] = 0xc0;
          RawWrite(pBBEI, u8Cache, 2);
      }
  } // OLED
#if !defined( WIMPY_MCU ) && !defined(__AVR__)
  if (iType == LCD_ST7302)
  {
 //     uint8_t *s = (uint8_t *)st7302_hpm_init;
      const uint8_t *s = (uint8_t *)st7302_wenting; //st7302_lpm_init;
    iLen = 1;

      while (iLen) {
          iLen = pgm_read_byte(s++); // parameter byte count
          if (iLen) {
              if (pgm_read_byte(s) == 0xff) { // delay
                  _delay(pgm_read_byte(&s[1]));
                  s += 2;
              } else {
                  obdWriteCommand(pBBEI, pgm_read_byte(s));
                  if (iLen > 1) {
                      memcpy_P(u8Cache, s, iLen);
                      RawWrite(pBBEI, u8Cache, iLen);
                  }
                  s += iLen;
              }
          }
      } // while commands to transmit
      if (pBBEI->invert)
          obdWriteCommand(pBBEI, 0x21); // inversion on
      // Clear RAM
//      obdWriteCommand(pBBEI, 0xb9);
//      uc[0] = 0x40;
//      uc[1] = 0xe3;
//      RawWrite(pBBEI, uc, 2);
//      _delay(1000);
//      obdWriteCommand(pBBEI, 0xb9);
//      uc[0] = 0x40;
//      uc[1] = 0x23;
//      RawWrite(pBBEI, uc, 2);
      return;
  } /* ST7302 */
#endif // !WIMPY_MCU
  if (iType == LCD_UC1701 || iType == LCD_HX1230 || iType == LCD_32x64)
  {
      uint8_t cCOM = 0xc0;
      LCDPowerUp(pBBEI);
      if (iType == LCD_HX1230)
      {
          obdSetContrast(pBBEI, 0); // contrast of 0 looks good
          cCOM = 0xc8;
      }
      if (bFlip) // flip horizontal + vertical
      {
         obdWriteCommand(pBBEI, 0xa1); // set SEG direction (A1 to flip horizontal)
         obdWriteCommand(pBBEI, cCOM); // set COM direction (C0 to flip vert)
      }
      if (bInvert)
      {
         obdWriteCommand(pBBEI, 0xa7); // set inverted pixel mode
      }
  }
  if (iType == LCD_UC1609)
  {
      LCDPowerUp(pBBEI);
      return;
//      obdWriteCommand(pBBEI, 0xe2); // system reset
//      _delay(100);
//      obdWriteCommand(pBBEI, 0xa0); // set frame rate to 76fps
//      obdWriteCommand(pBBEI, 0xeb); // set BR
//      obdWriteCommand(pBBEI, 0x2f); // set Power Control
//      obdWriteCommand(pBBEI, 0xc4); // set LCD mapping control
//      obdWriteCommand(pBBEI, 0x81); // set PM
//      obdWriteCommand(pBBEI, 0x90); // set contrast to 144
//      _delay(100);
//      obdWriteCommand(pBBEI, 0xaf); // display enable
//      if (bFlip) // flip horizontal + vertical
//      {
//         obdWriteCommand(pBBEI, 0xa1); // set SEG direction (A1 to flip horizontal)
//         obdWriteCommand(pBBEI, 0xc2); // set COM direction (C0 to flip vert)
//      }
//      if (bInvert)
//      {
//         obdWriteCommand(pBBEI, 0xa7); // set inverted pixel mode
//      }
  } // UC1609
#endif // MEMORY_ONLY
} /* obdSPIInit() */
void obdSetRotation(OBDISP *pBBEI, int iRotation)
{
    pBBEI->iScreenOffset = 0;
    
    switch (iRotation) {
        default: return;
        case 0:
            obdSetFlip(pBBEI, 0);
            pBBEI->iOrientation = 0;
            pBBEI->width = pBBEI->native_width;
            pBBEI->height = pBBEI->native_height;
            break;
        case 1:
        case 90:
            obdSetFlip(pBBEI, 0);
            pBBEI->iOrientation = 90;
            pBBEI->width = pBBEI->native_height;
            pBBEI->height = pBBEI->native_width;
            break;
        case 2:
        case 180:
            pBBEI->iOrientation = 180;
            obdSetFlip(pBBEI, 1);
            pBBEI->width = pBBEI->native_width;
            pBBEI->height = pBBEI->native_height;
            break;
        case 3:
        case 270:
            pBBEI->iOrientation = 270;
            obdSetFlip(pBBEI, 1);
            pBBEI->width = pBBEI->native_height;
            pBBEI->height = pBBEI->native_width;
            break;
  }
} /* obdSetRotation() */
//
// Set the memory configuration to display the pixels at 0 or 180 degrees (flipped)
// Pass true (1) to flip 180, false (0) to set to 0
//
void obdSetFlip(OBDISP *pBBEI, int iOnOff)
{
   if (pBBEI == NULL) return;
   if (!pBBEI->can_flip) return; // don't try it on some displays
    
   pBBEI->flip = iOnOff;
   if (pBBEI->type == LCD_UC1701 || pBBEI->type == LCD_UC1609 || pBBEI->type == LCD_32x64)
   {
      if (iOnOff) // rotate display 180
      {
         obdWriteCommand(pBBEI, 0xa1); // SEG direction (A1 to flip horizontal)
         obdWriteCommand(pBBEI, 0xc0); // COM direction (C0 to flip vert)
      } else { // non-rotated
         obdWriteCommand(pBBEI, 0xa0);
         obdWriteCommand(pBBEI, 0xc8);
      }
   } else { // OLEDs
      if (iOnOff)
      {
          obdWriteCommand(pBBEI, 0xa0);
          obdWriteCommand(pBBEI, 0xc0);
      } else {
          obdWriteCommand(pBBEI, 0xa1);
          obdWriteCommand(pBBEI, 0xc8);
      }
   }
} /* obdSetFlip() */
//
// Initializes the OLED controller into "page mode"
//
int obdI2CInit(OBDISP *pBBEI, int iType, int iAddr, int bFlip, int bInvert, int bWire, int sda, int scl, int reset, int32_t iSpeed)
{
int rc = OLED_NOT_FOUND;
#ifndef MEMORY_ONLY
unsigned char uc[32];
uint8_t u8Len, *s;

  if (pBBEI == NULL)
      return -1;
    pBBEI->can_flip = 1; // assume can flip 180
    if (iType < 0 || iType >= LCD_COUNT)
        return -1; // invalid display type
  if (sda == scl) { // invalid situation, set to -1
    sda = scl = -1;
  }
  pBBEI->type = iType;
  pBBEI->flip = bFlip;
  pBBEI->invert = bInvert;
  pBBEI->wrap = 0; // default - disable text wrap
  pBBEI->bbi2c.iSDA = sda;
  pBBEI->bbi2c.iSCL = scl;
  pBBEI->bbi2c.bWire = bWire;
  pBBEI->com_mode = COM_I2C; // communication mode
#ifdef _LINUX_
  pBBEI->bbi2c.iBus = sda;
  pBBEI->ucScreen = NULL;
#endif
  I2CInit(&pBBEI->bbi2c, iSpeed); // on Linux, SDA = bus number, SCL = device address
#ifdef _LINUX_
  pBBEI->oled_addr = (uint8_t)scl;
#else
  // Reset it
  if (reset != 0xff && reset != -1)
  {
    pinMode(reset, OUTPUT);
    digitalWrite(reset, HIGH);
    _delay(50);
    digitalWrite(reset, LOW);
    _delay(50);
    digitalWrite(reset, HIGH);
    _delay(10);
  }
  // find the device address if requested
  if (iAddr == -1 || iAddr == 0 || iAddr == 0xff) // find it
  {
    I2CTest(&pBBEI->bbi2c, 0x3c);
    if (I2CTest(&pBBEI->bbi2c, 0x3c))
       pBBEI->oled_addr = 0x3c;
    else if (I2CTest(&pBBEI->bbi2c, 0x3d))
       pBBEI->oled_addr = 0x3d;
    else
       return rc; // no display found!
  }
  else
  {
    pBBEI->oled_addr = iAddr;
    I2CTest(&pBBEI->bbi2c, iAddr);
    if (!I2CTest(&pBBEI->bbi2c, iAddr))
       return rc; // no display found
  }

#endif
  // Detect the display controller (SSD1306, SH1107 or SH1106)
  uint8_t u = 0;
  I2CReadRegister(&pBBEI->bbi2c, pBBEI->oled_addr, 0x00, &u, 1); // read the status register
  u &= 0x0f; // mask off power on/off bit
  if ((u == 0x7 || u == 0xf) && pBBEI->type == OLED_128x128) // SH1107
  { // A single SSD1306 display returned 7, so only act on it if the
    // user specified that they're working with a 128x128 display
    rc = OLED_SH1107_3C;
    bFlip = !bFlip; // SH1107 seems to have this reversed from the usual direction
  }
  else if (u == 0x8) // SH1106
  {
    rc = OLED_SH1106_3C;
    pBBEI->type = OLED_132x64; // needs to be treated a little differently
  }
  else if (u == 3 || u == 6 || u == 7) // 7=128x64(rare),6=128x64 display, 3=smaller
  {
    rc = OLED_SSD1306_3C;
  }
    if (pBBEI->oled_addr == 0x3d) {
        rc++; // return the '3D' version of the type
    }
  if (iType == OLED_80x128)
  {
      s = (uint8_t *)oled80_initbuf;
      u8Len = sizeof(oled80_initbuf);
  }
  if (iType == OLED_128x32 || iType == OLED_96x16)
  {
      s = (uint8_t *)oled32_initbuf;
      u8Len = sizeof(oled32_initbuf);
  }
  else if (iType == OLED_128x128)
  {
      s = (uint8_t *)oled128_initbuf;
      u8Len = sizeof(oled128_initbuf);
  }
  else if (iType == OLED_72x40)
  {
      s = (uint8_t *)oled72_initbuf;
      u8Len = sizeof(oled72_initbuf);
  }
  else if (iType == OLED_64x128)
  {
      s = (uint8_t *)oled64x128_initbuf;
      u8Len = sizeof(oled64x128_initbuf);
      pBBEI->can_flip = 0; // this display can't flip 180 (don't know why)
  }
  else if (iType == LCD_128x64_I2C)
  {
      obdWriteCommand(pBBEI, 0xe2); // Need to be reset after power-on
      _delay(10);
      s = (uint8_t *)lcd128x64_initbuf;
      u8Len = sizeof(lcd128x64_initbuf);
      rc = LCD_OK;
      memcpy_P(uc, s, u8Len);
  }
  else // 132x64, 128x64, 64x48 and 64x32
  {
      s = (uint8_t *)oled64_initbuf;
      u8Len = sizeof(oled64_initbuf);
  }

    memcpy_P(uc, s, u8Len);
  RawWrite(pBBEI, uc, u8Len);
  if (bInvert)
  {
    uc[0] = 0; // command
    uc[1] = 0xa7; // invert command
    RawWrite(pBBEI,uc, 2);
  }
  if (bFlip) // rotate display 180
  {
    uc[0] = 0; // command
    uc[1] = 0xa0;
    RawWrite(pBBEI,uc, 2);
    uc[1] = 0xc0;
    RawWrite(pBBEI,uc, 2);
  }
  pBBEI->native_width = pBBEI->width = 128; // assume 128x64
  pBBEI->native_height = pBBEI->height = 64;
  if (iType == OLED_96x16)
  {
      pBBEI->native_width = pBBEI->width = 96;
      pBBEI->native_height = pBBEI->height = 16;
  }
  else if (iType == OLED_80x128)
  {
      pBBEI->native_width = pBBEI->width = 80;
      pBBEI->native_height = pBBEI->height = 128;
  }
  else if (iType == OLED_64x128)
  {
      pBBEI->native_width = pBBEI->width = 64;
      pBBEI->native_height = pBBEI->height = 128;
  }
  else if (iType == OLED_128x32)
      pBBEI->native_height = pBBEI->height = 32;
  else if (iType == OLED_128x128)
      pBBEI->native_height = pBBEI->height = 128;
  else if (iType == OLED_64x32)
  {
      pBBEI->native_width = pBBEI->width = 64;
      pBBEI->native_height = pBBEI->height = 32;
  }
  else if (iType == OLED_64x48)
  {
      pBBEI->native_width = pBBEI->width = 64;
      pBBEI->native_height = pBBEI->height = 48;
  }
  else if (iType == OLED_72x40)
  {
      pBBEI->native_width = pBBEI->width = 72;
      pBBEI->native_height = pBBEI->height = 40;
  }
#endif // MEMORY_ONLY
  return rc;
} /* obdI2CInit() */
//
// Sends a command to turn on or off the OLED display
//
void oledPower(OBDISP *pBBEI, uint8_t bOn)
{
    if (bOn)
      obdWriteCommand(pBBEI, 0xaf); // turn on OLED
    else
      obdWriteCommand(pBBEI, 0xae); // turn off OLED
} /* oledPower() */

// Sets the D/C pin to data or command mode
void obdSetDCMode(OBDISP *pBBEI, int iMode)
{
  if (pBBEI->iDCPin == 0xff) // 9-bit SPI
      pBBEI->mode = (uint8_t)iMode;
  else // set the GPIO line
      digitalWrite(pBBEI->iDCPin, (iMode == MODE_DATA));
} /* obdSetDCMode() */

static void obdWriteCommand2(OBDISP *pBBEI, unsigned char c, unsigned char d)
{
unsigned char buf[4];

    if (pBBEI->com_mode == COM_I2C) {// I2C device
        buf[0] = 0x00;
        buf[1] = c;
        buf[2] = d;
        RawWrite(pBBEI, buf, 3);
    } else { // must be SPI
        obdWriteCommand(pBBEI, c);
        obdWriteCommand(pBBEI, d);
    }
} /* obdWriteCommand2() */

//
// Sets the brightness (0=off, 255=brightest)
//
void obdSetContrast(OBDISP *pBBEI, unsigned char ucContrast)
{
  if (pBBEI->type == LCD_HX1230)
  { // valid values are 0-31, so scale it
      ucContrast >>= 3;
      obdWriteCommand(pBBEI, 0x80 + ucContrast);
  }
  else if (pBBEI->type == LCD_NOKIA5110)
  {
      // we allow values of 0xb0-0xbf, so shrink the range
      ucContrast >>= 4;
      obdWriteCommand(pBBEI, 0x21); // set advanced command mode
      obdWriteCommand(pBBEI, 0xb0 | ucContrast);
      obdWriteCommand(pBBEI, 0x20); // set simple command mode
  }
  else // OLEDs + UC1701
      obdWriteCommand2(pBBEI, 0x81, ucContrast);
} /* obdSetContrast() */
static void EPDWaitBusy(OBDISP *pBBEI, int bQuick)
{
#ifndef MEMORY_ONLY
int iTimeout = 0;
int iEndTime = (int)pBBEI->iTimeout;
    long l = micros();

    if (pBBEI->iLEDPin == 0xff || pBBEI->iLEDPin == -1) {
        if (bQuick)
            iEndTime /= 5; // 1/5th for short waits such as after Power-on
        _delay(iEndTime); // no busy pin, wait fixed amount
        return;
    }
    _delay(1);
    while (1) {
        if (digitalRead(pBBEI->iLEDPin) == pBBEI->busy_idle) break;
    }
//  while (1) {
//     if (digitalRead(pBBEI->iLEDPin) == pBBEI->busy_idle)
//         break;
//     iTimeout += (iEndTime/4);
//     if (iTimeout > iEndTime)
//         break; // DEBUG - timeout
//     _delay(iEndTime/4);
//  }
    l = micros() - l;
    Serial.print((int)l, DEC);
    Serial.println(" us spent in EPDWaitBusy");
    
    if ((l/1000) > iEndTime) {
#if defined( ARDUINO ) && !defined(WIMPY_MCU)
        Serial.println("EPDWaitBusy() timed out");
#endif
#if defined(_LINUX_)
        printf("EPDWaitBusy() timed out\n");
#endif
    }
#endif // MEMORY_ONLY
} /* EPDWaitBusy() */

void EPD27_SetLut(OBDISP *pBBEI, int bFast)
{
    if (bFast) {
        obdWriteCommand(pBBEI, UC8151_LUT_VCOM); //vcom
        RawWriteData(pBBEI, (uint8_t *)lut_fast_vcom_dc, 44);
       
        obdWriteCommand(pBBEI, UC8151_LUT_WW);
        RawWriteData(pBBEI, (uint8_t *)lut_fast_ww, 42);
       
        obdWriteCommand(pBBEI, UC8151_LUT_BW);
        RawWriteData(pBBEI, (uint8_t *)lut_fast_bw, 42);

        obdWriteCommand(pBBEI, UC8151_LUT_WB);
        RawWriteData(pBBEI, (uint8_t *)lut_fast_bb, 42);

        obdWriteCommand(pBBEI, UC8151_LUT_BB);
        RawWriteData(pBBEI, (uint8_t *)lut_fast_wb, 42);
    } else {
        obdWriteCommand(pBBEI, UC8151_LUT_VCOM); //vcom
        RawWriteData(pBBEI, (uint8_t *)lut_vcom_dc, 44);
       
        obdWriteCommand(pBBEI, UC8151_LUT_WW);
        RawWriteData(pBBEI, (uint8_t *)lut_ww, 42);
       
        obdWriteCommand(pBBEI, UC8151_LUT_BW);
        RawWriteData(pBBEI, (uint8_t *)lut_bw, 42);

        obdWriteCommand(pBBEI, UC8151_LUT_WB);
        RawWriteData(pBBEI, (uint8_t *)lut_bb, 42);

        obdWriteCommand(pBBEI, UC8151_LUT_BB);
        RawWriteData(pBBEI, (uint8_t *)lut_wb, 42);
    }
}

//
// Wake up the EPD controller
//
static void EPDWakeUp(OBDISP *pBBEI, int bResetOnly)
{
    if (pBBEI->iRSTPin != 0xff) {
        for (int i=0; i<1; i++) {
          digitalWrite(pBBEI->iRSTPin, LOW);
          _delay(10);
          digitalWrite(pBBEI->iRSTPin, HIGH);
          _delay(10);
        }
    }
    if (bResetOnly) return;
    
    if (pBBEI->type == EPD164_BWYR_168x168 || pBBEI->type == EPD236_BWYR_168x296) return; // these don't like this cmd at this time
    if (pBBEI->chip_type == BBEI_CHIP_UC8151) {
        obdWriteCommand(pBBEI, UC8151_PON);
    } else {
        obdWriteCommand(pBBEI, SSD1608_SW_RESET); // soft reset
    }
    EPDWaitBusy(pBBEI, 1);
} /* EPDWakeUp() */

static void EPDSleep(OBDISP *pBBEI, int bDeep)
{

    if (pBBEI->chip_type == BBEI_CHIP_SSD16xx) {
        if (bDeep)
            EPD_CMD2(pBBEI, 0x10, 0x02); // deep sleep mode 2 (loses RAM, needs reset)
        else // no reset pin, put it into light sleep
            EPD_CMD2(pBBEI, 0x10, 0x00);
        return;
    } else { // UC81xx
        EPD_CMD2(pBBEI, UC8151_CDI, 0x17); // border floating
        obdWriteCommand(pBBEI, UC8151_POFF); // power off
        EPDWaitBusy(pBBEI, 1);
        if (bDeep) {
          EPD_CMD2(pBBEI, UC8151_DSLP, 0xa5); // deep sleep
        }
    }
} /* EPDSleep() */
//
// Set the boundaries of the partial update area
// start/end x/y
//
void EPDSetPartialArea(OBDISP *pBBEI, int sx, int sy, int ex, int ey)
{
uint8_t ucTemp[12];

  ucTemp[0] = 0x40; // data operation
  sx &= 0xFFF8; // byte boundary
  ex = (ex - 1) | 0x7; // byte boundary - 1
  obdWriteCommand(pBBEI, 0x90); // partial window
  ucTemp[1] = (uint8_t)(sx / 256);
  ucTemp[2] = (uint8_t)(sx & 256);
  ucTemp[3] = (uint8_t)(ex / 256);
  ucTemp[4] = (uint8_t)(ex & 256);
  ucTemp[5] = (uint8_t)(sy / 256);
  ucTemp[6] = (uint8_t)(sy & 256);
  ucTemp[7] = (uint8_t)(ey / 256);
  ucTemp[8] = (uint8_t)(ey & 256);
  ucTemp[9] = 0x01; // gates scan inside and outside the partial area (don't see any difference)
  RawWrite(pBBEI, ucTemp, 10);
  _delay(2);
  //IO.writeDataTransaction(0x00); // don't see any difference
  //return (7 + xe - x) / 8; // number of bytes to transfer
} /* EPDSetPartialArea() */

void EPD154_Finish(OBDISP *pBBEI, int bPartial)
{
    if (bPartial) {
        EPD_CMD2(pBBEI, SSD1608_DISP_CTRL2, 0xcf);
    } else {
        EPD_CMD2(pBBEI, SSD1608_DISP_CTRL2, 0xc7);
    }
    obdWriteCommand(pBBEI, SSD1608_MASTER_ACTIVATE);
    EPDWaitBusy(pBBEI, 0);
} /* EPD154_Finish() */

void EPD213_Begin(OBDISP *pBBEI, int x, int y, int w, int h, int bPartial)
{
uint8_t ucLine[8];

    // set luts
    obdWriteCommand(pBBEI, SSD1608_WRITE_LUT);
//    if (pBBEI->type == EPD213_104x212) {
//       RawWriteData(pBBEI, (uint8_t *)epd213_lut, sizeof(epd213_lut));
//    } else { // 122x250
    if (bPartial) {
//        obdWriteCommand(pBBEI, SSD1608_WRITE_LUT);
        memcpy_P(u8Cache, epd213_lut2_partial, sizeof(epd213_lut2_partial));
        RawWriteData(pBBEI, u8Cache, sizeof(epd213_lut2_partial));
    }  else {
        memcpy_P(u8Cache, epd213_lut2_full, sizeof(epd213_lut2_full));
            RawWriteData(pBBEI, u8Cache, sizeof(epd213_lut2_full));
    }

    obdWriteCommand(pBBEI, 0x44); // set RAM X start/end
    if (pBBEI->iOrientation == 0 || pBBEI->iOrientation == 180) {
        ucLine[0] = x/8;
        ucLine[1] = (x+w-1)/8;
    } else { // 90/270
        ucLine[0] = y/8;
        ucLine[1] = (y+h-1)/8;
    }
    RawWriteData(pBBEI, ucLine, 2);
    obdWriteCommand(pBBEI, 0x45); // set RAM Y start/end
    if (pBBEI->iOrientation == 0 || pBBEI->iOrientation == 180) {
        ucLine[0] = (uint8_t)y;
        ucLine[1] = (uint8_t)(y>>8);
        ucLine[2] = (uint8_t)(y+h-1);
        ucLine[3] = (uint8_t)((y+h-1)>>8);
    } else { // 90/270
        ucLine[0] = (uint8_t)x;
        ucLine[1] = (uint8_t)(x>>8);
        ucLine[2] = (uint8_t)(x+w-1);
        ucLine[3] = (uint8_t)((x+w-1)>>8);
    }
    RawWriteData(pBBEI, ucLine, 4);
    EPD_CMD2(pBBEI, 0x4e, 0x00); // set RAM X pointer start
    ucLine[0] = 0x00;
    ucLine[1] = 0x00;
    obdWriteCommand(pBBEI, 0x4f); // set RAM Y pointer start
    RawWriteData(pBBEI, ucLine, 2);
    if (bPartial) {
        EPD_CMD2(pBBEI, SSD1608_WRITE_VCOM, 0x26); // VCOM
        EPDWaitBusy(pBBEI, 1);
        obdWriteCommand(pBBEI, 0x37); // ?
        memset(ucLine, 0, 7);
        ucLine[4] = 0x40;
        RawWriteData(pBBEI, ucLine, 7);

        EPD_CMD2(pBBEI, 0x22, 0xc0);
        obdWriteCommand(pBBEI, 0x20);
        EPDWaitBusy(pBBEI, 1);
        EPD_CMD2(pBBEI, 0x3c, 0x01); // border waveform
    } else {
        EPD_CMD2(pBBEI, SSD1608_WRITE_VCOM, 0x55); // VCOM
        EPDWaitBusy(pBBEI, 1);
    }
} /* EPD213_Begin() */

//
// Special case for some e-ink displays
//
static int EPDDumpPartial(OBDISP *pBBEI, uint8_t *pBuffer, int x, int y, int w, int h)
{
    long lTime;
    
    if (pBuffer == NULL && pBBEI->ucScreen == NULL)
        return BBEI_ERROR_NO_MEMORY;
    
    EPDWakeUp(pBBEI, 1); // tickle the reset line
    
    if (pBBEI->pInitPart) { // command tables ready to go
        lTime = millis();
        EPDSendCMDSequence(pBBEI, pBBEI->pInitPart);
        if (pBBEI->chip_type == BBEI_CHIP_UC8151) {
            int iSize = pBBEI->width * ((pBBEI->height+7)>>3);
            EPDWriteImage(pBBEI, 0x10, &pBBEI->ucScreen[iSize], 0,0,pBBEI->width, pBBEI->height, pBBEI->iFlags & BBEI_FAST_INVERTED); // write old data inverted
            EPDWriteImage(pBBEI, 0x13, NULL, 0, 0, pBBEI->width, pBBEI->height, pBBEI->iFlags & BBEI_FAST_INVERTED);
            memcpy(&pBBEI->ucScreen[iSize], pBBEI->ucScreen, iSize); // new becomes old
            lTime = millis() - lTime;
            pBBEI->iDataTime = (int)lTime;
            lTime = millis();
            obdWriteCommand(pBBEI, UC8151_DRF);
            _delay(1); // wait before checking busy line (necessary!)
            EPDWaitBusy(pBBEI, 1);
            obdWriteCommand(pBBEI, UC8151_POFF);
            lTime = millis() - lTime;
            pBBEI->iOpTime = (int)lTime;
            return 0;
        } else { // SSD16xx
            EPDWriteImage(pBBEI, 0x24, NULL, 0, 0, pBBEI->width, pBBEI->height, pBBEI->iFlags & BBEI_FAST_INVERTED);
            EPD_CMD2(pBBEI, 0x22, 0xff);
            lTime = millis() - lTime;
            pBBEI->iDataTime = (int)lTime;
            lTime = millis();
            obdWriteCommand(pBBEI, 0x20); // refresh
            EPDWaitBusy(pBBEI, 0);
            lTime = millis() - lTime;
            pBBEI->iOpTime = (int)lTime;
            return 0;
            //EPD_CMD2(pBBEI, 0x10, 0x1); // deep sleep is needed (very strange behavior if it is not issued here)
        } // SSD16xx
    } // has a command list for fast updates
    if (pBBEI->type == EPD29R_128x296 || pBBEI->type == EPD154R_152x152 || pBBEI->type == EPD42R_400x300) {
        if (pBBEI->iFlags & BBEI_FULLUPDATE) {
            // Kludge alert - fool it into going into B&W mode to allow partial updates. Need to combine the black+red pixels and send them to the ALTRAM first
            uint8_t *s = pBBEI->ucScreen;
            int iOffset = ((pBBEI->native_width+7)/8) * pBBEI->native_height; // full buffer size
            for (int i=0; i<iOffset; i++) {
                s[i] |= s[i+iOffset]; // combine red+black pixels
            }
            // do a fake full update first
            EPD213_Begin(pBBEI, 0, 0, pBBEI->width, pBBEI->height, false);
#ifndef WIMPY_MCU
            EPDWriteImage(pBBEI, SSD1608_WRITE_RAM, NULL, 0, 0, pBBEI->width, pBBEI->height, 0);
            EPDWriteImage(pBBEI, SSD1608_WRITE_ALTRAM, NULL, 0, 0, pBBEI->width, pBBEI->height, 0); // send image to current and previous buffers
#endif
            return BBEI_SUCCESS;
        }
        EPD213_Begin(pBBEI, x, y, w, h, true);
#ifndef WIMPY_MCU
        EPDWriteImage(pBBEI, SSD1608_WRITE_RAM, NULL, x, y, w, h, 0);
#endif
        EPD154_Finish(pBBEI, true);
    }
    if (pBBEI->type == EPD213_104x212 || pBBEI->type == EPD213_122x250) {
        EPD213_Begin(pBBEI, x, y, w, h, true);
#ifndef WIMPY_MCU
        EPDWriteImage(pBBEI, SSD1608_WRITE_RAM, NULL, x, y, w, h, 0);
#endif
        EPD154_Finish(pBBEI, true);
    } else if (pBBEI->type == EPD154_152x152) {
        EPDSendCMDSequence(pBBEI, pBBEI->pInitFast);
#ifndef WIMPY_MCU
        if (pBBEI->ucScreen) {
            EPDWriteImage(pBBEI, UC8151_DTM2, NULL, 0, 0, pBBEI->width, pBBEI->height, 1);
        }
#endif
        obdWriteCommand(pBBEI, UC8151_DRF);
        EPDWaitBusy(pBBEI, 1);
    }
    EPDSleep(pBBEI, 0);
    return BBEI_SUCCESS;
} /* EPDDumpFast() */

void EPD_CMD2(OBDISP *pBBEI, uint8_t cmd, uint8_t param)
{
	obdWriteCommand(pBBEI, cmd);
	RawWriteData(pBBEI, &param, 1);
} /* EPD_CMD2() */

void EPD8951_REG(OBDISP *pBBEI, uint16_t addr, uint16_t data)
{
uint8_t ucTemp[4];
    
    EPDWaitBusy(pBBEI, 1);
    ucTemp[0] = 0x60; // cmd intro 6000h
    ucTemp[1] = 0x00;
    ucTemp[2] = 0x00; // reg write 0011h
    ucTemp[3] = 0x11;
    RawWriteData(pBBEI, ucTemp, 4);
    EPDWaitBusy(pBBEI, 1);
    ucTemp[0] = ucTemp[1] = 0; // 0000 data intro
    ucTemp[2] = (uint8_t)(addr >> 8);
    ucTemp[3] = (uint8_t)(addr & 0xff);
    RawWriteData(pBBEI, ucTemp, 4);
    ucTemp[2] = (uint8_t)(data >> 8);
    ucTemp[3] = (uint8_t)(data & 0xff);
    RawWriteData(pBBEI, ucTemp, 4);

} /* EPD8951_REG() */

void EPD8951_ARGS(OBDISP *pBBEI, uint16_t u16CMD, uint16_t *pArgs, int iCount)
{
    uint8_t ucTemp[4];
    EPDWaitBusy(pBBEI, 1);
    ucTemp[0] = 0x60;
    ucTemp[1] = 0x00; // 6000 cmd
    ucTemp[2] = (uint8_t)(u16CMD >> 8);
    ucTemp[3] = (uint8_t)(u16CMD & 0xff);
    RawWriteData(pBBEI, ucTemp, 4);
    
    for (int i=0; i<iCount; i++) {
        EPDWaitBusy(pBBEI, 1);
        ucTemp[0] = 0x00;
        ucTemp[1] = 0x00; // 0000 data
        ucTemp[2] = (uint8_t)(pArgs[i] >> 8);
        ucTemp[3] = (uint8_t)(pArgs[i] & 0xff);
        RawWriteData(pBBEI, ucTemp, 4);
    }
} /* EPD8951_ARGS() */

void EPD8951_CMD(OBDISP *pBBEI, int iCMD)
{
uint8_t ucTemp[4];
    
    EPDWaitBusy(pBBEI, 1);
    ucTemp[0] = 0x60;
    ucTemp[1] = 0x00; // cmd prefix = 6000h
    ucTemp[2] = (uint8_t)(iCMD >> 8);
    ucTemp[3] = (uint8_t)(iCMD & 0xff);
    RawWriteData(pBBEI, ucTemp, 4);
} /* EPD8951_CMD() */

void EPD8951_WORD(OBDISP *pBBEI, uint16_t u16Word)
{
uint8_t ucTemp[4];
    
    EPDWaitBusy(pBBEI, 1);
    ucTemp[0] = 0x00;
    ucTemp[1] = 0x00; // cmd prefix = 6000h
    ucTemp[2] = (uint8_t)(u16Word >> 8);
    ucTemp[3] = (uint8_t)(u16Word & 0xff);
    RawWriteData(pBBEI, ucTemp, 4);
} /* EPD8951_WORD() */

void EPDFill(OBDISP *pBBEI, uint8_t ucCMD, uint8_t ucPattern)
{
int iSize;

    if (ucCMD) {
        obdWriteCommand(pBBEI, ucCMD); // start write
    }
    if (pBBEI->type == EPD579_792x272) {
        iSize = 400*(272>>3);
#ifndef __AVR__
    } else if (pBBEI->type == EPD74R_640x384) {
        iSize = (pBBEI->native_width * pBBEI->native_height)/2; // 4bpp
#endif // __AVR__
    } else {
        iSize = ((pBBEI->native_width+7)>>3) * pBBEI->native_height;
    }
    for (int i=0; i<iSize; i++) {
        RawWriteData(pBBEI, &ucPattern, 1); // slow to go one byte at a time, but not a critical time issue for this feature and we need to support a "zero RAM" environment
    }
} /* EPDFill() */

//
// Write EPD Image data
//
#ifndef WIMPY_MCU
static void EPDWriteImage(OBDISP *pBBEI, uint8_t ucCMD, uint8_t *pBits, int x, int y, int w, int h, int bInvert)
{
int tx, ty;
uint8_t *s, *d, ucSrcMask, ucDstMask, uc;
uint8_t *pBuffer, ucInvert = 0;
int iPitch;
    
    if (pBits == NULL) { // use back buffer
        pBuffer = pBBEI->ucScreen;
        iPitch = pBBEI->width;
    } else {
        // assume the passed buffer is only the size and width of the requested write
        pBuffer = pBits;
        iPitch = w;
    }
    if (bInvert) {// use second bit plane
        //pBuffer += pBBEI->width * ((pBBEI->height+7)/8);
        ucInvert = 0xff; // red logic is inverted
    }
    if (ucCMD) {
        obdWriteCommand(pBBEI, ucCMD); // start write
    }
  // Convert the bit direction and write the data to the EPD
  u8Cache[0] = u8Cache[1] = 0; // prefix for IT8951
  if (pBBEI->iOrientation == 180) {
      for (ty=y+h-1; ty>=y; ty--) {
     ucSrcMask = 1 << (ty & 7);
     ucDstMask = 0x80;
     uc = 0xff;
     d = u8Cache+2;
     s = &pBuffer[(ty>>3) * iPitch];
     for (tx=x+w-1; tx>=x; tx--) {
            if (s[tx] & ucSrcMask)
           uc &= ~ucDstMask;
        ucDstMask >>= 1;
        if (ucDstMask == 0) {
           *d++ = (uc ^ ucInvert);
           uc = 0xff;
           ucDstMask = 0x80;
        }
     } // for tx
     *d++ = uc; // store final partial byte
     if (pBBEI->type == EPD47_540x960)
         RawWriteData(pBBEI, u8Cache, 2 + ((w+7)/8));
     else
         RawWriteData(pBBEI, u8Cache+2, (w+7)/8);
      } // for ty
  } else if (pBBEI->iOrientation == 0) {
      for (ty=y; ty<y+h; ty++) {
         ucSrcMask = 1 << (ty & 7);
         ucDstMask = 0x80;
         uc = 0xff;
         d = u8Cache+2;
         s = &pBuffer[(ty >> 3) * iPitch];
         for (tx=x; tx<x+w; tx++) {
            if (s[tx] & ucSrcMask) // src pixel set
               uc &= ~ucDstMask;
            ucDstMask >>= 1;
            if (ucDstMask == 0) { // completed byte
               *d++ = (uc ^ ucInvert);
               uc = 0xff;
               ucDstMask = 0x80;
            }
         } // for tx
        *d++ = (uc ^ ucInvert); // store final partial byte
        if (pBBEI->type == EPD47_540x960)
            RawWriteData(pBBEI, u8Cache, 2+((w+7)/8));
        else
            RawWriteData(pBBEI, u8Cache+2, (w+7)/8);
      } // for ty
  } else if (pBBEI->iOrientation == 90) {
      for (tx=x; tx<x+w; tx++) {
         d = u8Cache+2;
     // need to pick up and reassemble every pixel
     ucDstMask = 0x80;
     uc = 0xff;
     ucSrcMask = 1 << ((y+h-1) & 7);
     for (ty=y+h-1; ty>=y; ty--) {
            s = &pBuffer[tx + (ty>>3) * iPitch];
        if (s[0] & ucSrcMask) uc &= ~ucDstMask;
        ucDstMask >>= 1;
        ucSrcMask >>= 1;
            if (ucDstMask == 0) {
               *d++ = (uc ^ ucInvert);
           ucDstMask = 0x80;
           uc = 0xff;
        }
        if (ucSrcMask == 0)
           ucSrcMask = 0x80; // bottom up
     } // for ty
      *d++ = (uc ^ ucInvert); // store final partial byte
          if (pBBEI->type == EPD47_540x960)
              RawWriteData(pBBEI, u8Cache, 2+((h+7)/8));
         else
             RawWriteData(pBBEI, u8Cache+2, (h+7)/8);
      } // for tx
  } else if (pBBEI->iOrientation == 270) {
      for (tx=x+w-1; tx>=x; tx--) {
      d = u8Cache+2;
      // reassemble every pixel
      ucDstMask = 0x80;
      uc = 0xff;
      ucSrcMask = 1 << (y & 7);
      for (ty=y; ty<y+h; ty++) {
          s = &pBuffer[tx + (ty>>3) * iPitch];
          if (s[0] & ucSrcMask) uc &= ~ucDstMask;
          ucDstMask >>= 1;
          ucSrcMask <<= 1;
          if (ucDstMask == 0) {
         *d++ = (uc ^ ucInvert);
         ucDstMask = 0x80;
         uc = 0xff;
          }
          if (ucSrcMask == 0)
         ucSrcMask = 1;
      } // for ty
      *d++ = (uc ^ ucInvert); // store final partial byte
      if (pBBEI->type == EPD47_540x960)
          RawWriteData(pBBEI, u8Cache, 2+((h+7)/8));
      else
          RawWriteData(pBBEI, u8Cache+2, (h+7)/8);
      } // for x
  } // 270
} /* EPDWriteImage() */
//
// Write EPD Image data to a 4-color (black/white/yellow/red) e-paper
//
static void EPDWriteImage2bpp(OBDISP *pBBEI, uint8_t ucCMD, int x, int y, int w, int h)
{
uint8_t ucLine[324]; // send the data one line at a time
int tx, ty, iRedOff;
uint8_t *s, *d, ucSrcMask, uc;
uint8_t *pBuffer;

    pBuffer = pBBEI->ucScreen;
    iRedOff = pBBEI->width * ((pBBEI->height+7)/8);
    if (ucCMD) {
        obdWriteCommand(pBBEI, ucCMD); // start write
    }
  // Convert the bit direction and write the data to the EPD
  if (pBBEI->iOrientation == 180) {
      for (ty=y+h-1; ty>=y; ty--) {
     ucSrcMask = 1 << (ty & 7);
     d = ucLine;
     s = &pBuffer[(ty>>3) * pBBEI->width];
     for (tx=x+w-4; tx>=x; tx-=4) {
         uc = 0;
         for (int pix=3; pix>=0; pix--) {
             uc <<= 2; // shift down 1 pixel
             if (!(s[tx+pix] & ucSrcMask)) // inverted plane 0
                uc |= 0x01;
             if (s[tx+pix+iRedOff] & ucSrcMask) // plane 1
                uc |= 0x02;
         }
         *d++ = uc; // store 4 pixels
     } // for tx
      RawWriteData(pBBEI, ucLine, w/4);
      } // for ty
  } else if (pBBEI->iOrientation == 0) {
      for (ty=y; ty<y+h; ty++) {
         ucSrcMask = 1 << (ty & 7);
         d = ucLine;
         s = &pBuffer[(ty >> 3) * pBBEI->width];
         for (tx=x; tx<x+w; tx+=4) {
             uc = 0; // destination pixels
             for (int pix=0; pix<4; pix++) {
                uc <<= 2; // shift down 1 pixel
                if (!(s[tx+pix] & ucSrcMask)) // inverted plane 0
                   uc |= 0x01;
                if (s[tx+pix+iRedOff] & ucSrcMask) // plane 1
                   uc |= 0x02;
             } // for each pixel in the byte
             *d++ = uc; // store 4 pixels
         } // for tx
        RawWriteData(pBBEI, ucLine, w/4);
      } // for ty
  } else if (pBBEI->iOrientation == 90) {
      for (tx=x; tx<x+w; tx++) {
         d = ucLine;
     ucSrcMask = 1 << ((y+h-1) & 7);
     for (ty=y+h-4; ty>=y; ty-=4) {
         s = &pBuffer[tx + (ty>>3) * pBBEI->width];
         uc = 0;
         for (int pix=3; pix>=0; pix--) {
            uc <<= 2; // shift down 1 pixel
            if (!(s[tx] & ucSrcMask)) // inverted plane 0
               uc |= 0x01;
            if (s[tx+iRedOff] & ucSrcMask) // plane 1
               uc |= 0x02;
            ucSrcMask >>= 1;
            if (ucSrcMask == 0) ucSrcMask = 0x80;
         }
         *d++ = uc; // store 4 pixels
     } // for ty
        RawWriteData(pBBEI, ucLine, h/4);
      } // for tx
  } else if (pBBEI->iOrientation == 270) {
      for (tx=x+w-1; tx>=x; tx--) {
      d = ucLine;
      ucSrcMask = 1 << (y & 7);
      for (ty=y; ty<y+h; ty+=4) {
          s = &pBuffer[tx + (ty>>3) * pBBEI->width];
          uc = 0;
          for (int pix=0; pix<4; pix++) {
             uc <<= 2;
             if (!(s[tx] & ucSrcMask)) // inverted plane 0
                uc |= 0x01;
             if (s[tx+iRedOff] & ucSrcMask) // plane 1
                uc |= 0x02;
             ucSrcMask <<= 1;
             if (ucSrcMask == 0) ucSrcMask = 1;
          } // for pix
           *d++ = uc; // store 2 pixels
        } // for ty
        RawWriteData(pBBEI, ucLine, h/4);
     } // for x
  } // 270
} /* EPDWriteImage2bpp() */
//
// Write EPD Image data
//
static void EPDWriteImage4bpp(OBDISP *pBBEI, uint8_t ucCMD, int x, int y, int w, int h)
{
uint8_t ucLine[324]; // send the data one line at a time
int tx, ty, iRedOff;
uint8_t *s, *d, ucSrcMask, uc;
uint8_t *pBuffer;
    
    pBuffer = pBBEI->ucScreen;
    iRedOff = pBBEI->width * ((pBBEI->height+7)/8);
    if (ucCMD) {
        obdWriteCommand(pBBEI, ucCMD); // start write
    }
  // Convert the bit direction and write the data to the EPD
    // This particular controller has 4 bits per pixel where 0=black, 3=white, 4=red
    // this wastes 50% of the time transmitting bloated info (only need 2 bits)
  if (pBBEI->iOrientation == 180) {
      for (ty=y+h-1; ty>=y; ty--) {
     ucSrcMask = 1 << (ty & 7);
     d = ucLine;
     s = &pBuffer[(ty>>3) * pBBEI->width];
     for (tx=x+w-1; tx>=x; tx-=2) {
         uc = 0x33; // start with white
            if (s[tx] & ucSrcMask) // black
                uc = 0x03;
         if (s[tx+iRedOff] & ucSrcMask) // red
             uc = 0x43;
         if (s[tx-1] & ucSrcMask) {// src pixel = black
             uc &= 0xf0;
         }
         if (s[tx-1+iRedOff] & ucSrcMask) { // red
              uc &= 0xf0; uc |= 0x4;
         }
           *d++ = uc; // store 2 pixels
     } // for tx
      RawWriteData(pBBEI, ucLine, w/2);
      } // for ty
  } else if (pBBEI->iOrientation == 0) {
      for (ty=y; ty<y+h; ty++) {
         ucSrcMask = 1 << (ty & 7);
         d = ucLine;
         s = &pBuffer[(ty >> 3) * pBBEI->width];
         for (tx=x; tx<x+w; tx+=2) {
             uc = 0x33; // start with white
             if (s[tx] & ucSrcMask) {// src pixel = black
                uc = 0x03;
             }
             if (s[tx+iRedOff] & ucSrcMask) { // red
                 uc = 0x43;
             }
             if (s[tx+1] & ucSrcMask) {// src pixel = black
                 uc &= 0xf0;
             }
             if (s[tx+1+iRedOff] & ucSrcMask) { // red
                  uc &= 0xf0; uc |= 0x4;
             }
               *d++ = uc; // store 2 pixels
         } // for tx
        RawWriteData(pBBEI, ucLine, w/2);
      } // for ty
  } else if (pBBEI->iOrientation == 90) {
      for (tx=x; tx<x+w; tx++) {
         d = ucLine;
     ucSrcMask = 1 << ((y+h-1) & 7);
     for (ty=y+h-1; ty>=y; ty-=2) {
            s = &pBuffer[tx + (ty>>3) * pBBEI->width];
         uc = 0x33;
        if (s[0] & ucSrcMask)
            uc = 0x03; // black
         if (s[iRedOff] & ucSrcMask)
             uc = 0x43; // red
         ucSrcMask >>= 1;
         if (s[0] & ucSrcMask) {// src pixel = black
             uc &= 0xf0;
         }
         if (s[iRedOff] & ucSrcMask) { // red
              uc &= 0xf0; uc |= 0x4;
         }
         ucSrcMask >>= 1;
         *d++ = uc; // store 2 pixels
        if (ucSrcMask == 0)
           ucSrcMask = 0x80; // bottom up
     } // for ty
        RawWriteData(pBBEI, ucLine, h/2);
      } // for tx
  } else if (pBBEI->iOrientation == 270) {
      for (tx=x+w-1; tx>=x; tx--) {
      d = ucLine;
      ucSrcMask = 1 << (y & 7);
      for (ty=y; ty<y+h; ty+=2) {
          s = &pBuffer[tx + (ty>>3) * pBBEI->width];
          uc = 0x33; // start with white
          if (s[0] & ucSrcMask)
              uc = 0x03; // black
          if (s[iRedOff] & ucSrcMask)
              uc = 0x43; // red
          ucSrcMask <<= 1;
          if (s[0] & ucSrcMask) {// src pixel = black
              uc &= 0xf0;
          }
          if (s[iRedOff] & ucSrcMask) { // red
               uc &= 0xf0; uc |= 0x4;
          }
          ucSrcMask <<= 1;
         *d++ = uc; // store 2 pixels
          if (ucSrcMask == 0)
         ucSrcMask = 1;
      } // for ty
        RawWriteData(pBBEI, ucLine, h/2);
      } // for x
  } // 270
} /* EPDWriteImage4bpp() */
#endif // !WIMPY_MCU
const uint8_t epd294_init_sequence_part[] =
{
   0x0b, 0x37, 0,0,0,0,0,0x40,0,0,0,0,
    0x04, 0x01, 0x27, 0x01, 0x00,
    0x02, 0x11, 0x01,
    0x03, 0x44, 0x00, 0x0f,
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x03, 0x21, 0x00, 0x80,
    0x02, 0x3c, 0xc0,
   0x02, 0x22, 0xc0,
   0x01, 0x20,
   BUSY_WAIT,
   0x02, 0x4e, 0x00,
   0x03, 0x4f, 0x00, 0x00,
   0x00 // end of sequence
}; /* epd294_init_sequence_part */

const uint8_t epd293_lut[] =
{
    0x22,  0x11, 0x10, 0x00, 0x10, 0x00, 0x00, 0x11, 0x88, 0x80, 0x80, 0x80, 0x00, 0x00, 0x6A, 0x9B,
    0x9B, 0x9B, 0x9B, 0x00, 0x00, 0x6A, 0x9B, 0x9B, 0x9B, 0x9B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x18, 0x04, 0x16, 0x01, 0x0A, 0x0A, 0x0A, 0x0A, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x08, 0x3C, 0x07, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void EPD47_Begin(OBDISP *pBBEI, int x, int y, int w, int h, int bPartial)
{
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    EPD8951_CMD(pBBEI, 0x0001); // IT8951_TCON_SYS_RUN
    EPD8951_REG(pBBEI, 0x0004, 0x0001); // enable pack write
    // set vcom to -2.3v
    EPD8951_CMD(pBBEI, 0x0039);
    EPD8951_WORD(pBBEI, 0x0001);
    EPD8951_WORD(pBBEI, 2300);
    if (bPartial) {
        
    } else {
        uint16_t u16Args[5];
        // set target memory area
        EPD8951_REG(pBBEI, 0x000a, 0x0012); // _dev_memaddr_h
        EPD8951_REG(pBBEI, 0x0008, 0x36e0); // _dev_memaddr_l
        // set image area
        u16Args[0] = 0; // little endian, 2bpp, no rotation
        u16Args[1] = 0; // start x
        u16Args[2] = 0; // start y
        u16Args[3] = pBBEI->native_width;
        u16Args[4] = pBBEI->native_height;
        EPD8951_ARGS(pBBEI, 0x0021, u16Args, 5); // IT8951_TCON_LD_IMG_AREA
        EPD8951_CMD(pBBEI, 0x20); // IT8951_TCON_LD_IMG
        EPD8951_WORD(pBBEI, 0); // default settings (endian, format, rotation)
    }
} /* EPD47_Begin() */
//
// Special case for e-ink displays
//
static int EPDDumpBuffer(OBDISP *pBBEI, int bRefresh, int bWait, int bFast)
{
    long lTime;
    
    if (!(pBBEI->iFlags & BBEI_HAS_FAST_UPDATE))
        bFast = 0; // no fast update available
    
    pBBEI->iFlags |= BBEI_FULLUPDATE; // mark it for later
    // UC81xx need to be told the old and new images for fast updates
    // Keep a copy of the last full update for this purpose
    if (pBBEI->type >= EPD42_400x300 && pBBEI->chip_type == BBEI_CHIP_UC8151 && pBBEI->ucScreen && !(pBBEI->iFlags & BBEI_3COLOR)) {
         int iSize = pBBEI->width * ((pBBEI->height+7)>>3);
         memcpy(&pBBEI->ucScreen[iSize], pBBEI->ucScreen, iSize);
    }
    EPDWakeUp(pBBEI, 0); // tickle the reset line
    if (pBBEI->pInitFull) { // if we have the command list, use common code
        int iSize = pBBEI->width * ((pBBEI->height+7)>>3);
        int bInvert = 0;
        uint8_t *pBuf1 = NULL, *pBuf2 = NULL;
        
        lTime = millis(); // get data transmission time
        if (bFast && pBBEI->pInitFast) {
            Serial.println("Sending fast init");
            EPDSendCMDSequence(pBBEI, pBBEI->pInitFast);
        } else {
            EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
        }
        if (pBBEI->chip_type == BBEI_CHIP_UC8151) {
            if (pBBEI->ucScreen) { // we have a back buffer, send it
                if (pBBEI->iFlags & BBEI_3COLOR) { // B/W/R
                    bInvert = 1; // invert the red plane
                    pBuf1 = pBBEI->ucScreen; // B/W plane
                    pBuf2 = &pBBEI->ucScreen[iSize]; // R plane
                }
                EPDWriteImage(pBBEI, UC8151_DTM1, pBuf1, 0, 0, pBBEI->width, pBBEI->height, 0); // B/W or 'old' plane
                EPDWriteImage(pBBEI, UC8151_DTM2, pBuf2, 0, 0, pBBEI->width, pBBEI->height, bInvert); // R or 'new' plane
            }
            lTime = millis() - lTime;
            pBBEI->iDataTime = (int)lTime; // record the time to xfer data
            if (bRefresh) {
                lTime = millis();
                obdWriteCommand(pBBEI, UC8151_DRF);
                EPDWaitBusy(pBBEI, 0);
                lTime = millis() - lTime;
                pBBEI->iOpTime = (int)lTime; // record the update time
            }
        } else { // SSD16xx controllers
            if (pBBEI->ucScreen) { // we have a back buffer, send it
                uint8_t ucTemp[4];
                if (pBBEI->iFlags & BBEI_3COLOR) { // B/W/R
                    pBuf1 = pBBEI->ucScreen; // B/W plane
                    pBuf2 = &pBBEI->ucScreen[iSize]; // R plane
                    bInvert = 1; // invert the R plane
                }
                EPDWriteImage(pBBEI, SSD1608_WRITE_ALTRAM, pBuf2, 0, 0, pBBEI->width, pBBEI->height, bInvert);
                // need to reset the write pointer between planes
                EPD_CMD2(pBBEI, SSD1608_SET_RAMXCOUNT, 0x00);
                ucTemp[0] = ucTemp[1] = 0;
                obdWriteCommand(pBBEI, SSD1608_SET_RAMYCOUNT);
                RawWriteData(pBBEI, ucTemp, 2);
                EPDWriteImage(pBBEI, SSD1608_WRITE_RAM, pBuf1, 0, 0, pBBEI->width, pBBEI->height, 0);
            }
            lTime = millis() - lTime; // record data xfer time
            pBBEI->iDataTime = (int)lTime;
            if (bRefresh) {
                lTime = millis();
                EPD_CMD2(pBBEI, SSD1608_DISP_CTRL2, (bFast || (pBBEI->iFlags & BBEI_3COLOR)) ? 0xc7 : 0xf7);
                obdWriteCommand(pBBEI, SSD1608_MASTER_ACTIVATE); // refresh
                EPDWaitBusy(pBBEI, 0);
                lTime = millis() - lTime; // record the update time
                pBBEI->iOpTime = (int)lTime;
            }
        } // UC8151 / SSD16xx
        return 0;
    } // has init sequence
    // Special case EPDs below (4-gray, 4-color or 3-color/4-bpp)
#if !defined( WIMPY_MCU ) && !defined(__AVR__)
    // not enough RAM on AVR to use this type of EPD
    if (pBBEI->type == EPD42_4GRAY_400x300) {
        EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
        if (pBBEI->ucScreen) {
            EPDWriteImage(pBBEI, 0x10, NULL, 0, 0, pBBEI->width, pBBEI->height, 1); // MSB plane
            EPDWriteImage(pBBEI, 0x13, NULL, 0, 0, pBBEI->width, pBBEI->height, 0); // LSB plane
        }   
        obdWriteCommand(pBBEI, UC8151_LUT_VCOM);
        RawWriteData(pBBEI, (uint8_t *)lut42_4gray_vcom, 42);
        obdWriteCommand(pBBEI, UC8151_LUT_WW);
        RawWriteData(pBBEI, (uint8_t *)lut42_4gray_ww, 42);
        obdWriteCommand(pBBEI, UC8151_LUT_BW);
        RawWriteData(pBBEI, (uint8_t *)lut42_4gray_bw, 42);
        obdWriteCommand(pBBEI, UC8151_LUT_WB);
        RawWriteData(pBBEI, (uint8_t *)lut42_4gray_wb, 42);
        obdWriteCommand(pBBEI, UC8151_LUT_BB);
        RawWriteData(pBBEI, (uint8_t *)lut42_4gray_bb, 42);
        obdWriteCommand(pBBEI, UC8151_LUT_VCOM2);
        RawWriteData(pBBEI, (uint8_t *)lut42_4gray_vcom, 42);
        if (bRefresh)
           obdWriteCommand(pBBEI, UC8151_DRF); // refresh display
        else return 0;
    }
    if (pBBEI->type == EPD74R_640x384) {
        if (pBBEI->native_width == 640) {
            EPDSendCMDSequence(pBBEI, epd74r_init_sequence_full);
        } else {
            EPDSendCMDSequence(pBBEI, epd583r_init_sequence_full);
        }
        if (pBBEI->ucScreen) {
            EPDWriteImage4bpp(pBBEI, 0x10, 0, 0, pBBEI->width, pBBEI->height);
        }
        if (bRefresh) {
            obdWriteCommand(pBBEI, 0x12); // master activation
        }
        if (bWait) {
            EPDWaitBusy(pBBEI, 0);
        }
        return BBEI_SUCCESS;
    }
#endif // !WIMPY_MCU
    if (pBBEI->type == EPD30_BWYR_168x400 || pBBEI->type == EPD164_BWYR_168x168 || pBBEI->type == EPD236_BWYR_168x296) {
       EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
       if (pBBEI->ucScreen) {
          if (pBBEI->type == EPD164_BWYR_168x168 || pBBEI->type == EPD236_BWYR_168x296) {
             EPD_CMD2(pBBEI, 0x68, 0x01);
             obdWriteCommand(pBBEI, 0x04);
             EPDWaitBusy(pBBEI, 0);
          }
          EPDWriteImage2bpp(pBBEI, 0x10, 0, 0, pBBEI->width, pBBEI->height);
       }
       if (pBBEI->type == EPD164_BWYR_168x168 || pBBEI->type == EPD236_BWYR_168x296) {
             EPD_CMD2(pBBEI, 0x68, 0x00);
       }
       EPD_CMD2(pBBEI, 0x12, 0x01); // display refresh
    }
    if (pBBEI->type == EPD266_BWYR_184x360 || pBBEI->type == EPD29_BWYR_168x384 || pBBEI->type == EPD29_BWYR_128x296) {
       EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
       if (pBBEI->ucScreen) {
          EPDWriteImage2bpp(pBBEI, 0x10, 0, 0, pBBEI->width, pBBEI->height);
       }
       EPD_CMD2(pBBEI, 0x12, 0x00); // display refresh
    }
    if (pBBEI->type == EPD42R2_400x300) {
        EPDSendCMDSequence(pBBEI, pBBEI->pInitFull);
       // EPD27_SetLut(pBBEI, false);
#ifndef WIMPY_MCU
        if (pBBEI->ucScreen) {
            EPDWriteImage(pBBEI, 0x10, NULL, 0, 0, pBBEI->width, pBBEI->height, 0); // black/white plane
            EPDWriteImage(pBBEI, 0x13, NULL, 0, 0, pBBEI->width, pBBEI->height, 1); // red plane
        }
#endif // !WIMPY_MCU
        if (bRefresh) {
            obdWriteCommand(pBBEI, 0x12); // master activation
        }
    }
    if (pBBEI->type == EPD213R_104x212_d) {
        EPDSendCMDSequence(pBBEI, EPD213R_104x212_d_init_sequence_full);
        EPD27_SetLut(pBBEI, false);
#ifndef WIMPY_MCU
        if (pBBEI->ucScreen) {
            EPDWriteImage(pBBEI, 0x10, NULL, 0, 0, pBBEI->width, pBBEI->height, 0); // black/white plane
            EPDWriteImage(pBBEI, 0x13, NULL, 0, 0, pBBEI->width, pBBEI->height, 1); // red plane
        }
#endif // !WIMPY_MCU
        if (bRefresh) {
            obdWriteCommand(pBBEI, 0x12); // master activation
        }
    }
    if (pBBEI->type == EPD579_792x272) {
        EPD_CMD2(pBBEI, SSD1608_DISP_CTRL2, 0xf7);
        obdWriteCommand(pBBEI, SSD1608_MASTER_ACTIVATE);
    }
    if (pBBEI->type == EPD47_540x960) {
        uint16_t u16Args[7];
        EPD47_Begin(pBBEI, 0, 0, pBBEI->width, pBBEI->height, false);
#ifndef WIMPY_MCU
        if (pBBEI->ucScreen) {
            EPDWriteImage(pBBEI, 0, NULL, 0, 0, pBBEI->width, pBBEI->height, 0);
        }
#endif
        EPD8951_CMD(pBBEI, 0x0022); // LCD_IMG_END
        u16Args[0] = 0; // x
        u16Args[1] = 0; // y
        u16Args[2] = pBBEI->native_width;
        u16Args[3] = pBBEI->native_height;
        u16Args[4] = 0; // full update mode (2 seconds)
        u16Args[5] = 0x36e0; // _dev_memaddr_l
        u16Args[6] = 0x0012; // _dev_memaddr_h
        EPD8951_ARGS(pBBEI, 0x0037, u16Args, 7); // IT8951_I80_CMD_DPY_BUF_AREA
    }
    if (bWait) {
        EPDWaitBusy(pBBEI, 0);
        EPDSleep(pBBEI, 0);
    }
    return BBEI_SUCCESS;
} /* EPDDumpBuffer() */
//
// Special case for ST7302
//
#ifndef WIMPY_MCU
void obdST7302SetPos(OBDISP *pBBEI, int x, int y)
{
uint8_t ucTemp[4];
    
    obdWriteCommand(pBBEI, 0x2a); // Column set
    ucTemp[0] = 0x19 + (y/12); // start x (we treat it as y)
    ucTemp[1] = 0x27; // end x
    RawWriteData(pBBEI, ucTemp, 2);
    obdWriteCommand(pBBEI, 0x2b); // Row set
    ucTemp[0] = (x/2); // start y (we treat it as x)
    ucTemp[1] = 0x80; // end y
    RawWriteData(pBBEI, ucTemp, 2);
    obdWriteCommand(pBBEI, 0x2c); // memory write
} /* obdST7302SetPos() */
//
// Special case for ST7302
//
static int ST7302DumpBuffer(OBDISP *pBBEI, uint8_t *pBuffer)
{
uint8_t ucPixels[40];
int x, y, iPitch, count;
uint8_t ucMask, uc1, *s, *d;
    
    iPitch = pBBEI->width;
// Shift out the image in pairs of lines
    ucPixels[0] = 0x40;
    switch (pBBEI->iOrientation) {
        case 0:
        case 180:
            for (x = 0; x < pBBEI->width; x += 2) { // a pair of columns at a time
                d = ucPixels+1;
                ucMask = 1;
                uc1 = 0;
                count = 0;
                if (pBBEI->iOrientation == 180) {
                    ucMask = 1<<((pBBEI->height-1) & 7);
                    s = &pBuffer[pBBEI->width-2-x + ((pBBEI->height-1)>>3)*iPitch];
                    for (y = pBBEI->height-1; y >= 0; y--) {
                        uc1 <<= 2;
                        if (s[0] & ucMask) uc1 |= 1;
                        if (s[1] & ucMask) uc1 |= 2;
                        count++;
                        if (count == 4) { // finish the byte
                            *d++ = uc1;
                            count = 0;
                            uc1 = 0;
                        }
                        ucMask >>= 1;
                        if (ucMask == 0) {
                            ucMask = 0x80;
                            s -= iPitch;
                        } // next line
                    } // for y
                    if (count) {
                        uc1 <<= (count*2);
                        *d++ = uc1;
                    }
                } else {
                    s = &pBuffer[x];
                    uc1 = 0;
                    for (y=0; y < pBBEI->height+3; y++) {
                        uc1 <<= 2;
                        if (s[0] & ucMask) uc1 |= 2;
                        if (s[1] & ucMask) uc1 |= 1;
                        count++;
                        if (count == 4) { // finish the byte
                            *d++ = uc1;
                            count = 0;
                            uc1 = 0;
                        }
                        ucMask <<= 1;
                        if (ucMask == 0) {
                            ucMask = 1;
                            s += iPitch;
                        } // next line
                    } // for y
                } // flipped
                obdSetPosition(pBBEI, (pBBEI->flip) ? 248-x : x, 0, 1);
                RawWrite(pBBEI, ucPixels, 3 + (int)(d - ucPixels));
            } // for x
            break;
        case 90:
        case 270: // x/y swapped
            if (pBBEI->iOrientation == 270) {
                for (y=pBBEI->height-2; y >= 0; y-=2) {
                    ucMask = 1<<(y & 7);
                    s = &pBuffer[(y>>3)*pBBEI->width];
                    count = 0;
                    d = ucPixels+1;
                    uc1 = 0;
                    for (x=0; x<pBBEI->width; x++) {
                        uc1 <<= 2;
                        if (s[x] & ucMask) uc1 |= 1;
                        if (s[x] & (ucMask << 1)) uc1 |= 2;
                        count++;
                        if (count == 4) { // finish the byte
                            *d++ = uc1;
                            count = 0;
                        }
                    } // for x
                    obdSetPosition(pBBEI, pBBEI->height - 2 - y, 0, 1);
                    RawWrite(pBBEI, ucPixels, 3 + (int)(d - ucPixels));
                } // for y
            } else { // 90
                for (y=0; y < pBBEI->height; y+=2) {
                    ucMask = 1<<(y & 7);
                    s = &pBuffer[(y>>3)*pBBEI->width];
                    count = 0;
                    d = ucPixels+1;
                    uc1 = 0;
                    for (x=pBBEI->width-1; x>=0; x--) {
                        uc1 <<= 2;
                        if (s[x] & ucMask) uc1 |= 2;
                        if (s[x] & (ucMask << 1)) uc1 |= 1;
                        count++;
                        if (count == 4) { // finish the byte
                            *d++ = uc1;
                            count = 0;
                        }
                    } // for x
                    obdSetPosition(pBBEI, y, 0, 1);
                    RawWrite(pBBEI, ucPixels, 3 + (int)(d - ucPixels));
                } // for y
            } // flipped
            break;
    } // switch
    return BBEI_SUCCESS;
} /* ST7302DumpBuffer() */
#endif // !WIMPY_MCU
//
// Special case for Sharp Memory LCD
//
static int SharpDumpBuffer(OBDISP *pBBEI, uint8_t *pBuffer)
{
int x, y;
uint8_t c, ucInvert, ucStart, ucMask;
uint8_t *s, *d;
uint8_t ucLineBuf[64];
int iPitch = pBBEI->native_width / 8;
static int ucVCOM = 0;
int iBit;

  ucInvert = (pBBEI->invert) ? 0x00 : 0xff;
  digitalWrite(pBBEI->iCSPin, HIGH); // active high
    ucLineBuf[0] = 0; // Tell RawWrite that it's command (doesn't matter for Sharp LCDs)
  ucStart = 0x80; // write command
  if (ucVCOM)
    ucStart |= 0x40; // VCOM bit
  ucLineBuf[1] = ucStart;
  // this code assumes I2C, so the first byte is ignored
  RawWrite(pBBEI, ucLineBuf, 2); // write command(01) + vcom(02)
  ucVCOM = !ucVCOM; // need to toggle this each transaction

 // We need to flip and invert the image in code because the Sharp memory LCD
 // controller only has the simplest of commands for data writing
  if (pBBEI->iOrientation == 180)
  {
     for (y=0; y<pBBEI->height; y++) // we have to write the memory in the wrong direction
     {
         ucMask = 0x80 >> (y & 7);
        s = &pBuffer[pBBEI->width - 1 + (pBBEI->width * ((pBBEI->height - 1 - y) >> 3))]; // point to last line first
        d = &ucLineBuf[2];
        ucLineBuf[1] = pgm_read_byte(&ucMirror[y+1]); // current line number
        for (x=0; x<pBBEI->width/8; x++)
        {
           c = ucInvert; // we need to brute-force invert it
            for (iBit=7; iBit>=0; iBit--)
            {
                if (s[0] & ucMask)
                    c ^= (1 << iBit);
                s--;
            }
           *d++ = c;
        } // for y
        // write this line to the display
        ucLineBuf[iPitch+2] = 0; // end of line
        RawWrite(pBBEI, ucLineBuf, iPitch+3);
     } // for x
  }
  else if (pBBEI->iOrientation == 0) // normal orientation
  {
     for (y=0; y<pBBEI->height; y++) // we have to write the memory in the wrong direction
     {
        ucMask = 1 << (y & 7);
        s = &pBuffer[pBBEI->width * (y >> 3)]; // point to last line first
        d = &ucLineBuf[2];
        ucLineBuf[1] = pgm_read_byte(&ucMirror[y+1]); // current line number
        for (x=0; x<pBBEI->width/8; x++)
        {
            c = ucInvert;
            for (iBit=7; iBit>=0; iBit--)
            {
                if (s[0] & ucMask)
                    c ^= (1 << iBit);
                s++;
            }
           *d++ = c;
        } // for x
        // write this line to the display
        *d++ = 0; // end of line
        RawWrite(pBBEI, ucLineBuf, iPitch+3);
     } // for y
  } else if (pBBEI->iOrientation == 90) {
     for (x=0; x<pBBEI->width; x++) {
	s = &pBuffer[x+((pBBEI->height-1)>>3)*pBBEI->width];
	d = &ucLineBuf[2];
	ucLineBuf[1] = pgm_read_byte(&ucMirror[x+1]); // line number
        for (y=0; y<pBBEI->height; y+=8) { // we have to write the memory in the wrong direction
            *d++ = s[0] ^ ucInvert;
	    s -= pBBEI->width;
        } // for y
	ucLineBuf[iPitch+2] = 0; // end of line
	RawWrite(pBBEI, ucLineBuf, iPitch+3);
     } // for x
  } else { // must be 270
     for (x=0; x<pBBEI->width; x++) {
	 d = &ucLineBuf[2];
	 s = &pBuffer[(pBBEI->width-1-x)];
	 ucLineBuf[1] = pgm_read_byte(&ucMirror[x+1]); // line number
         for (y=0; y<pBBEI->height; y+=8) {
	     *d++ = pgm_read_byte(&ucMirror[s[0] ^ ucInvert]);
	     s += pBBEI->width;
	 } // for y
	 ucLineBuf[iPitch+2] = 0; // end of line
	 RawWrite(pBBEI, ucLineBuf, iPitch+3);
     } // for x
  } // 270
  ucLineBuf[1] = 0;
  RawWrite(pBBEI, ucLineBuf, 2); // final transfer
  digitalWrite(pBBEI->iCSPin, LOW); // de-activate
    return BBEI_SUCCESS;
} /* SharpDumpBuffer() */
//
// Dump a screen's worth of data directly to the display
// Try to speed it up by comparing the new bytes with the existing buffer
//
int obdDumpBuffer(OBDISP *pBBEI, uint8_t *pBuffer, int bRefresh, int bWait, int bFast)
{
int x, y;
int iLines;
    
    if (pBBEI->type == LCD_VIRTUAL) { // wrong function for this type of display
        return BBEI_ERROR_BAD_PARAMETER;
    }
    if (pBuffer == NULL) { // dump the internal buffer if none is given
        pBuffer = pBBEI->ucScreen;
    }
//    if (pBuffer == NULL) {
//        return; // no backbuffer and no provided buffer
//    }
    
  if (pBBEI->type >= EPD42_400x300) // all e-ink panels
  {
     return EPDDumpBuffer(pBBEI, bRefresh, bWait, bFast);
  }
#if !defined( WIMPY_MCU ) && !defined(__AVR__)
  if (pBBEI->type == LCD_ST7302) // special case for ST7302
  {
      return ST7302DumpBuffer(pBBEI, pBuffer);
  }
#endif // !WIMPY_MCU
  if (pBBEI->chip_type == BBEI_CHIP_SHARP) // special case for Sharp Memory LCDs
  {
    return SharpDumpBuffer(pBBEI, pBuffer);
  }
  iLines = (pBBEI->native_height+7) >> 3;
    // 0/180 we can send the 8 lines of pixels straight through
    if (pBBEI->iOrientation == 0 || pBBEI->iOrientation == 180) {
        if (pBBEI->iOrientation == 0 || (pBBEI->iOrientation == 180 && pBBEI->can_flip)) {
            for (y=0; y<iLines; y++) {
                obdSetPosition(pBBEI, 0, y*8, 1);
                RawWriteData(pBBEI, pBuffer, pBBEI->width);
                pBuffer += pBBEI->width;
            }
        } else { // have to manually do 180 flipped
            for (y=iLines-1; y>=0; y--) {
                obdSetPosition(pBBEI, 0, y*8, 1);
                for (int x=0; x<pBBEI->width; x++) {
                    u8Cache[pBBEI->width-1-x] = ucMirror[pBuffer[x]];
                }
                RawWriteData(pBBEI, u8Cache, pBBEI->width);
                pBuffer += pBBEI->width;
            }
        }
     return BBEI_SUCCESS;
  } else { // must be 90/270
      // Capture the pixels 'sideways' and send a line at a time
     if (pBBEI->iOrientation == 90 || (pBBEI->iOrientation == 270 && pBBEI->can_flip)) {
          for (x=0; x<pBBEI->width; x+=8) {
              uint8_t *s, *d, uc, ucMask, ucTemp[132];
              int j;
              d = ucTemp;
              *d++ = 0x40;
              s = &pBuffer[x + ((pBBEI->height-7)>>3)*pBBEI->width];
              ucMask = 0x80;
              for (y=0; y<pBBEI->height; y++) {
                  uc = 0;
                  for (j=7; j>=0; j--) {
                      uc <<= 1;
                      if (s[j] & ucMask) uc |= 1;
                  } // for j
                  *d++ = uc;
                  ucMask >>= 1;
                  if (ucMask == 0) {
                      ucMask = 0x80;
                      s -= pBBEI->width;
                  }
              } // for y
              obdSetPosition(pBBEI, 0, x, 1);
              RawWrite(pBBEI, ucTemp, pBBEI->height + 1);
          } // for x
      } else { // 270 degrees and can't flip 180
          for (x=pBBEI->width-8; x>=0; x-=8) {
              uint8_t j, *s, *d, uc, ucMask, ucTemp[132];
              d = ucTemp;
              *d++ = 0x40;
              s = &pBuffer[x];
              ucMask = 1;
              for (y=0; y<pBBEI->height; y++) {
                  uc = 0;
                  for (j=0; j<8; j++) {
                      uc <<= 1;
                      if (s[j] & ucMask) uc |= 1;
                  } // for j
                  *d++ = uc;
                  ucMask <<= 1;
                  if (ucMask == 0) {
                      ucMask = 1;
                      s += pBBEI->width;
                  }
              } // for y
              obdSetPosition(pBBEI, 0, pBBEI->width-8-x, 1);
              RawWrite(pBBEI, ucTemp, pBBEI->height + 1);
          } // for x
      }
  } // 90/270 degrees rotated
    return BBEI_SUCCESS;
} /* obdDumpBuffer() */

// Send a single byte command to the OLED controller
void obdWriteCommand(OBDISP *pBBEI, unsigned char c)
{
#ifndef MEMORY_ONLY
unsigned char buf[4];

    buf[0] = 0x00; // command introducer
    buf[1] = c;
    RawWrite(pBBEI, buf, 2);
#endif // MEMORY_ONLY
} /* obdWriteCommand() */
//
// Experimental bufferless E-ink drawing code
//
void EPDSetPosition(OBDISP *pBBEI, int x, int y, int cx, int cy)
{
uint8_t uc[12];
int i, tx, ty;
    
    // We need to rotate the drawing direction since native direction for EPDs
    // is horizontal bytes across the X direction with the MSB on the left
    if (pBBEI->iOrientation == 90 || pBBEI->iOrientation == 270) {
        if (x + cx > pBBEI->native_height)
            cx = pBBEI->native_height - x;
        if (y + cy > pBBEI->native_width)
            cy = pBBEI->native_width - y;
        y &= 0xff8; // fix to byte boundary
        if ((pBBEI->iFlags & BBEI_FLIPV) || (pBBEI->ucScreen == NULL/* && pBBEI->chip_type == BBEI_CHIP_SSD16xx*/)) // flipped Y when drawing with no back buffer
            tx = (pBBEI->native_width /*+ 7*/ - y - cy)/8;
        else
            tx = y/8;
        ty = x;
        // swap width/height
        i = cx;
        cx = cy;
        cy = i;
    } else {
        tx = (x+7)/8;
        ty = y;
        if (ty+cy > pBBEI->native_height) {
            cy = pBBEI->native_height - ty;
        }
        if ((tx*8)+cx > pBBEI->native_width) {
            cx = pBBEI->native_width - (tx*8);
        }
    }
    switch (pBBEI->chip_type) {
        case BBEI_CHIP_SSD16xx:
            if (pBBEI->type == EPD579_792x272) {  // special case
                EPD_CMD2(pBBEI, SSD1608_DATA_MODE, 0x02);
                if (cx > 400 && cy == 8) { // a single line of graphics
                    cx = 8;
                    cy = 271-ty; // different mem direction
                }
                if (cx > 400) cx = 400; // two sets of 400 pixels
                obdWriteCommand(pBBEI, SSD1608_SET_RAMXPOS);
                uc[1] = tx; // start x (byte boundary)
                uc[0] = tx+(cx>>3); // end x
                RawWriteData(pBBEI, uc, 2);
                obdWriteCommand(pBBEI, SSD1608_SET_RAMYPOS);
                uc[0] = (uint8_t)ty; // start y
                uc[1] = (uint8_t)(ty>>8);
                uc[2] = (uint8_t)(ty+cy-1); // end y
                uc[3] = (uint8_t)((ty+cy-1)>>8);
                RawWriteData(pBBEI, uc, 4);
                EPD_CMD2(pBBEI, SSD1608_SET_RAMXCOUNT, 0xf);
                obdWriteCommand(pBBEI, SSD1608_SET_RAMYCOUNT);
                uc[0] = uc[1] = 0;
                RawWriteData(pBBEI, uc, 2);
                if (pBBEI->iFG == BBEI_RED) // write to red plane
                    obdWriteCommand(pBBEI, SSD1608_WRITE_ALTRAM);
                else
                    obdWriteCommand(pBBEI, SSD1608_WRITE_RAM); // prepare to start writing pixels
            } else {
                obdWriteCommand(pBBEI, SSD1608_SET_RAMXPOS);
                uc[0] = tx + pBBEI->x_offset; // start x (byte boundary)
               // if (cx == pBBEI->native_width && (pBBEI->native_width & 7) != 0)
               //     cx += 7; // needs one more byte for the full display width
                uc[1] = tx+((cx-1)>>3) + pBBEI->x_offset; // end x
                RawWriteData(pBBEI, uc, 2);
                obdWriteCommand(pBBEI, SSD1608_SET_RAMYPOS);
                if (pBBEI->iFlags & BBEI_FLIPV) {
                    uc[0] = (uint8_t)(ty+cy-1); // start y
                    uc[1] = (uint8_t)((ty+cy-1)>>8);
                    uc[2] = (uint8_t)ty; // end y
                    uc[3] = (uint8_t)(ty>>8);
                } else {
                    uc[0] = (uint8_t)ty; // start y
                    uc[1] = (uint8_t)(ty>>8);
                    uc[2] = (uint8_t)(ty+cy-1); // end y
                    uc[3] = (uint8_t)((ty+cy-1)>>8);
                }
                RawWriteData(pBBEI, uc, 4);
                // set ram counter to start of this region
                EPD_CMD2(pBBEI, SSD1608_SET_RAMXCOUNT, tx+pBBEI->x_offset);
                uc[0] = ty;
                uc[1] = (ty>>8);
                obdWriteCommand(pBBEI, SSD1608_SET_RAMYCOUNT);
                RawWriteData(pBBEI, uc, 2);
                if (pBBEI->iFlags & BBEI_FLIPV)
                    EPD_CMD2(pBBEI, SSD1608_DATA_MODE, 0x1);
                else
                    EPD_CMD2(pBBEI, SSD1608_DATA_MODE, 0x3);
                if (pBBEI->iFG == BBEI_RED) // write to red plane
                    obdWriteCommand(pBBEI, SSD1608_WRITE_ALTRAM);
                else
                    obdWriteCommand(pBBEI, SSD1608_WRITE_RAM); // prepare to start writing pixels
            }
            break;
        case BBEI_CHIP_UC8151:
            obdWriteCommand(pBBEI, UC8151_PTIN); // partial in
            obdWriteCommand(pBBEI, UC8151_PTL); // partial window
            i = 0;
            tx *= 8;
            if (pBBEI->native_width >= 256) { // need 2 bytes per x
                uc[i++] = (uint8_t)(tx>>8); // start x
                uc[i++] = (uint8_t)tx;
                uc[i++] = (uint8_t)((tx+cx-1)>>8); // end x
                uc[i++] = (uint8_t)((tx+cx-1) | 7);
            } else {
                uc[i++] = tx; // start x
                uc[i++] = (tx+cx-1) | 7; // end x
            }
            if (pBBEI->iFlags & BBEI_FLIPV) {
                if (pBBEI->native_height >= 256) {
                   uc[i++] = (uint8_t)((ty+cy-1)>>8); // start y
                   uc[i++] = (uint8_t)(ty+cy-1);
                   uc[i++] = (uint8_t)(ty>>8); // end y
                   uc[i++] = (uint8_t)ty;
                } else {
                   uc[i++] = (uint8_t)(ty+cy-1);
                   uc[i++] = (uint8_t)ty;
                }
            } else {
                if (pBBEI->native_height >= 256) {
                   uc[i++] = (uint8_t)(ty>>8); // start y
                   uc[i++] = (uint8_t)ty;
                   uc[i++] = (uint8_t)((ty+cy-1)>>8); // end y
                   uc[i++] = (uint8_t)(ty+cy-1);
                } else {
                   uc[i++] = (uint8_t)ty;
                   uc[i++] = (uint8_t)(ty+cy-1);
                }
            }
            uc[i++] = 0; // stay inside the partial window3
            RawWriteData(pBBEI, uc, i);
            if (pBBEI->iFlags & BBEI_3COLOR) {
                if (pBBEI->iFG == BBEI_RED) // write to red plane
                    obdWriteCommand(pBBEI, UC8151_DTM2);
                else
                    obdWriteCommand(pBBEI, UC8151_DTM1); // prepare to start writing pixels
            } else {
                obdWriteCommand(pBBEI, UC8151_DTM2);
            }
            break;
        default: // error
            break;
    }
} /* EPDSetPosition() */
//
// Send commands to position the "cursor" (aka memory write address)
// to the given row and column
//
void obdSetPosition(OBDISP *pBBEI, int x, int y, int bRender)
{
unsigned char buf[4];
int iPitch = pBBEI->width;

    // e-paper direct access
    if (pBBEI->type >= EPD42_400x300 && !pBBEI->ucScreen) { // immediate mode for e-paper displays
        EPDSetPosition(pBBEI, x, y, 800, 8); // let the width get corrected in the command prep
        return;
    }
#if !defined( WIMPY_MCU ) && !defined(__AVR__)
    if (pBBEI->type == LCD_ST7302) { // special case for ST7302
        obdST7302SetPos(pBBEI, x, y);
    }
#endif // !WIMPY_MCU
    y >>= 3; // DEBUG - since we address the display by lines of 8 pixels
  pBBEI->iScreenOffset = (y*iPitch)+x;

  if (pBBEI->type == LCD_VIRTUAL || pBBEI->chip_type == BBEI_CHIP_SHARP)
    return; // nothing to do
  if (!bRender)
      return; // don't send the commands to the OLED if we're not rendering the graphics now
    obdCachedFlush(pBBEI, bRender); // flush any cached data first
  if (pBBEI->type == LCD_NOKIA5110)
  {
      obdWriteCommand(pBBEI, 0x40 | y);
      obdWriteCommand(pBBEI, 0x80 | x);
      return;
  }
  if (pBBEI->type == OLED_80x128) // visible portion starts at column 24, row 0
  {
    x += 24;
  }
  else if (pBBEI->type == LCD_32x64)
  {
      x += 50;
  }
  else if (pBBEI->type == OLED_64x32) // visible display starts at column 32, row 4
  {
    x += 32; // display is centered in VRAM, so this is always true
    if (pBBEI->flip == 0) // non-flipped display starts from line 4
       y += 4;
  }
  else  if (pBBEI->type == OLED_64x48) {
      x += 32;
      if (!pBBEI->flip)
          y += 2; // 128x64 internal RAM
  }
  else if (pBBEI->type == LCD_UC1701 && pBBEI->flip)
  {
    x += 4;
  }
  else if (pBBEI->type == OLED_132x64) // SH1106 has 128 pixels centered in 132
  {
    x += 2;
  }
  else if (pBBEI->type == OLED_96x16) // visible display starts at line 2
  { // mapping is a bit strange on the 96x16 OLED
    if (pBBEI->flip)
      x += 32;
    else
      y += 2;
  }
  else if (pBBEI->type == OLED_72x40) // starts at x=28,y=3
  {
    x += 28;
    if (!pBBEI->flip)
    {
      y += 3;
    }
  }
    if (pBBEI->com_mode == COM_I2C) { // I2C device
      buf[0] = 0x00; // command introducer
      buf[1] = 0xb0 | y; // set page to Y
      buf[2] = x & 0xf; // lower column address
      buf[3] = 0x10 | (x >> 4); // upper column addr
      RawWrite(pBBEI, buf, 4);
    } else { // SPI mode
        obdWriteCommand(pBBEI, 0xb0 | y); // set Y
        obdWriteCommand(pBBEI, 0x10 | (x >> 4)); // set X MSB
        obdWriteCommand(pBBEI, 0x00 | (x & 0xf)); // set X LSB
    }
} /* obdSetPosition() */

//
// Write a block of pixel data to the OLED
// Length can be anything from 1 to 1024 (whole display)
//
void obdWriteDataBlock(OBDISP *pBBEI, unsigned char *ucBuf, int iLen, int bRender)
{
int iPitch, iBufferSize;
int iRedOffset = 0;
    
  iPitch = pBBEI->width;
  iBufferSize = iPitch * ((pBBEI->height+7) / 8);
    if (pBBEI->iFG >= BBEI_YELLOW) {
        iRedOffset = pBBEI->width * ((pBBEI->height+7)/8);
    }
// Keep a copy in local buffer
if (pBBEI->ucScreen && (iLen + pBBEI->iScreenOffset) <= iBufferSize)
{
  memcpy(&pBBEI->ucScreen[iRedOffset + pBBEI->iScreenOffset], ucBuf, iLen);
  pBBEI->iScreenOffset += iLen;
  // wrap around ?
  if (pBBEI->iScreenOffset >= iBufferSize)
    pBBEI->iScreenOffset -= iBufferSize;
}
if (pBBEI->type == LCD_VIRTUAL || pBBEI->chip_type == BBEI_CHIP_SHARP)
  return; // nothing else to do
#ifndef MEMORY_ONLY
  if (bRender)
  {
      RawWriteData(pBBEI, ucBuf, iLen);
  }
#endif // MEMORY_ONLY
} /* obdWriteDataBlock() */
