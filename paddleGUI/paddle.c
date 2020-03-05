
#include "elecanisms.h"
#include "usb.h"
#include "ajuart.h"
#include <stdio.h>
#include <math.h>
// #include <unistd.h>

#define SPRING              0
#define DAMPER              1
#define TEXTURE             2
#define WALL            3
#define READ_SW2            4
#define READ_SW3            5
#define ENC_READ_REG        6
#define SET_DUTY_VAL        7
#define GET_DUTY_VAL        8
#define GET_DUTY_MAX        9


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
int MODE = 2;
int PWMc = 1;
int pos = 0;
int temppos = 0;
int angtemp = 0;
int angle = 0;
int chan = 0;
int tempangle = 0;
// this is code for the angle sencor
uint16_t even_parity(uint16_t v) {
    v ^= v >> 8;
    v ^= v >> 4;
    v ^= v >> 2;
    v ^= v >> 1;
    return v & 1;
}
WORD enc_readReg(WORD address) {
    WORD cmd, result;
    uint16_t temp;

    cmd.w = 0x4000 | address.w;         // set 2nd MSB to 1 for a read
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
    result.b[1] = (uint8_t)SPI2BUF;

    SPI2BUF = 0;
    while (SPI2STATbits.SPIRBF == 0) {}
    result.b[0] = (uint8_t)SPI2BUF;

    ENC_CSn = 1;

    return result;
}
// this is code for the angle sencor END


// USB REQest and mode code here
void vendor_requests(void) {
    WORD temp;
    uint16_t i;
    uint16_t angle_reg = 0x3FFF;
    // uint16_t angle;

    switch (USB_setup.bRequest) {
        case SPRING:   // should keep a change
            LED1 = 1; LED2 = 0; LED3 = 0; MODE = 0; chan = 1;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;
        case DAMPER: // should keep a change
            LED1 = 0; LED2 = 1; LED3 = 0; MODE = 1; chan = 1;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;
        case TEXTURE: // should keep a change
            LED1 = 0; LED2 = 0; LED3 = 1; MODE = 2; chan = 1;
            //D9 = 1;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;              // should addd the 4th button
        case WALL: // should keep a change
            // LED3 = !LED3;
            LED1 = 1; LED2 = 0; LED3 = 1; MODE = 3; chan = 1;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;
        // case READ_SW1:  // should keep a change
        //     BD[EP0IN].address[0] = SW1 ? 1 : 0;
        //     BD[EP0IN].bytecount = 1;
        //     BD[EP0IN].status = UOWN | DTS | DTSEN;
        //     break;
        case READ_SW2:  // could delete
            BD[EP0IN].address[0] = SW2 ? 1 : 0;
            BD[EP0IN].bytecount = 1;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;
        case READ_SW3: // could delete
            BD[EP0IN].address[0] = SW3 ? 1 : 0;
            BD[EP0IN].bytecount = 1;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;
        case ENC_READ_REG: // keep butt change to the angle sencor
            //angle = enc_readReg(angle_reg);
            //temp.w = read_analog(A0_AN);
            temp = enc_readReg(USB_setup.wValue);
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;
            // anglelast = angle;
            angle = ((temp.b[0] + (temp.b[1] << 8)) & 0x3FFF)/100;
            // angle = (temp[0] + 256 * temp[1]) && 0x3FFF;
            printf("angles: %d\r\n",angle);
            BD[EP0IN].status = UOWN | DTS | DTSEN;


            postion();
            modeControl();
            break;
        case SET_DUTY_VAL:
            OC1R = USB_setup.wValue.w;
            BD[EP0IN].bytecount = 0;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;
        case GET_DUTY_VAL:
            temp.w = OC1R;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;
        case GET_DUTY_MAX:
            temp.w = OC1RS;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;
            BD[EP0IN].status = UOWN | DTS | DTSEN;
            break;
        default:
            USB_error_flags |= REQUEST_ERROR;
    }
}
void postion(void){
  printf("angtemp: %d\r\n",angtemp);
  if(angle != angtemp && angle != 0 && angtemp != 0 ){
    int diff = (angle - angtemp);
    pos += diff;
    // printf("%d\r\n",angtemp);
  }
  angtemp = angle;
  printf("postion: %d\r\n",pos);
}
void modeControl(void){
  // printf("mode");
  switch (MODE){
    case SPRING:   // should keep a change
        D10 = 1;
        // //LED1 = 1; LED2 = 0; LED3 = 0; MODE = 0
        // if(angle >= 0 && angle <= 8 ){
        //   D9 = !D9;
        //   printf(" turn" );
        // }
        // D6 = 1;
        // // printf(" spring %d", angle);
        if(chan == 1){
          tempangle = 60;
          // printf("%d\r\n",tempangle);
        }//OC1RS
        if(angle >= 0){
        if(angle >= tempangle+2 || angle <= tempangle-2){
          if(angle-tempangle > 0){
            D10 = 0;
            D9 = 0;
          } else if(angle-tempangle < 0){
            D10 = 0;
            D9 = 1;
          }
        } else {D10 = 1;}}
        chan = 0;
        break;
    case DAMPER: // should keep a change
        D6 = 1;
        // PSEUDOCODE
        if (chan == 1){
          temppos = pos;
        }
        if(pos >= temppos+3 || pos <= temppos-3){
          if(pos-temppos > 0){
            D10 = 0;
            D9 = 0;

            // OC1R = OC1R * (temppos-pos)/ 1000;
            OC1R = (abs(temppos-pos) < 70) ? OC1R *(1 + (abs(temppos-pos) /100)): OC1R *0;
            // PWMc = (pos-temppos); // TODO there needs to be scaling here or the motor will fuck itself
            // assume "base_PWN" is the default one we use in SPRING and TEXTURE
          } else {//else if(pos-temppos < 0){
            D10 = 0;
            D9 = 1;
            OC1R = (abs(temppos-pos) < 70) ? OC1R * (1 + (abs(temppos-pos) /100)): OC1R *0;
            // PWMc = (temppos-pos); // TODO there needs to be scaling here or the motor will fuck itself
             // OC1R = OC1R * (temppos-pos) / 1000;
            // assume "base_PWN" is the default one we use in SPRING and TEXTURE
          } //else {D10 = 1;}
        } else {
          D10 = 1;
          OC1R = OC1RS >> 4;
        }
        chan = 0;
        printf("T postion: %d\r\n",temppos);
        break;
    case TEXTURE: // should keep a change
        D10 = 1;
        // if(angle <= (angle%7)+2 || angle >= (angle%7)-2){
        //     D10 = 0;
        //     D9 = 0;
        // } else if(angle <= (angle%13)+2 || angle >= (angle%13)-2){
        //     D10 = 0;
        //     D9 = 1;
        //   }else {D10 = 1;}
        if(angle%29>=27 || angle%29<=2){
          D9 = 1;
          D10 = 0;
          // D10 = 1;
        } else if(angle%37>=35 || angle%37<=2){
          D9 = 0;
          D10 = 0;
          // D10 = 1;
        } else{D10 = 1;}

        break;              // should addd the 4th button
    case WALL: // should keep a change
        D6 = 1;
        // D10 = 1
        // PSEUDOCODE
        // let's say we want to stay within positions 200 and 500 (completely arbitrary)
        if(pos >= 60){
          D9 = 0;
          D10 = 0;
        } else if(pos <= -60){
          D9 = 1;
          D10 = 0;
        } else{D10 = 1;}
        break;
    default:
        D6 = 1;
  }
  // printf("\r\n");
}

