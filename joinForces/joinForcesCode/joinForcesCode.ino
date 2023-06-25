#include <RH_ASK.h>
#include "mpu9250.h"

#include <stdio.h>

//add generic libraries

#define TX_PIN 8
#define SPEED 2000 //speed The desired bit rate in bits per second
#define interval 1500 // 1.5 seg
/*
 * second and last param not needed. 
 * driver(speed, rx, tx, en);
 */
RH_ASK driver(SPEED, 0, TX_PIN, 0); 
bfs::Mpu9250 imu(&SPI, 2);

typedef struct {
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
} AxisDataStruct;

typedef struct {
   //pressure, temperature  & altitude
   float pres;
   float temp;
   float alt;
} BMPDataStruct;

typedef struct {
   //GPS stuff
   char* gpsMessage;
} GPSDataStruct;


AxisDataStruct axis;
//BMPDataStruct bmp;
//GPSDataStruct gps;

void setup() {
  
	/*433 Init*/
	
	#ifdef RH_HAVE_SERIAL
		Serial.begin(9600);	  // Debugging only
	#endif
		if (!driver.init())
	#ifdef RH_HAVE_SERIAL
			 Serial.println("init failed");
	#else
		;
	#endif
	
	/*Gy-9250 Init */
	
	/* Serial to display data */
	Serial.begin(115200);
	while(!Serial) {}
	/* Start the SPI bus */
  //SPI.begin()
	SPI.begin();
  
  	/* Initialize and configure IMU */
	if (!imu.Begin()) {
	Serial.println("Error initializing communication with IMU");
	while(1) {}
	}
	/* Set the sample rate divider */
	if (!imu.ConfigSrd(19)) {
	Serial.println("Error configured SRD");
	while(1) {}
	}
  
}
char message[10];

void loop() {
	// put your main code here, to run repeatedly:
  if (imu.Read()) {
    axis.ac_x = imu.accel_x_mps2();
    axis.ac_y = imu.accel_y_mps2();
    axis.ac_z = imu.accel_z_mps2();
    axis.gy_x = imu.gyro_x_radps();
    axis.gy_y = imu.gyro_y_radps();
    axis.gy_z = imu.gyro_z_radps();
    axis.mag_x = imu.mag_x_ut();
    axis.mag_y = imu.mag_y_ut();
    axis.mag_z = imu.mag_z_ut();

    
    sprintf(message, "%f", axis.ac_x);
  }
   // Serial.println(axis.ac_x);
    delay(1000);
	
	//sprintf(message, "AC_X:%f, AC_Y:%f, AC_Z:%f, GY_X:%f, GY_Y:%f, GY_Z:%f, MAG_X:%f, MAG_Y:%f, MAG_Z:%f",axis.ac_x, axis.ac_y, axis.ac_z, axis.gy_x, axis.gy_y, axis.gy_z, axis.mag_x, axis.mag_y, axis.mag_z);
      
	//const char *msg = "hello world XD";
  //driver.send((uint8_t *)msg, strlen(msg));
  for(int i = 0; i < strlen(message); i++){
    Serial.println(message[i], HEX);
  }
    Serial.println("--------------");
    //Serial.print("Size:");
    //Serial.println(strlen(message));
    driver.send((uint8_t *)message, strlen(message));
    //char char_value;

    // Copy float bytes to char using memcpy
    // memcpy(&char_value, &(axis.ac_x), sizeof(float));
    //Serial.print("char_value: ");
    //Serial.println(char_value);

    //driver.send(&char_value, strlen(char_value));
    driver.waitPacketSent();
    delay(1000);
}
