//
// I/O wrapper functions for the S2/S3 RISC-V ULP
//
#ifndef __ULP_IO__
#define __ULP_IO__

#define INPUT 0
#define INPUT_PULLUP 1
#define OUTPUT 2
#define DISABLED 3

#define PROGMEM
#define memcpy_P memcpy
#define pgm_read_byte(a) *(uint8_t *)a

#define HIGH 1
#define LOW 0
#define PIN_SDA 6
#define PIN_SCL 7

// Pinout for my custom ESP32-S3 circuit
//#define PIN_BUSY 12
//#define PIN_RST 7
//#define PIN_DC 10
//#define PIN_CS 11
//#define PIN_SCK 1
//#define PIN_MOSI 3

// CPU cycles per bit
#define I2C_DELAY 40

void delayMicroseconds(long l)
{
    l *= 40;
    while (l) {
        __asm__ __volatile__ ("nop");
        l--;
    }
}
void delay(long l)
{   
    delayMicroseconds(l*1000);
}

void digitalWrite(int iPin, int iState) {
   ulp_riscv_gpio_output_level(iPin, iState);
}
void pinMode(int iPin, int iMode)
{
    ulp_riscv_gpio_init(iPin);
    if (iMode == INPUT || iMode == INPUT_PULLUP) {
        ulp_riscv_gpio_output_disable(iPin);
        ulp_riscv_gpio_input_enable(iPin);
        if (iMode == INPUT_PULLUP) {
            ulp_riscv_gpio_pullup(iPin);
        } else {
            ulp_riscv_gpio_pullup_disable(iPin);
        }
    } else if (iMode == DISABLED) {
        ulp_riscv_gpio_deinit(iPin);
    } else { // OUTPUT
        ulp_riscv_gpio_input_disable(iPin);
        ulp_riscv_gpio_set_output_mode(iPin, RTCIO_MODE_OUTPUT);
        ulp_riscv_gpio_output_enable(iPin);
    }
} /* pinMode() */

int digitalRead(int iPin)
{
  return (int)ulp_riscv_gpio_get_level(iPin);
} /* digitalRead() */
//
// De-initialize the GPIO pins
//
void bbepDeInitIO(BBEPDISP *pBBEP)
{
    pinMode(pBBEP->iDCPin, DISABLED);
    pinMode(pBBEP->iCSPin, DISABLED);
    pinMode(pBBEP->iRSTPin, DISABLED);
    pinMode(pBBEP->iBUSYPin, DISABLED);
    pinMode(pBBEP->iMOSIPin, DISABLED);
    pinMode(pBBEP->iCLKPin, DISABLED);
} /* bbepDeInitIO() */
//
// Initialize the GPIO pins and SPI for use by bb_epaper
//
void bbepInitIO(BBEPDISP *pBBEP, uint8_t u8DC, uint8_t u8RST, uint8_t u8BUSY, uint8_t u8CS, uint8_t u8MOSI, uint8_t u8SCK, uint32_t u32Speed)
{
    (void)u32Speed; // irrelevant for the ULP
    pBBEP->iDCPin = u8DC;
    pBBEP->iCSPin = u8CS;
    pBBEP->iMOSIPin = u8MOSI;
    pBBEP->iCLKPin = u8SCK;
    pBBEP->iRSTPin = u8RST;
    pBBEP->iBUSYPin = u8BUSY;

    pinMode(pBBEP->iDCPin, OUTPUT);
    pinMode(pBBEP->iRSTPin, OUTPUT);
    digitalWrite(pBBEP->iRSTPin, LOW);
    delay(100);
    digitalWrite(pBBEP->iRSTPin, HIGH);
    delay(100);
    if (pBBEP->iBUSYPin != 0xff) {
        pinMode(pBBEP->iBUSYPin, INPUT);
    }
    pinMode(pBBEP->iCSPin, OUTPUT);
    pinMode(pBBEP->iMOSIPin, OUTPUT);
    pinMode(pBBEP->iCLKPin, OUTPUT);
    digitalWrite(pBBEP->iCSPin, HIGH); // we have to manually control the CS pin
} /* bbepInitIO() */

