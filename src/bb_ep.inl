//
// bb_epaper
// Copyright (c) 2024 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
// Project started 9/11/2024
//
// Use of this software is governed by the Business Source License
// included in the file ./LICENSE.
//
// As of the Change Date specified in that file, in accordance with
// the Business Source License, use of this software will be governed
// by the Apache License, Version 2.0, included in the file
// ./APL.txt.
//
// bb_ep.inl
// display interfacing/control code for bb_eink library
//
#ifndef __BB_EP__
#define __BB_EP__
// forward declarations
void InvertBytes(uint8_t *pData, uint8_t bLen);

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

const uint8_t epd29_init_sequence_part[] PROGMEM = {
    0x02, UC8151_PSR, 0x80 | 0x20 | 0x10 | 0x08 | 0x04 | 0x02 | 0x01, // RES_128x296, LUT_REG, FORMAT_BW, SHIFT_LEFT, BOOSTER_ON, RESET_NONE
    0x06, UC8151_PWR, 0x03, 0x00, 0x2b, 0x2b, 0x03,
    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x04, UC8151_TRES, 0x80, 1, 0x28, // resolution
    0x02, UC8151_PLL, 0x3a, // HZ_100
    0x02, UC8151_VDCS, 0x12, // VCOM DC setting
    0x02, UC8151_CDI, 0xBC, // white border & non-inverted partial update old->new changes
    0x2d, UC8151_LUT_VCOM, // VCOM LUT
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        ,0x00, 0x00,
    0x2b, UC8151_LUT_WW, // WW LUT
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_BW,
        0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_WB,
        0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_BB,
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00 // end of table
};
const uint8_t epd42r_init_sequence_full[] PROGMEM = {
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
const uint8_t epd29r_init_sequence_full[] PROGMEM = {
    0x01, 0x12, // soft reset
    BUSY_WAIT,
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart
    0x04, 0x01, 0x27, 0x01, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x0f, // RAM X start/end
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01, // RAM Y start/end
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

// for 122x250 BWR
const uint8_t epd213r_inky_init_sequence_full[] PROGMEM = {
    0x01, 0x12, // soft reset
    BUSY_WAIT, 
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart 
    0x04, 0x01, 249, 0x00, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode 
    0x03, 0x44, 0x00, 0xf, // RAM X start/end
    0x05, 0x45, 0x00, 0x00, 249, 0x00, // RAM Y start/end
    0x02, 0x3c, 0x01, // border (0=bk,1=wh,2=red)
    0x02, 0x18, 0x80, // temp sensor = internal 
    0x02, 0x21, 0x00, // display update ctrl 1
    0x02, 0x22, 0xb1, // display update ctrl 2
    0x01, 0x20, // master activation
    BUSY_WAIT,
    0x02, 0x4e, 0x00, // RAM X counter
    0x03, 0x4f, 249, 0x00, // RAM Y counter
    0x00
};

// for 152x152 BWR
const uint8_t epd154r_init_sequence_full[] PROGMEM = {
    0x01, 0x12, // soft reset
    BUSY_WAIT,
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart
    0x04, 0x01, 0x97, 0x00, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x12, // RAM X start/end
    0x05, 0x45, 0x00, 0x00, 0x97, 0x00, // RAM Y start/end
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

const uint8_t epd29yr_init_sequence_full[] PROGMEM = 
{
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
const uint8_t epd29yrh_init_sequence_full[] PROGMEM = 
{
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

const uint8_t epd266yr_init_sequence_full[] PROGMEM = 
{
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

const uint8_t epd154_init_sequence_fast[] PROGMEM = 
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
    0x02, 0x11, 0x03,
    0x03, 0x44, 0x00, 0x18,
    0x05, 0x45, 0x00, 0x00, 199, 0x00,
    0x02, 0x3c, 0x80, // border waveform
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    0

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

const uint8_t epd122_init_sequence_full[] PROGMEM =
{
    0x01, SSD1608_SW_RESET,
    BUSY_WAIT,
    
    0x04, 0x01, 0xaf, 0x00, 0x00, // driver output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x17, // ram start/end
    0x05, 0x45, 0x00, 0x00, 0xbf, 0x00,
    0x02, 0x3c, 0x05, // border waveform
    0x02, 0x18, 0x80, // read built-in temp sensor
    
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
};

const uint8_t epd122_init_sequence_fast[] PROGMEM =
{
    0x01, SSD1608_SW_RESET,
    BUSY_WAIT,
    
    0x02, 0x11, 0x03, // data entry mode
    0x02, 0x18, 0x80, // read built-in temp sensor
    0x02, 0x22, 0xb1, // load temp value
    0x01, 0x20, // execute
    BUSY_WAIT,
    0x03, 0x1a, 0x64, 0x00, // write temp value
    0x02, 0x22, 0x91, // load temp
    0x01, 0x20, // execute
    BUSY_WAIT,
    0x03, 0x44, 0x00, 0x17, // ram start/end
    0x05, 0x45, 0x00, 0x00, 0xaf, 0x00,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
};

const uint8_t epd122_init_sequence_part[] PROGMEM =
{
    0x02, 0x3c, 0x80, // border waveform
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x17, // ram start/end
    0x05, 0x45, 0x00, 0x00, 0xaf, 0x00,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    BUSY_WAIT,
    0x00 // end of table
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

const uint8_t epd27_ws_init_sequence_full[] = {
    1, UC8151_PON,
    BUSY_WAIT,
    3, 0x00, 0x1f, 0x0b, // panel setting
    5, UC8151_TRES, 0x01, 0x08, 0x00, 0xb0,
    2, 0x50, 0x97, // VCOM
    0
};

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
    1, 0x04, // power on
    BUSY_WAIT,
    
    0x00 // end of table
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
//    2, 0x50, 0x97, // VCOM
    5, 0x01, 0x03,0x00,0x2b,0x2b, // power setting
    4, 0x06, 0x17,0x17,0x17, // boost soft start
    1, 0x04, // power on
    BUSY_WAIT,
    3, 0x00, 0xbf, 0x0b, // panel setting
    2, 0x30, 0x3c, // 3A 100HZ
    5, 0x61, 0x01, 0x90, 0x01, 0x2c, // resolution 400x300
    2, 0x82, 0x12, // vcom_DC
    2, 0x50, 0xB7, // mode
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

const uint8_t epd213_inky_init_sequence_full[] PROGMEM = {
    5, UC8151_PWR, 0x07, 0x00, 0x0a, 0x00,
    4, UC8151_BTST, 0x07, 0x07, 0x07,
    1, UC8151_PON, // power on
    BUSY_WAIT,
    2, UC8151_PSR, 0xcf,
    2, 0x50, 0x07,
    2, 0x30, 0x29,
    4, UC8151_TRES, 104, 0, 212,
    2, 0x82, 0x0a,
    
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

uint8_t u8Cache[128]; // buffer a single line of up to 1024 pixels
//
// Definitions for each supported panel
// The order tracks that of the enumerated panel types
// ** ONLY ADD NEW PANELS TO THE END OF THE LIST **
//
const EPD_PANEL panelDefs[] PROGMEM = {
    {0}, // undefined panel
    {400, 300, 0, epd42_init_sequence_full, NULL, epd42_init_sequence_part, 0, BBEP_CHIP_UC81xx}, // EP42_400x300
    {400, 300, 0, epd42b_init_sequence_full, epd42b_init_sequence_fast, epd42b_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP42B_400x300
    {122, 250, 0, epd213_122x250_init_sequence_full, NULL, epd213_122x250_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP213_122x250 WaveShare
    {122, 250, 0, epd213b_init_sequence_full, NULL, epd213b_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP213B_122x250, newer Inky phat 2.13" B/W
    {128, 296, 0, epd293_init_sequence_full, epd293_init_sequence_fast, epd293_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP293_128x296
    {128, 296, 0, epd294_init_sequence_full, NULL, NULL, 0, BBEP_CHIP_SSD16xx}, // EP294_128x296
    {128, 296, 0, epd293_init_sequence_full, epd293_init_sequence_fast, epd295_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP295_128x296
    {152, 296, 0, epd266_init_sequence_full, NULL, epd266_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP266_152x296
    {80, 128, 0, epd102_init_sequence_full, NULL, epd102_init_sequence_part, 0, BBEP_CHIP_UC81xx}, // EP102_80x128
    {176, 264, 0, epd27_init_sequence_full, NULL, epd27_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP27B_176x264
    {128, 296, 0, epd29r_init_sequence_full, NULL, NULL, BBEP_3COLOR, BBEP_CHIP_SSD16xx}, // EP29R_128x296
    {192, 176, 0, epd122_init_sequence_full, epd122_init_sequence_fast, epd122_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP122_192x176
    {152, 152, 0, epd154r_init_sequence_full, NULL, NULL, BBEP_3COLOR, BBEP_CHIP_SSD16xx}, // EP154R_152x152
    {400, 300, 0, epd42r_init_sequence_full, NULL, NULL, BBEP_3COLOR, BBEP_CHIP_SSD16xx}, // EP42R_400x300, Inky wHAT 4.2" 400x300 B/W/R
    {400, 300, 0, epd42r2_init_sequence_full, NULL, NULL, BBEP_RED_SWAPPED | BBEP_3COLOR, BBEP_CHIP_UC81xx}, // EP42R2_400x300
    {240, 416, 0, epd37_init_sequence_full, NULL, epd37_init_sequence_part, 0, BBEP_CHIP_UC81xx}, // EP37_240x416
    {104, 212, 0, epd213_inky_init_sequence_full, NULL, NULL, 0, BBEP_CHIP_UC81xx}, // EP213_104x212, older InkyPHAT black and white
    {800, 480, 0, epd75_init_sequence_full, epd75_init_sequence_fast, epd75_init_sequence_partial, 0, BBEP_CHIP_UC81xx}, // EP75_800x480
    {128, 296, 0, epd29_init_sequence_full, NULL, epd29_init_sequence_part, 0, BBEP_CHIP_UC81xx}, // Badger 2040 
    {122, 250, 1, epd213r_inky_init_sequence_full, NULL, NULL, BBEP_3COLOR, BBEP_CHIP_SSD16xx}, // EP213R_122x250 Inky phat 2.13" B/W/R
    {200, 200, 0, epd154_init_sequence_full, epd154_init_sequence_fast, epd154_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP154_200x200
    {200, 200, 0, epd154_init_sequence_full, epd154_init_sequence_fast, epd154b_init_sequence_part, 0, BBEP_CHIP_SSD16xx}, // EP154B_200x200
    {184, 360, 0, epd266yr_init_sequence_full, NULL, NULL, BBEP_4COLOR, BBEP_CHIP_UC81xx}, // EP266YR_184x360
    {128, 296, 0, epd29yr_init_sequence_full, NULL, NULL, BBEP_4COLOR, BBEP_CHIP_UC81xx}, // EP29YR_129x296
    {168, 384, 0, epd29yrh_init_sequence_full, NULL, NULL, BBEP_4COLOR, BBEP_CHIP_UC81xx}, // EP29YR_168x384
    {648, 480, 0, epd583_init_sequence_full, NULL, epd583_init_sequence_part, 0, BBEP_CHIP_UC81xx}, // EP583_648x480
};
//
// Set the e-paper panel type
// This must be called before any other bb_epaper functions
//
int bbepSetPanelType(BBEPDISP *pBBEP, int iPanel)
{
    if (pBBEP == NULL || iPanel <= EP_PANEL_UNDEFINED || iPanel >= EP_PANEL_COUNT)
        return BBEP_ERROR_BAD_PARAMETER;
    
    memset(pBBEP, 0, sizeof(BBEPDISP));
    pBBEP->native_width = pBBEP->width = panelDefs[iPanel].width;
    pBBEP->native_height = pBBEP->height = panelDefs[iPanel].height;
    pBBEP->x_offset = panelDefs[iPanel].x_offset;
    pBBEP->chip_type = panelDefs[iPanel].chip_type;
    pBBEP->iFlags = panelDefs[iPanel].flags;
    pBBEP->pInitFull = panelDefs[iPanel].pInitFull;
    pBBEP->pInitFast = panelDefs[iPanel].pInitFast;
    pBBEP->pInitPart = panelDefs[iPanel].pInitPart;
    pBBEP->type = iPanel;
    return BBEP_SUCCESS;
} /* bbepSetPanelType() */

int bbepCreateVirtual(BBEPDISP *pBBEP, int iWidth, int iHeight, int iFlags)
{
    if (pBBEP) {
        memset(pBBEP, 0, sizeof(BBEPDISP));
        pBBEP->native_width = pBBEP->width = iWidth;
        pBBEP->native_height = pBBEP->height = iHeight;
        pBBEP->iFlags = iFlags;
        pBBEP->chip_type = BBEP_CHIP_NONE;
        return BBEP_SUCCESS;
    } else {
        return BBEP_ERROR_BAD_PARAMETER;
    }
}
//
// Wait for the busy status line to show idle
// The polarity of the busy signal is reversed on the UC81xx compared
// to the SSD16xx controllers
//
void bbepWaitBusy(BBEPDISP *pBBEP)
{
    int iTimeout = 0;

    if (!pBBEP) return;
    if (pBBEP->iBUSYPin == 0xff) return;
    uint8_t busy_idle =  (pBBEP->chip_type == BBEP_CHIP_UC81xx) ? HIGH : LOW;
    delay(1); // some panels need a short delay before testing the BUSY line
    while (iTimeout < 5000) {
        if (digitalRead(pBBEP->iBUSYPin) == busy_idle) break;
        delay(1);
    }
} /* bbepWaitBusy() */
//
// Toggle the reset line to wake up the eink from deep sleep
//
void bbepWakeUp(BBEPDISP *pBBEP)
{
    if (!pBBEP) return;
    digitalWrite(pBBEP->iRSTPin, LOW);
    delay(10);
    digitalWrite(pBBEP->iRSTPin, HIGH);
    delay(10);
    bbepWaitBusy(pBBEP);
} /* bbepWakeUp() */
//
// Set the memory window for future writes into panel memory
//
void bbepSetAddrWindow(BBEPDISP *pBBEP, int x, int y, int cx, int cy)
{
    uint8_t uc[12];
    int i, tx, ty;
    
    if (!pBBEP) return;
    if (pBBEP->iFlags & BBEP_4COLOR) return;

    tx = x/8; // round down to next lower byte
    ty = y;
    cx = (cx + 7) & 0xfff8; // make width an even number of bytes
    if (pBBEP->chip_type == BBEP_CHIP_UC81xx) {
        bbepWriteCmd(pBBEP, UC8151_PTIN); // partial in
        bbepWriteCmd(pBBEP, UC8151_PTL); // partial window
        i = 0;
        tx *= 8;
        if (pBBEP->native_width >= 256) { // need 2 bytes per x
            uc[i++] = (uint8_t)(tx>>8); // start x
            uc[i++] = (uint8_t)tx;
            uc[i++] = (uint8_t)((tx+cx-1)>>8); // end x
            uc[i++] = (uint8_t)((tx+cx-1) | 7);
        } else {
            uc[i++] = tx; // start x
            uc[i++] = (tx+cx-1) | 7; // end x
        }
        if (pBBEP->native_height >= 256) {
            uc[i++] = (uint8_t)(ty>>8); // start y
            uc[i++] = (uint8_t)ty;
            uc[i++] = (uint8_t)((ty+cy-1)>>8); // end y
            uc[i++] = (uint8_t)(ty+cy-1);
        } else {
            uc[i++] = (uint8_t)ty;
            uc[i++] = (uint8_t)(ty+cy-1);
        }
        uc[i++] = 1; // refresh whole screen (0=refresh partial window only)
        bbepWriteData(pBBEP, uc, i);
        //       EPDWriteCmd(UC8151_PTOU); // partial out
    } else { // SSD16xx
        //        bbepCMD2(pBBEP, SSD1608_DATA_MODE, 0x3);
        bbepWriteCmd(pBBEP, SSD1608_SET_RAMXPOS);
        tx += pBBEP->x_offset;
        uc[0] = tx; // start x (byte boundary)
        uc[1] = tx+((cx-1)>>3); // end x
        bbepWriteData(pBBEP, uc, 2);
        
        bbepWriteCmd(pBBEP, SSD1608_SET_RAMYPOS);
        uc[0] = (uint8_t)ty; // start y
        uc[1] = (uint8_t)(ty>>8);
        uc[2] = (uint8_t)(ty+cy-1); // end y
        uc[3] = (uint8_t)((ty+cy-1)>>8);
        bbepWriteData(pBBEP, uc, 4);
        
        // set ram counter to start of this region
        bbepCMD2(pBBEP, SSD1608_SET_RAMXCOUNT, tx);
        uc[0] = ty;
        uc[1] = (ty>>8);
        bbepWriteCmd(pBBEP, SSD1608_SET_RAMYCOUNT);
        bbepWriteData(pBBEP, uc, 2);
        //        bbepCMD2(pBBEP, SSD1608_DATA_MODE, 0x3);
    }
} /* bbepSetAddrWindow() */
//
// Put the eink into light or deep sleep
//
void bbepSleep(BBEPDISP *pBBEP, int bDeep)
{
    if (!pBBEP) return;
    if (pBBEP->chip_type == BBEP_CHIP_UC81xx) {
        bbepCMD2(pBBEP, UC8151_CDI, 0x17); // border floating
        bbepWriteCmd(pBBEP, UC8151_POFF); // power off
        bbepWaitBusy(pBBEP);
        if (bDeep) {
            bbepCMD2(pBBEP, UC8151_DSLP, 0xa5); // deep sleep
        }
    } else {
        bbepCMD2(pBBEP, SSD1608_DEEP_SLEEP, 0x01); // deep sleep mode 1 keeps RAM,only uses about 1uA
    }
    pBBEP->is_awake = 0;
} /* bbepSleep() */

void bbepStartWrite(BBEPDISP *pBBEP, int iPlane)
{
    uint16_t u8Cmd; // AVR crashes w/odd number of bytes for stack vars
    
    if (!pBBEP) return;
    if (pBBEP->chip_type == BBEP_CHIP_UC81xx) {
        if (pBBEP->iFlags & BBEP_RED_SWAPPED) {
            if (iPlane == PLANE_0)
                u8Cmd = UC8151_DTM1;
            else
                u8Cmd = UC8151_DTM2;
        } else {
            if (iPlane == PLANE_0)
                u8Cmd = UC8151_DTM2;
            else
                u8Cmd = UC8151_DTM1;
        }
    } else { // SSD16xx
        if (iPlane == PLANE_0)
            u8Cmd = SSD1608_WRITE_RAM;
        else
            u8Cmd = SSD1608_WRITE_ALTRAM;
    }
    bbepWriteCmd(pBBEP, (uint8_t)u8Cmd);
} /* bbepStartWrite() */

//
// More efficient means of sending commands, data and busy-pauses
//
void bbepSendCMDSequence(BBEPDISP *pBBEP, const uint8_t *pSeq)
{
    int iLen;
    uint8_t *s;
    
    if (pBBEP == NULL || pSeq == NULL) return;
    
    s = (uint8_t *)pSeq;
    while (s[0] != 0) { // A 0 length terminates the list
        iLen = *s++;
        if (iLen == BUSY_WAIT) {
            bbepWaitBusy(pBBEP);
        } else {
            bbepWriteCmd(pBBEP, s[0]);
            s++;
            if (iLen > 1) {
                bbepWriteData(pBBEP, s, iLen-1);
                s += (iLen-1);
            }
        }
    } // while more commands to send
} /* bbepSendCMDSequence() */

//
// Tests the BUSY line to see if you're connected to a
// SSD16xx or UC81xx panel
//
int bbepTestPanelType(BBEPDISP *pBBEP)
{
    if (!pBBEP) return BBEP_CHIP_NOT_DEFINED;
    digitalWrite(pBBEP->iRSTPin, LOW);
    delay(40);
    digitalWrite(pBBEP->iRSTPin, HIGH);
    delay(50);
    if (digitalRead(pBBEP->iBUSYPin))
        return BBEP_CHIP_UC81xx; // high state = UltraChip ready
    else
        return BBEP_CHIP_SSD16xx; // low state = Solomon ready
} /* bbepTestPanelType() */
//
// Fill the display with a color or byte pattern
// e.g. all black (0x00) or all white (0xff)
// if there is no backing buffer, write directly to
// the EPD's framebuffer
//
void bbepFill(BBEPDISP *pBBEP, unsigned char ucData, int iPlane)
{
    uint8_t uc1=0, uc2=0;
    int y, iSize, iPitch;
    uint8_t ucCMD1, ucCMD2;
    
    if (pBBEP == NULL) return;
    
    pBBEP->iCursorX = pBBEP->iCursorY = 0;
    iPitch = ((pBBEP->native_width+7)/8);
    iSize = pBBEP->native_height * iPitch;
    if (pBBEP->iFlags & BBEP_3COLOR) {
        if (ucData == BBEP_WHITE) {
            uc1 = 0xff; uc2 = 0x00; // red plane has priority
        } else if (ucData == BBEP_BLACK) {
            uc1 = 0x00; uc2 = 0x00;
        } else if (ucData == BBEP_RED) {
            uc1 = 0x00; uc2 = 0xff;
        }
    } else if (pBBEP->iFlags & BBEP_4COLOR) {
        iPitch = ((pBBEP->native_width+3)/4);
        iSize = pBBEP->native_height * iPitch;
        iPlane = PLANE_0; // only 1 2-bit memory plane
        uc1 = ucData | (ucData << 2);
        uc1 |= (uc1 << 4); // set color in all 4 pixels of the byte
    } else { // B/W
        if (ucData == BBEP_WHITE) {
            uc1 = uc2 = 0xff;
	    } else {
            uc1 = uc2 = 0x00;
	    }
    }
    if (pBBEP->ucScreen) { // there's a local framebuffer, use it
        if ((pBBEP->iFlags & BBEP_3COLOR) || iPlane == PLANE_BOTH) {
            memset(pBBEP->ucScreen, uc1, iSize);
            memset(&pBBEP->ucScreen[iSize], uc2, iSize);
        } else if (iPlane == PLANE_DUPLICATE) {
            memset(pBBEP->ucScreen, uc1, iSize);
            memset(&pBBEP->ucScreen[iSize], uc1, iSize);
        } else if (iPlane == PLANE_0) {
            memset(pBBEP->ucScreen, uc1, iSize);
        } else if (iPlane == PLANE_1) {
            memset(&pBBEP->ucScreen[iSize], uc2, iSize);
        }
    } else { // write directly to the EPD's framebuffer
        if (pBBEP->iFlags & BBEP_3COLOR) {
            if (ucData == BBEP_WHITE) {
                uc1 = 0xff; uc2 = 0x00; // red plane has priority
            } else if (ucData == BBEP_BLACK) {
                uc1 = 0x00; uc2 = 0x00;
            } else if (ucData == BBEP_RED) {
                uc1 = 0x00; uc2 = 0xff;
            }
        } else if (!(pBBEP->iFlags & BBEP_4COLOR)) { // for B/W, both planes get the same data
            if (ucData == BBEP_WHITE) ucData = 0xff;
            else if (ucData == BBEP_BLACK) ucData = 0;
            uc1 = uc2 = ucData;
        }
        if (pBBEP->chip_type == BBEP_CHIP_UC81xx) {
            if (pBBEP->iFlags & (BBEP_RED_SWAPPED | BBEP_4COLOR)) {
                ucCMD1 = UC8151_DTM1;
                ucCMD2 = UC8151_DTM2;
            } else {
                ucCMD1 = UC8151_DTM2;
                ucCMD2 = UC8151_DTM1;
            }
        } else {
            ucCMD1 = SSD1608_WRITE_RAM;
            ucCMD2 = SSD1608_WRITE_ALTRAM;
        }
        // Write one or both memory planes to the EPD
        if (iPlane == PLANE_0 || iPlane == PLANE_DUPLICATE) { // write to first plane
            bbepSetAddrWindow(pBBEP, 0,0, pBBEP->native_width, pBBEP->native_height);
            bbepWriteCmd(pBBEP, ucCMD1);
            for (y=0; y<pBBEP->native_height; y++) {
                memset(u8Cache, uc1, iPitch); // the data is overwritten after each write
                bbepWriteData(pBBEP, u8Cache, iPitch);
            } // for y
        }
        if (iPlane == PLANE_1 || iPlane == PLANE_DUPLICATE) { // write to first plane
            bbepSetAddrWindow(pBBEP, 0,0, pBBEP->native_width, pBBEP->native_height);
            bbepWriteCmd(pBBEP, ucCMD2);
            for (y=0; y<pBBEP->native_height; y++) {
                memset(u8Cache, uc2, iPitch); // the data is overwritten after each write
                bbepWriteData(pBBEP, u8Cache, iPitch);
            } // for y
        }
    }
} /* bbepFill() */

int bbepRefresh(BBEPDISP *pBBEP, int iMode)
{
    if (iMode != REFRESH_FULL && iMode != REFRESH_FAST && iMode != REFRESH_PARTIAL)
        return BBEP_ERROR_BAD_PARAMETER;
    
    switch (iMode) {
        case REFRESH_FULL:
            bbepSendCMDSequence(pBBEP, pBBEP->pInitFull);
            break;
        case REFRESH_FAST:
            if (!pBBEP->pInitFast)
                return BBEP_ERROR_BAD_PARAMETER;
            bbepSendCMDSequence(pBBEP, pBBEP->pInitFast);
            break;
        case REFRESH_PARTIAL:
            if (!pBBEP->pInitPart)
                return BBEP_ERROR_BAD_PARAMETER;
            bbepSendCMDSequence(pBBEP, pBBEP->pInitPart);
            break;
        default:
            return BBEP_ERROR_BAD_PARAMETER;
    } // switch on mode
    if (pBBEP->chip_type == BBEP_CHIP_UC81xx) {
        if (pBBEP->iFlags & BBEP_4COLOR) {
            bbepCMD2(pBBEP, 0x12, 0x00);
        } else {
            bbepWriteCmd(pBBEP, UC8151_PTOU); // partial out (update the entire panel, not just the last memory window)
            bbepWriteCmd(pBBEP, UC8151_DRF);
        }
    } else {
        const uint8_t u8CMD[4] = {0xf7, 0xc7, 0xff, 0}; // normal, fast, partial
        if (pBBEP->iFlags & BBEP_3COLOR) {
            iMode = REFRESH_FAST;
        } // 3-color = 0xc7
        bbepCMD2(pBBEP, SSD1608_DISP_CTRL2, u8CMD[iMode]);
        bbepWriteCmd(pBBEP, SSD1608_MASTER_ACTIVATE); // refresh
    }
    return BBEP_SUCCESS;
} /* bbepRefresh() */

void bbepSetRotation(BBEPDISP *pBBEP, int iRotation)
{
    pBBEP->iScreenOffset = 0;
    pBBEP->iOrientation = iRotation;
    
    switch (iRotation) {
        default: return;
        case 0:
            pBBEP->width = pBBEP->native_width;
            pBBEP->height = pBBEP->native_height;
            break;
        case 90:
            pBBEP->width = pBBEP->native_height;
            pBBEP->height = pBBEP->native_width;
            break;
        case 180:
            pBBEP->width = pBBEP->native_width;
            pBBEP->height = pBBEP->native_height;
            break;
        case 270:
            pBBEP->width = pBBEP->native_height;
            pBBEP->height = pBBEP->native_width;
            break;
    }
} /* bbepSetRotation() */

void bbepWriteImage2bpp(BBEPDISP *pBBEP, uint8_t ucCMD)
{
int tx, ty;
uint8_t *s, *d, uc, uc1, ucMask;
uint8_t *pBuffer;

    pBuffer = pBBEP->ucScreen;
    if (ucCMD) {
        bbepWriteCmd(pBBEP, ucCMD); // start write
    }
    // Convert the bit direction and write the data to the EPD
    if (pBBEP->iOrientation == 180) {
        for (ty=pBBEP->height-1; ty>=0; ty--) {
            d = u8Cache;
            s = &pBuffer[ty * pBBEP->width/4];
            for (tx=pBBEP->width-4; tx>=0; tx-=4) {
                uc = 0;
                ucMask = 0x03;
                uc1 = s[tx>>2];
                for (int pix=0; pix<8; pix +=2) { // reverse the direction of the 4 pixels
                    uc <<= 2; // shift down 1 pixel
                    uc |= ((uc1 & ucMask) >> pix);
                    ucMask <<= 2;
                }
                *d++ = uc; // store 4 pixels
            } // for tx
            bbepWriteData(pBBEP, u8Cache, pBBEP->width/4);
        } // for ty
    } else if (pBBEP->iOrientation == 0) {
        s = pBBEP->ucScreen;
        for (ty=0; ty<pBBEP->height; ty++) {
            bbepWriteData(pBBEP, s, pBBEP->width/4);
            s += pBBEP->width/4; // 4 pixels per byte
        } // for ty
    } else if (pBBEP->iOrientation == 90) {
        for (tx=0; tx<pBBEP->width; tx++) {
            d = u8Cache;
            for (ty=pBBEP->height-1; ty > 0; ty-=4) {
                s = &pBuffer[(tx>>2) + (ty * (pBBEP->width/4))];
                uc = 0;
                ucMask = 0xc0 >> ((tx & 3) * 2);
                for (int pix=0; pix<4; pix++) {
                    uc <<= 2; // shift down 1 pixel
                    uc |= ((s[0] & ucMask) >> ((3-(tx&3))*2)); // inverted plane 0
                    s -= (pBBEP->width/4);
                }
                *d++ = uc; // store 4 pixels
            } // for ty
            bbepWriteData(pBBEP, u8Cache, pBBEP->height/4);
        } // for tx
    } else if (pBBEP->iOrientation == 270) {
        for (tx=pBBEP->width-1; tx>=0; tx--) {
            d = u8Cache;
            for (ty=3; ty<pBBEP->height; ty+=4) {
                s = &pBuffer[(tx>>2) + (ty * pBBEP->width/4)];
                ucMask = 0xc0 >> ((tx & 3) * 2);
                uc = 0;
                for (int pix=0; pix<4; pix++) {
                    uc >>= 2;
                    uc |= ((s[0] & ucMask) << ((tx&3)*2)); // inverted plane 0
                    s -= (pBBEP->width/4);
                } // for pix
                *d++ = uc; // store 2 pixels
            } // for ty
            bbepWriteData(pBBEP, u8Cache, pBBEP->height/4);
        } // for x
  } // 270
} /* bbepWriteImage2bpp() */

//
// Write Image data (entire plane) from RAM to the e-paper
// Rotate the pixels if necessary
//
static void bbepWriteImage(BBEPDISP *pBBEP, uint8_t ucCMD, uint8_t *pBuffer, int bInvert)
{
    int tx, ty;
    uint8_t *s, *d, ucSrcMask, ucDstMask, uc;
    uint8_t ucInvert = 0;
    int iPitch;
    
    iPitch = (pBBEP->width + 7) >> 3;
    if (bInvert) {
        ucInvert = 0xff; // red logic is inverted
    }
    if (ucCMD) {
        bbepWriteCmd(pBBEP, ucCMD); // start write
    }
    // Convert the bit direction and write the data to the EPD
    switch (pBBEP->iOrientation) {
        case 0:
            for (ty=0; ty<pBBEP->native_height; ty++) {
                d = u8Cache;
                s = &pBuffer[ty * iPitch];
                memcpy(d, s, iPitch);
                if (ucInvert) InvertBytes(d, iPitch);
                bbepWriteData(pBBEP, u8Cache, iPitch);
            } // for ty
            break;
        case 90:
            for (tx=0; tx<pBBEP->width; tx++) {
                d = u8Cache;
                // need to pick up and reassemble every pixel
                ucDstMask = 0x80;
                uc = 0xff;
                ucSrcMask = 0x80 >> (tx & 7);
                for (ty=pBBEP->height-1; ty>=0; ty--) {
                    s = &pBuffer[(tx >> 3) + (ty * iPitch)];
                    if ((s[0] & ucSrcMask) == 0) uc &= ~ucDstMask;
                    ucDstMask >>= 1;
                    if (ucDstMask == 0) {
                        *d++ = (uc ^ ucInvert);
                        ucDstMask = 0x80;
                        uc = 0xff;
                    }
                } // for ty
                *d++ = (uc ^ ucInvert); // store final partial byte
                bbepWriteData(pBBEP, u8Cache, (pBBEP->native_width+7)/8);
            } // for tx
            break;
        case 180:
            for (ty=pBBEP->native_height-1; ty>=0; ty--) {
                d = u8Cache;
                s = &pBuffer[ty * iPitch];
                for (tx=iPitch-1; tx>=0; tx--) {
                    *d++ = (ucMirror[s[tx]] ^ ucInvert);
                } // for tx
                bbepWriteData(pBBEP, u8Cache, iPitch);
            } // for ty
            break;
        case 270:
            for (tx=pBBEP->width-1; tx>=0; tx--) {
                d = u8Cache;
                // reassemble every pixel
                ucDstMask = 0x80;
                uc = 0xff;
                ucSrcMask = 0x80 >> (tx & 7);
                for (ty=0; ty<pBBEP->height; ty++) {
                    s = &pBuffer[(tx>>3) + (ty * iPitch)];
                    if ((s[0] & ucSrcMask) == 0) uc &= ~ucDstMask;
                    ucDstMask >>= 1;
                    if (ucDstMask == 0) {
                        *d++ = (uc ^ ucInvert);
                        ucDstMask = 0x80;
                        uc = 0xff;
                    }
                } // for ty
                *d++ = (uc ^ ucInvert); // store final partial byte
                bbepWriteData(pBBEP, u8Cache, (pBBEP->native_width+7)/8);
            } // for x
            break;
    } // switch on orientation
} /* bbepWriteImage() */
//
// Write the local copy of the memory plane(s) to the eink's internal framebuffer
//
int bbepWritePlane(BBEPDISP *pBBEP, int iPlane)
{
    uint8_t ucCMD1, ucCMD2;
    int iOffset;
    
    if (pBBEP == NULL || pBBEP->ucScreen == NULL || iPlane < PLANE_0 || iPlane > PLANE_DUPLICATE)
        return BBEP_ERROR_BAD_PARAMETER;
    bbepSetAddrWindow(pBBEP, 0,0, pBBEP->native_width, pBBEP->native_height);

    if (pBBEP->iFlags & BBEP_4COLOR) { // 4-color only has 1 way to go
        bbepWriteImage2bpp(pBBEP, 0x10);
        return BBEP_SUCCESS;
    }
    if (pBBEP->chip_type == BBEP_CHIP_UC81xx) {
        if (pBBEP->iFlags & BBEP_RED_SWAPPED) {
            ucCMD1 = UC8151_DTM1;
            ucCMD2 = UC8151_DTM2;
        } else {
            ucCMD1 = UC8151_DTM2;
            ucCMD2 = UC8151_DTM1;
        }
    } else {
        ucCMD1 = SSD1608_WRITE_RAM;
        ucCMD2 = SSD1608_WRITE_ALTRAM;
    }
    iOffset = ((pBBEP->native_width+7)>>3) * pBBEP->native_height;
    if (pBBEP->iFlags & BBEP_3COLOR && iPlane == PLANE_DUPLICATE) {
        iPlane = PLANE_BOTH;
    } 
    switch (iPlane) {
        case PLANE_0:
            bbepWriteImage(pBBEP, ucCMD1, pBBEP->ucScreen, 0);
            break;
        case PLANE_1:
            bbepWriteImage(pBBEP, ucCMD2, &pBBEP->ucScreen[iOffset], 0);
            break;
        case PLANE_BOTH:
            bbepWriteImage(pBBEP, ucCMD1, pBBEP->ucScreen, 0);
            bbepWriteImage(pBBEP, ucCMD2, &pBBEP->ucScreen[iOffset], 0);
            break;
        case PLANE_DUPLICATE:
            bbepWriteImage(pBBEP, ucCMD1, pBBEP->ucScreen, 0);
            bbepWriteImage(pBBEP, ucCMD2, pBBEP->ucScreen, 0);
            break;
    }
    return BBEP_SUCCESS;
} /* bbepWritePlane() */

#endif // __BB_EP__
