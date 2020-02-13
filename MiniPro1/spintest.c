#include "elecanisms.h"
#include "ajuart.h"
#include <stdio.h>

// DEFINES
#define ENC_MISO            D1
#define ENC_MOSI            D0
#define ENC_SCK             D2
#define ENC_CSn             D3

#define ENC_MISO_DIR        D1_DIR
#define ENC_MOSI_DIR        D0_DIR
#define ENC_SCK_DIR         D2_DIR
#define ENC_CSn_DIR         D3_DIR

#define ENC_MISO_RP         D1_RP
#define ENC_MOSI_RP         D0_RP
#define ENC_SCK_RP          D2_RP

uint16_t even_parity(uint16_t v) {
    v ^= v >> 8;
    v ^= v >> 4;
    v ^= v >> 2;
    v ^= v >> 1;
    return v & 1;
}

uint16_t enc_readReg(uint16_t address) {
    WORD cmd;// result;
    uint16_t temp;
      // find the angle reg in the datasheet
    cmd.w = 0x4000 | address;         // set 2nd MSB to 1 for a read
    cmd.w |= even_parity(cmd.w) << 15;

    ENC_CSn = 0;

    SPI2BUF = (uint16_t)cmd.b[1];
    while (SPI2STATbits.SPIRBF == 0) {}
    temp = SPI2BUF;

    SPI2BUF = (uint16_t)cmd.b[0];
    while (SPI2STATbits.SPIRBF == 0) {}
    temp = SPI2BUF;

    ENC_CSn = 1;

    __asm__("nop");     // p.12 of the AS5048 datasheet specifies a minimum
    __asm__("nop");     //   high time of CSn between transmission of 350ns
    __asm__("nop");     //   which is 5.6 Tcy, so do nothing for 6 Tcy.
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");

    ENC_CSn = 0;

    SPI2BUF = 0;
    while (SPI2STATbits.SPIRBF == 0) {}
    uint8_t reshold1 = (uint8_t)SPI2BUF;
    // result.b[1] = (uint8_t)SPI2BUF;

    SPI2BUF = 0;
    while (SPI2STATbits.SPIRBF == 0) {}
    uint8_t reshold = (uint8_t)SPI2BUF;
    // result.b[0] = (uint8_t)SPI2BUF;
    uint16_t result = reshold1 + (reshold << 8);

    ENC_CSn = 1;

    return result;
}

int16_t main(void) {
  uint8_t *RPOR, *RPINR;
  // setup stage

  init_elecanisms();
  init_ajuart();

  // Configure Timer1 period to be 20 ms
  T1CON = 0x0010;
  PR1 = 0x9C3F;

  // Start Timer1
  TMR1 = 0;
  IFS0bits.T1IF = 0;
  T1CONbits.TON = 1;
  // Commatecation setup
  // Configure encoder pins and connect them to SPI2
  ENC_CSn_DIR = OUT; ENC_CSn = 1;
  ENC_SCK_DIR = OUT; ENC_SCK = 0;
  ENC_MOSI_DIR = OUT; ENC_MOSI = 0;
  ENC_MISO_DIR = IN;

  RPOR = (uint8_t *)&RPOR0;
  RPINR = (uint8_t *)&RPINR0;

  __builtin_write_OSCCONL(OSCCON & 0xBF);
  RPINR[MISO2_RP] = ENC_MISO_RP;
  RPOR[ENC_MOSI_RP] = MOSI2_RP;
  RPOR[ENC_SCK_RP] = SCK2OUT_RP;
  __builtin_write_OSCCONL(OSCCON | 0x40);

  SPI2CON1 = 0x003B;              // SPI2 mode = 1, SCK freq = 8 MHz
  SPI2CON2 = 0;
  SPI2STAT = 0x8000;

  uint16_t angle_reg = 0x3FFF;
  uint16_t angle;

  D7_DIR = OUT;
  D6_DIR = OUT;

  D6 = 1;
  D7 = 0;

  D5_DIR = OUT;
  D5 = 1; 

  // loop  start here
  while (1) {

    if (IFS0bits.T1IF == 1) {
        IFS0bits.T1IF = 0;      // Lower Timer1 interrupt flag
        angle = enc_readReg(angle_reg);
        printf("angles: %d\r\n",angle);

    }
  }
}