int16_t main(void) {
    uint8_t *RPOR, *RPINR;

    init_elecanisms();
    init_ajuart();

    // Configure pin D13 to produce a 1-kHz PWM signal with a 25% duty cycle
    // using the OC1 module.

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

    // D13_DIR = OUT;      // configure D13 to be a digital output
    // D13 = 0;            // set D13 low

    D10_DIR = OUT;
    D9_DIR = OUT;
    D9 = 0;
    D10 = 1;
    D5_DIR = OUT;
    D8 = 0;

    __builtin_write_OSCCONL(OSCCON & 0xBF);
    RPOR[D8_RP] = OC1_RP;  // connect the OC1 module output to pin D13
    __builtin_write_OSCCONL(OSCCON | 0x40);

    OC1CON1 = 0x1C06;   // configure OC1 module to use the peripheral
                        //   clock (i.e., FCY, OCTSEL<2:0> = 0b111) and
                        //   and to operate in edge-aligned PWM mode
                        //   (OCM<2:0> = 0b110)
    OC1CON2 = 0x001F;   // configure OC1 module to syncrhonize to itself
                        //   (i.e., OCTRIG = 0 and SYNCSEL<4:0> = 0b11111)

    OC1RS = (uint16_t)(FCY / 1e3 - 1.);     // configure period register to
                                            //   get a frequency of 1kHz
    OC1R = OC1RS >> 4; //OC1R = OC1RS >> 2;  // configure duty cycle to 25% (i.e., period / 4)
    OC1TMR = 0;         // set OC1 timer count to 0

    USB_setup_vendor_callback = vendor_requests;
    init_usb();
    // while(1){
    //   modeControl();
    //   usb_service();
    // }
    while (USB_USWSTAT != CONFIG_STATE) {
#ifndef USB_INTERRUPT
        usb_service();
        modeControl();
#endif
    }
    while (1) {
#ifndef USB_INTERRUPT
        usb_service();
        modeControl();
#endif
    }
}
