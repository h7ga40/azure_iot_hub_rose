/* 
 * MIT License
 *
 * Copyright (c) 2019 Hiroyuki Okada
 *
 * https://github.com/okhiroyuki/ADNS5050
 */
/*
 * Modify to C language from C++/Arduino
 */
#include <stdint.h>
#include "adns5050.h"
#include "Arduino.h"

static int ConvertToSignedNumber(uint8_t twoscomp);

//Constructor sets the pins used for the mock 'i2c' communication
void adns5050_init(adns5050_t *adns5050, int sdio, int sclk, int ncs)
{
	adns5050->_sdio = sdio;
	adns5050->_sclk = sclk;
	adns5050->_ncs = ncs;
}

//Configures the communication pins for their initial state
void adns5050_begin(adns5050_t *adns5050)
{
	pinMode(adns5050->_sdio, OUTPUT);
	pinMode(adns5050->_sclk, OUTPUT);
	pinMode(adns5050->_ncs, OUTPUT);
}

//Essentially resets communication to the ADNS5050 module
void adns5050_sync(adns5050_t *adns5050)
{
	digitalWrite(adns5050->_ncs, LOW);
	delayMicroseconds(1);
	digitalWrite(adns5050->_ncs, HIGH);
}

//Reads a register from the ADNS5050 sensor. Returns the result to the calling function.
//Example: value = mouse.read(CONFIGURATION_REG);
int adns5050_read(adns5050_t *adns5050, unsigned char addr)
{
  uint8_t temp;
  int n;

  //select the chip
  digitalWrite(adns5050->_ncs, LOW);	//nADNSCS = 0;
  temp = addr;

  //start clock low
  digitalWrite(adns5050->_sclk, LOW); //SCK = 0;

	//set data line for output
  pinMode(adns5050->_sdio, OUTPUT); //DATA_OUT;
  //read 8bit data
  for (n=0; n<8; n++) {
    //delayMicroseconds(2);
    digitalWrite(adns5050->_sclk, LOW);//SCK = 0;
    //delayMicroseconds(2);
    pinMode(adns5050->_sdio, OUTPUT); //DATA_OUT;
    if (temp & 0x80) {
      digitalWrite(adns5050->_sdio, HIGH);//SDOUT = 1;
    }
    else {
      digitalWrite(adns5050->_sdio, LOW);//SDOUT = 0;
    }
    delayMicroseconds(2);
    temp = (temp << 1);
    digitalWrite(adns5050->_sclk, HIGH); //SCK = 1;
  }

  // This is a read, switch to input
  temp = 0;
  pinMode(adns5050->_sdio, INPUT); //DATA_IN;
  //read 8bit data
	for (n=0; n<8; n++) {		// read back the data
    delayMicroseconds(1);
    digitalWrite(adns5050->_sclk, LOW);
    delayMicroseconds(1);
    if(digitalRead(adns5050->_sdio)) {
      temp |= 0x1;
    }
    if( n != 7) temp = (temp << 1); // shift left
    digitalWrite(adns5050->_sclk, HIGH);
  }
  delayMicroseconds(20);
  digitalWrite(adns5050->_ncs, HIGH);// de-select the chip
  return ConvertToSignedNumber(temp);
}

int ConvertToSignedNumber(uint8_t twoscomp){
  int value;

	if (bitRead(twoscomp,7)){
    value = -128 + (twoscomp & 0b01111111 );
  }
  else{
    value = twoscomp;
  }
	return value;
}

//Writes a value to a register on the ADNS2620.
//Example: mouse.write(CONFIGURATION_REG, 0x01);
void adns5050_write(adns5050_t *adns5050, unsigned char addr, unsigned char data)
{
  char temp;
  int n;

  //select the chip
  //nADNSCS = 0;
  digitalWrite(adns5050->_ncs, LOW);

  temp = addr;
  //クロックを開始
  digitalWrite(adns5050->_sclk, LOW);//SCK = 0;					// start clock low
  //SDIOピンを出力にセット
  pinMode(adns5050->_sdio, OUTPUT);//DATA_OUT; // set data line for output
  //8ビットコマンドの送信
  for (n=0; n<8; n++) {
    digitalWrite(adns5050->_sclk, LOW);//SCK = 0;
    pinMode(adns5050->_sdio, OUTPUT);
    delayMicroseconds(1);
    if (temp & 0x80)  //0x80 = 0101 0000
      digitalWrite(adns5050->_sdio, HIGH);//SDOUT = 1;
    else
      digitalWrite(adns5050->_sdio, LOW);//SDOUT = 0;
    temp = (temp << 1);
    digitalWrite(adns5050->_sdio, HIGH);//SCK = 1;
    delayMicroseconds(1);//delayMicroseconds(1);			// short clock pulse
  }
	temp = data;
  for (n=0; n<8; n++) {
    delayMicroseconds(1);
    digitalWrite(adns5050->_sclk, LOW);//SCK = 0;
    delayMicroseconds(1);
    if (temp & 0x80)
      digitalWrite(adns5050->_sdio, HIGH);//SDOUT = 1;
    else
      digitalWrite(adns5050->_sdio, LOW);//SDOUT = 0;
    temp = (temp << 1);
    digitalWrite(adns5050->_sclk, HIGH);//SCK = 1;
    delayMicroseconds(1);			// short clock pulse
  }
  delayMicroseconds(20);
  digitalWrite(adns5050->_ncs, HIGH);//nADNSCS = 1; // de-select the chip

}
