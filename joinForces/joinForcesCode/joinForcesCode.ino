#include <RH_ASK.h>

//add generic libraries

#define TX_PIN
#define SPEED 2000 //speed The desired bit rate in bits per second

/*
 * second and last param not needed. 
 * driver(speed, rx, tx, en);
 */
RH_ASK driver(SPEED, 0, TX_PIN, 0); 

typedef struct DATA_STRUCT{
   //AXIS vars
   float ac_x;
   float ac_y;
   float ac_z;
   float gy_x;
   float gy_y;
   float gy_z;
   float mag_x;
   float mag_y;
   float mag_z;

   //pressure, temperature  & altitude
   float pres;
   float temp;
   float alt;

   //GPS stuff(shit)
   char* gpsMessage;

}DataStruct;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
