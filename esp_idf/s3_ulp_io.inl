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
#define HIGH 1
#define LOW 0
#define PIN_SDA 6
#define PIN_SCL 7

// Pinout for the LilyGo mini-epaper S3
#define PIN_BUSY 10
#define PIN_RST 11
#define PIN_DC 12
#define PIN_CS 13
#define PIN_SCK 14
#define PIN_MOSI 15

// Pinout for my custom ESP32-S3 circuit
//#define PIN_BUSY 12
//#define PIN_RST 7
//#define PIN_DC 10
//#define PIN_CS 11
//#define PIN_SCK 1
//#define PIN_MOSI 3

// CPU cycles per bit
#define I2C_DELAY 40

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
// Bit Bang SPI
//
void SPIWriteByte(uint8_t uc)
{
    if (uc == 0 || uc == 0xff) { // special case
        ulp_riscv_gpio_output_level(PIN_MOSI, (uc & 1));
        for (int i=0; i<8; i++) {
            ulp_riscv_gpio_output_level(PIN_SCK, 1); // no delays needed since it runs at a slow clock
            ulp_riscv_gpio_output_level(PIN_SCK, 0);
        }
    } else {
        for (int i=0; i<8; i++) {
            ulp_riscv_gpio_output_level(PIN_MOSI, (uc & 0x80)); // msb first
            ulp_riscv_gpio_output_level(PIN_SCK, 1); // no delays needed since it runs at a slow clock
            uc <<= 1;
            ulp_riscv_gpio_output_level(PIN_SCK, 0);
        }
    }
} /* SPIWriteByte() */

void delayMicroseconds(long l)
{
    l *= 40;
    while (l) {
        __asm__ __volatile__ ("nop");
        l--;
    }
}
void _delay(long l)
{
    delayMicroseconds(l*1000);
}
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
//
// Need to bit-bang I2C
//
uint8_t SDA_READ(void)
{
    return ulp_riscv_gpio_get_level(PIN_SDA);
}
void SDA_HIGH(void)
{
    ulp_riscv_gpio_output_disable(PIN_SDA);
    ulp_riscv_gpio_input_enable(PIN_SDA);
}
void SDA_LOW(void)
{
    ulp_riscv_gpio_input_disable(PIN_SDA);
    ulp_riscv_gpio_output_enable(PIN_SDA);
    ulp_riscv_gpio_output_level(PIN_SDA, 0);
}
void SCL_HIGH(void)
{
    ulp_riscv_gpio_output_disable(PIN_SCL);
    ulp_riscv_gpio_input_enable(PIN_SCL);
}
void SCL_LOW(void)
{
    ulp_riscv_gpio_input_disable(PIN_SCL);
    ulp_riscv_gpio_output_enable(PIN_SCL);
    ulp_riscv_gpio_output_level(PIN_SDA, 0);
}
// Transmit a byte and read the ack bit
// if we get a NACK (negative acknowledge) return 0
// otherwise return 1 for success
//

