/* 
 * File:   HTU21DF.c
 * Author: Jason McGuire (j.mcguire.2015@ieee.org)
 * Description: A library for a HTU21D-F Humidity/Temperature Sensor
 * for the PIC18F using I2C.
 * See example.c for implementation of the library
 * Sample Product: https://www.adafruit.com/products/1899
 * Created on March 27, 2016, 9:08 AM
 
License Information:
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 
 
 */
 
#include "HTU21DF.h"

// Available Functions:
unsigned char read8bits(char); // read an 8 bit register 
void write8bits(char, char); // write a byte to an 8 bit register
unsigned int read16bits(char); // read two bytes from two registers 
void I2Cstart(); // Start signal for I2C 
void I2Cstop(); // Stop signal for I2C
void I2Cack(); // Acknowledgement signal for I2C
void I2Cnack(); // No-Acknowledgement signal for I2C
void I2Crestart(); // Restart signal for I2C
unsigned char I2Cread(); // Read signal for I2C
void I2Cwrite(char); // Write a byte for I2C
void I2Cwait(); // Wait for flags to clear 
float readTemp(); // Get a temperature reading in F
float readHumidity(); // Get a humidity reading in %RH


void I2Cstart(){
    SEN = 1; // Initiate start condition on SDA/SCL. Cleared by hardware
    while(SEN); // wait for it to be cleared 
}

void I2Cstop(){
    PEN = 1; // Initiate stop condition on SDA/SCL. Cleared by hardware 
    while(PEN); // wait for it to be cleared 
    
}

void I2Cack(){
    ACKDT = 0; // Acknowledge 
    ACKEN = 1; // Initiate acknowledge condition on SDA/SCL
    while(ACKEN); // wait for it to be cleared 
}

void I2Cnack(){
    ACKDT = 1; // Not Acknowledge
    ACKEN = 1; // Initiate acknowledge condition of SDA/SCL
    while(ACKEN); // wait for it to be cleared 
    
}

void I2Crestart(){
    RSEN = 1; // Initiate restart condition on SDA/SCL.
    while(RSEN); // wait for it to be cleared 
    
}

unsigned char I2Cread(){
    RCEN = 1; // enables receive mode for I2C
    while(!BF); // wait until SSPBUF is full
    I2Cwait(); 
    return SSPBUF; // get the data 
}

void I2Cwrite(char data){
    SSPBUF = data; // put data in the buffer
    while(BF); //  wait until data leaves the buffer 
    I2Cwait();
} 

void I2Cwait(){
    while ((SSPCON2 & 0x1F ) || (SSPSTAT & 0x04 )); 
}

float readTemp(){
    I2Cstart(); // start
    I2Cwrite(HTU_WR_ADDR); // I2C address + write 
    while(ACKSTAT); // wait for ack
    I2Cwrite(HTU_TEMP_CMD); // send commnand 
    while(ACKSTAT); // wait for ack
    I2Cstart(); // start again
    I2Cwrite(HTU_RD_ADDR); // I2C address + read 
    while(ACKSTAT); // wait for ack 
    unsigned char msb = I2Cread(); // read 8 bits 
    I2Cack(); // send ack
    unsigned char lsb = I2Cread(); // read 8 bits 
    I2Cack(); // send ack
    unsigned char crc = I2Cread();// read crc 
    I2Cnack(); // send nack
    I2Cstop(); // send stop bit 
    
    // handle data
    unsigned int temp = msb;
    temp = temp << 8; 
    temp = temp | (lsb & 0xFC);
    float tempC = -46.85 + 175.72*((float)temp/65536.0); // Use formula from datasheet
    float tempF = tempC*9.0/5.0 + 32.0; //  Convert to Fahrenheit
    return tempF;
}

float readHumidity(){
    I2Cstart(); // start
    I2Cwrite(HTU_WR_ADDR); // I2C address + write 
    while(ACKSTAT); // wait for ack
    I2Cwrite(HTU_HUMI_CMD); // send commnand 
    while(ACKSTAT); // wait for ack
    I2Cstart();
    I2Cwrite(HTU_RD_ADDR); // I2C address + read 
    while(ACKSTAT); // wait for ack 
    unsigned char msb = I2Cread(); // read 8 bits 
    I2Cack(); // send ack
    unsigned char lsb = I2Cread(); // read 8 bits 
    I2Cack(); // send ack
    unsigned char crc = I2Cread();// read crc 
    I2Cnack(); // send nack
    I2Cstop(); // send stop bit
    
    // process data 
    unsigned int rh = msb;
    rh = rh << 8;
    rh = rh | (lsb & 0xFC); 
    float rhfloat = -6 + 125*(float)rh/65536;
    return rhfloat;
}
