#include <RH_ASK.h>
#include "mpu9250.h"

#include <stdio.h>

//add generic libraries

#define TX_PIN
#define SPEED 2000 //speed The desired bit rate in bits per second
#define interval 1500 // 1.5 seg
/*
 * second and last param not needed. 
 * driver(speed, rx, tx, en);
 */
RH_ASK driver(SPEED, 0, TX_PIN, 0); 

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

void loop() {
	// put your main code here, to run repeatedly:
  
	axis.ac_x = imu.accel_x_mps2();
	axis.ac_y = imu.accel_y_mps2();
	axis.ac_z = imu.accel_z_mps2();
	axis.gy_x = imu.gyro_x_radps();
	axis.gy_y = imu.gyro_y_radps();
	axis.gy_z = imu.gyro_z_radps();
	axis.mag_x = imu.mag_x_ut();
	axis.mag_y = imu.mag_y_ut();
	axis.mag_z = imu.mag_z_ut();
	
	delay(1000);
	
	char message[256];
	sprintf(message, "AC_X:%.2f, AC_Y:%.2f, AC_Z:%.2f, GY_X:%.2f, GY_Y:%.2f, GY_Z:%.2f, MAG_X:%.2f, MAG_Y:%.2f, MAG_Z:%.2f",axis.ac_x, axis.ac_y, axis.ac_z, axis.gy_x, axis.gy_y, axis.gy_z, axis.mag_x, axis.mag_y, axis.mag_z);
    
	//const char *msg = "hello world XD";
    //driver.send((uint8_t *)msg, strlen(msg));
	
    driver.send((uint8_t *)message, strlen(msg));
    driver.waitPacketSent();
    delay(1000);
}