int i2cByteOut(uint8_t b)
{
uint8_t i, ack;

for (i=0; i<8; i++) {
//    my_sleep_us(iDelay);
    if (b & 0x80)
      SDA_HIGH(); // set data line to 1
    else
      SDA_LOW(); // set data line to 0
    b <<= 1;
//    my_sleep_us(iDelay);
    SCL_HIGH(); // clock high (slave latches data)
    ulp_riscv_delay_cycles(I2C_DELAY);
    SCL_LOW(); // clock low
    ulp_riscv_delay_cycles(I2C_DELAY);
} // for i
//my_sleep_us(iDelay);
// read ack bit
SDA_HIGH(); // set data line for reading
//my_sleep_us(iDelay);
SCL_HIGH(); // clock line high
ulp_riscv_delay_cycles(I2C_DELAY); // DEBUG - delay/2
ack = SDA_READ();
//my_sleep_us(iDelay);
SCL_LOW(); // clock low
ulp_riscv_delay_cycles(I2C_DELAY); // DEBUG - delay/2
SDA_LOW(); // data low
return (ack == 0); // a low ACK bit means success
} /* i2cByteOut() */
//
// Receive a byte and read the ack bit
// if we get a NACK (negative acknowledge) return 0
// otherwise return 1 for success
//
uint8_t i2cByteIn(uint8_t bLast)
{
uint8_t i;
uint8_t b = 0;

     SDA_HIGH(); // set data line as input
     for (i=0; i<8; i++)
     {
         ulp_riscv_delay_cycles(I2C_DELAY); // wait for data to settle
         SCL_HIGH(); // clock high (slave latches data)
         ulp_riscv_delay_cycles(I2C_DELAY);
         b <<= 1;
         if (SDA_READ() != 0) // read the data bit
           b |= 1; // set data bit
         SCL_LOW(); // clock low
     } // for i
     if (bLast)
        SDA_HIGH(); // last byte sends a NACK
     else
        SDA_LOW();
//     my_sleep_us(iDelay);
     SCL_HIGH(); // clock high
     ulp_riscv_delay_cycles(I2C_DELAY);
     SCL_LOW(); // clock low to send ack
     ulp_riscv_delay_cycles(I2C_DELAY);
//     SDA_HIGH();
     SDA_LOW(); // data low
  return b;
} /* i2cByteIn() */
//
// Send I2C STOP condition
//
void i2cEnd(void)
{
   SDA_LOW(); // data line low
   ulp_riscv_delay_cycles(I2C_DELAY);
   SCL_HIGH(); // clock high
   ulp_riscv_delay_cycles(I2C_DELAY);
   SDA_HIGH(); // data high
   ulp_riscv_delay_cycles(I2C_DELAY);
} /* i2cEnd() */

int i2cBegin(uint8_t addr, uint8_t bRead)
{
   int rc;
//   SCL_HIGH();
//   my_sleep_us(iDelay);
   SDA_LOW(); // data line low first
   ulp_riscv_delay_cycles(I2C_DELAY);
   SCL_LOW(); // then clock line low is a START signal
   addr <<= 1;
   if (bRead)
      addr++; // set read bit
   rc = i2cByteOut(addr); // send the slave address and R/W bit
   return rc;
} /* i2cBegin() */
void I2CWrite(uint8_t addr, uint8_t *pData, int iLen)
{
uint8_t b;
int rc;

   i2cBegin(addr, 0);
   rc = 1;
   while (iLen && rc == 1)
   {
      b = *pData++;
      rc = i2cByteOut(b);
      if (rc == 1) // success
      {
         iLen--;
      }
   } // for each byte
   i2cEnd();
//return (rc == 1) ? (iOldLen - iLen) : 0; // 0 indicates bad ack from sending a byte
} /* I2CWrite() */
int I2CRead(uint8_t addr, uint8_t *pData, int iLen)
{
   i2cBegin(addr, 1);
   while (iLen--)
   {
      *pData++ = i2cByteIn(iLen == 0);
   } // for each byte
   i2cEnd();
   return 1;
} /* I2CRead() */

int I2CTest(uint8_t addr)
{
int response = 0;

   if (i2cBegin(addr, 0)) // try to write to the given address
   {
      response = 1;
   }
   i2cEnd();
return response;
} /* I2CTest() */

void I2CInit(void)
{
    pinMode(PIN_SDA, INPUT_PULLUP);
    pinMode(PIN_SCL, INPUT_PULLUP);
}
void I2CDeInit(void)
{
    pinMode(PIN_SDA, DISABLED);
    pinMode(PIN_SCL, DISABLED);
}
// 
// Read N bytes starting at a specific I2C internal register
// returns 1 for success, 0 for error
// 
void I2CReadRegister(uint8_t iAddr, uint8_t u8Register, uint8_t *pData, int iLen)
{
  I2CWrite(iAddr, &u8Register, 1);
  I2CRead(iAddr, pData, iLen);
} /* I2CReadRegister() */

#endif // __ULP_IO__