//
// Bit Bang SPI
//
void SPIWriteByte(BBEPDISP *pBBEP, uint8_t uc)
{
uint8_t u8MOSI = pBBEP->iMOSIPin;
uint8_t u8SCK = pBBEP->iCLKPin;

    if (uc == 0 || uc == 0xff) { // special case
        ulp_riscv_gpio_output_level(u8MOSI, (uc & 1));
        for (int i=0; i<8; i++) {
            ulp_riscv_gpio_output_level(u8SCK, 1); // no delays needed since it runs at a slow clock
            ulp_riscv_gpio_output_level(u8SCK, 0);
        }
    } else {
        for (int i=0; i<8; i++) {
            ulp_riscv_gpio_output_level(u8MOSI, (uc & 0x80)); // msb first
            ulp_riscv_gpio_output_level(u8SCK, 1); // no delays needed since it runs at a slow clock
            uc <<= 1;
            ulp_riscv_gpio_output_level(u8SCK, 0);
        }
    }
} /* SPIWriteByte() */

void bbepWriteCmd(BBEPDISP *pBBEP, uint8_t ucCMD)
{   
    digitalWrite(pBBEP->iDCPin, LOW);
    digitalWrite(pBBEP->iCSPin, LOW);
    SPIWriteByte(pBBEP, ucCMD);
    digitalWrite(pBBEP->iDCPin, HIGH);
    digitalWrite(pBBEP->iCSPin, HIGH);
} /* bbepWriteCmd() */

void bbepWriteData(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
    digitalWrite(pBBEP->iCSPin, LOW);
    for (int i=0; i<iLen; i++) {
        SPIWriteByte(pBBEP, pData[i]);
    }
    digitalWrite(pBBEP->iCSPin, HIGH);
}

void bbepCMD2(BBEPDISP *pBBEP, uint8_t cmd, uint8_t param)
{  
        bbepWriteCmd(pBBEP, cmd);
        bbepWriteData(pBBEP, &param, 1);
} /* bbepCMD2() */

void mymemcpy(void *d, void *s, size_t iLen)
{
    uint8_t *d8 = (uint8_t *)d;
    uint8_t *s8 = (uint8_t *)s;
    while (iLen) {
        *d8++ = *s8++;
        iLen--;
    }
} /* mymemcpy() */

void mymemset(void *d, uint8_t u8, int len)
{
    uint8_t *d8;
    if (((intptr_t)d & 3) == 0) { // dword aligned
        uint32_t u32, *d32 = (uint32_t *)d;
        u32 = u8 | (u8 << 8);
        u32 |= (u32 << 16);
        while (len >= 4) {
            *d32++ = u32;
            len -= 4;
        }
        d = d32;
    }
    d8 = (uint8_t *)d;
    while (len) {
        *d8++ = u8;
        len--;
    }
} /* mymemset() */
//
// Convert a number into a fixed length, zero-terminated string
//
void i2strf(char *pDest, int iVal, int iDigits)
{
        char *d = pDest;
        int i;
        pDest[iDigits] = 0;
        while (iDigits) {
                iDigits--;
            i = iVal % 10;
            d[iDigits] = '0' + (char)i;
            iVal /= 10;
        }
} /* i2strf() */
//
// Convert a number into a zero-terminated string
//
int i2str(char *pDest, int iVal)
{
        char *d = pDest;
        int i, iPlaceVal = 10000;
        int iDigits = 0;

        if (iVal < 0) {
                iDigits++;
                *d++ = '-';
                iVal = -iVal;
        }
        while (iPlaceVal) {
                if (iVal >= iPlaceVal) {
                        i = iVal / iPlaceVal;
                        *d++ = '0' + (char)i;
                        iVal -= (i*iPlaceVal);
                        iDigits++;
                } else if (iDigits != 0) {
                        *d++ = '0'; // non-zeros were already displayed
                }
                iPlaceVal /= 10;
        }
        if (d == pDest) // must be zero
                *d++ = '0';
        *d++ = 0; // terminator
        return (int)(d - pDest - 1); // string length
} /* i2str() */

#endif // __ULP_IO__
