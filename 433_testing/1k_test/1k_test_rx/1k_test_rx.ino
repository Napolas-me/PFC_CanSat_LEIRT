// ask_receiver.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to receive messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) receiver with an Rx-B1 module
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12

#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h>  // Not actually used but needed to compile
#endif

RH_ASK driver;  //mudamos na biblioteca os pins de rx e tx de 11 e 12 para 3 e 8
// RH_ASK driver(2000, 4, 5, 0); // ESP8266 or ESP32: do not use pin 11 or 2
//RH_ASK driver(2000, 3, 4, 0); // ATTiny, RX on D3 (pin 2 on attiny85) TX on D4 (pin 3 on attiny85),
// RH_ASK driver(2000, PD14, PD13, 0); STM32F4 Discovery: see tx and rx on Orange and Red LEDS

int start = 0;
int end = 0;
int nMessage = 0;

void setup() {
  Serial.begin(9600);  // Debugging only
  while(!Serial);
  Serial.println("Start Test");
 if (!driver.init())
    Serial.println("init failed");
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if(driver.recv(buf, &buflen)){
    
    //Serial.println(buf[0]);
    driver.printBuffer("Got:", buf, buflen);

    if(nMessage++ == 0) start = millis();
  
    if(buf ==  0xFF){
      end = millis();
      Serial.print("Running Time:");
      Serial.println(end-start);
      Serial.println("End Test");
      while(1);
    }    
  }
}
