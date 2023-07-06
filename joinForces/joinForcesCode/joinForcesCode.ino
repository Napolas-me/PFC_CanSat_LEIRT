#include <RH_ASK.h> //433
#include <mpu9250.h> //axis

#include <Adafruit_BMP3XX.h>
#include <bmp3.h>
#include <bmp3_defs.h>//bmp390

#include <TinyGPS++.h>//gps
#include <TinyGPSPlus.h>

#include <stdio.h>

//add generic libraries

#define TX_PIN 8
#define SPEED 2000 //speed The desired bit rate in bits per second
#define interval 1500 // 1.5 seg
#define MSG_SIZE 256
#define BMP_CS 10
#define BMP_SCK 7
#define BMP_MISO 9
#define BMP_MOSI 8

#define SEALEVELPRESSURE_HPA_PORTUGAL (1014)
/*
 * second and last param not needed. 
 * driver(speed, rx, tx, en);
 */
RH_ASK driver(SPEED, 0, TX_PIN, 0); 
bfs::Mpu9250 imu(&SPI, 2);
Adafruit_BMP3XX bmp390;
TinyGPSPlus neo;

typedef struct {
   float ac_x;
   float ac_y;
   float ac_z;
   float gy_x;
   float gy_y;
   float gy_z;
   float mag_x;
   float mag_y;
   float mag_z;
   //int lastRead;
   //int lastSent;
} AxisDataStruct;

typedef struct {
   float pres;
   float temp;
   float alt;
   //int lastRead;
   //int lastSent;
} BMPDataStruct;

typedef struct {
   double lat;
   double lng;
   //int lastRead;
   //int lastSent;
} GPSDataStruct;

AxisDataStruct axis;
BMPDataStruct bmp;
GPSDataStruct gps;

char* messageAxis;
char* messageGps;
char* messageBmp;

void errorMsg(char* msg){
  Serial.print("ERROR: ");
  Serial.println(msg);
  while(1);
}

int appendFloat(char* msg, char sep, float value){
  char* helperMsg;
  dtostrf(value, 0, 6, helperMsg);

  int helperSize = strlen(helperMsg);
  int initialSize = strlen(msg);

  int i = initialSize, j = 0;
  for(; i < MSG_SIZE && j <= helperSize; i++, j++){
    if(i == initialSize){
      msg[i] = sep;
      i++;
    }
    msg[i] = helperMsg[j]; 

    if(i+1 == MSG_SIZE) errorMsg("Not enough space in msg buffer");
  }
  //msg++ = sep;
  msg[i+1] = '\0'; // terminar string

  return strlen(msg);
}

void setup() {
  
	/*433 Init*/
	messageAxis = malloc(MSG_SIZE);
  messageBmp = malloc(MSG_SIZE);
  messageGps = malloc(MSG_SIZE);

  Serial.begin(9600);

  while(!Serial);
  Serial.println("Serial inicialized!");

  if(!driver.init()) errorMsg("Transmitter not initialized");

  SPI.begin();
	
	/*Gy-9250 Init */
  	/* Initialize and configure IMU */
	if (!imu.Begin())	errorMsg("Error initializing communication with IMU");
	
	/* Set the sample rate divider */
	if (!imu.ConfigSrd(19))	errorMsg("Error configured SRD");

  //startup bmp
  if (!bmp390.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) errorMsg("Could not find a valid BMP3 sensor, check wiring!");

  //bmp390.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  //bmp390.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  //bmp390.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  //bmp390.setOutputDataRate(BMP3_ODR_50_HZ);

  // populate needed vars
  /*
  axis.lastRead = 0;
  axis.lastSent = 0;

  bmp.lastRead = 0;
  bmp.lastSent = 0;

  gps.lastRead = 0;
  gps.lastSent = 0;
  */

  messageAxis[0] = 'A'; //start char indicates what message is receiving
  messageBmp[0] = 'B';
  messageGps[0] = 'G';
}

void resetAxisMsg(){
  messageAxis[1] = '\0';
}

void resetBmpMsg(){
  messageBmp[1] = '\0';
}

void resetGpsMsg(){
  messageGps[1] = '\0';
}

void readAxis(){
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

    //axis.lastRead++;
  }

  //Assemble msg
  appendFloat(messageAxis, ',', axis.ac_x );
  appendFloat(messageAxis, ',', axis.ac_y );
  appendFloat(messageAxis, ',', axis.ac_z);
  appendFloat(messageAxis, ',', axis.gy_x);
  appendFloat(messageAxis, ',', axis.gy_y);
  appendFloat(messageAxis, ',', axis.gy_z);
  appendFloat(messageAxis, ',', axis.mag_x);
  appendFloat(messageAxis, ',', axis.mag_y);
  appendFloat(messageAxis, ',', axis.mag_z);
}

void readBmp(){
  if (bmp390.performReading()){
    bmp.temp = bmp390.temperature;
    bmp.pres = bmp390.pressure / 100;
    bmp.alt = bmp390.readAltitude(SEALEVELPRESSURE_HPA_PORTUGAL); 
  }
  appendFloat(messageBmp, ',', bmp.pres);
  appendFloat(messageBmp, ',', bmp.temp);
  appendFloat(messageBmp, ',', bmp.alt);
}

void readGps(){
  if(neo.satellites.isValid()){
    gps.lat = neo.location.lat();
    gps.lng = neo.location.lng();
  }
  appendFloat(messageGps, ',', gps.lat);
  appendFloat(messageGps, ',', gps.lng);
}

void sendMsg(char* msg){
  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();//deve tornar bloqueante, ver melhor
}

//maybe no need for this check in the following 3 funcs

void sendAxis(){
  //if(axis.lastRead > axis.lastSent){
    sendMsg(messageAxis);
    //axis.lastSent++;
  //}
}

void sendBmp(){
  //if(bmp.lastRead > bmp.lastSent){
    sendMsg(messageBmp);
    //bmp.lastSent++;
  //}
}

void sendGps(){
  //if(gps.lastRead > gps.lastSent){
    sendMsg(messageGps);
    //gps.lastSent++;
  //}
}

unsigned long beforeRead;
void loop() {
  beforeRead = millis();

  readAxis();
  readBmp();
  readGps();

  Serial.print("Reading from sensors time : ");
  Serial.println(millis() - beforeRead);

  sendAxis();
  sendBmp();
  sendGps();

  Serial.print("Loop time: ");
  Serial.println(millis() - beforeRead);
}
