/*
 *  OpenAquarium sensor platform for Arduino from Cooking-hacks.
 *
 *  Description: Open Aquarium platform for Arduino control several
 *  parameters in order to make a standalone aquarium. Sensors gather     
 *  information and correct possible errors with different actuators.  
 *
 *  This example send all the sensor data info to a web server
 *
 *  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
 *  http://www.libelium.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:           1.0
 *  Design:            David Gascon 
 *  Implementation:    Jorge Casanova
 */


#include "OpenAquarium.h"
#include "Wire.h"

#define point_1_cond 40000   // Write here your EC calibration value of the solution 1 in µS/cm
#define point_1_cal 40       // Write here your EC value measured in resistance with solution 1
#define point_2_cond 10500   // Write here your EC calibration value of the solution 2 in µS/cm
#define point_2_cal 120      // Write here your EC value measured in resistance with solution 2


#define calibration_point_4 2221  //Write here your measured value in mV of pH 4
#define calibration_point_7 2104  //Write here your measured value in mV of pH 7
#define calibration_point_10 2031 //Write here your measured value in mV of pH 10


String wifiString = "";
char buffer[20];
int flag = 0; // auxiliar variable
DateTime myTime;
String levelstatus1;
String levelstatus2;
String waterleakstatus;


char aux_str[90];
int8_t answer;
char response[300];


//Enter here your data
const char server[] = "http://YOUR_SERVER_IP";
const char server_port[] = "YOUR_SERVER_PORT";
const char wifi_ssid[] = "YOUR_WIFI_SSID";
const char wifi_password[] = "YOUR_WIFI_PASSWORD";
const char GET[] = "openaquarium/set_sensors.php?data=";


void setup() {
  Serial.begin(115200);
  OpenAquarium.init();    //Initialize sensors power 
  OpenAquarium.calibrateEC(point_1_cond,point_1_cal,point_2_cond,point_2_cal);
  OpenAquarium.calibratepH(calibration_point_4,calibration_point_7,calibration_point_10);

  wifiString.reserve(1000);

  wifireset(); 
  wificonfig(); 

}

void loop() {

  myTime =OpenAquarium.getTime();
  OpenAquarium.printTime(myTime);
  Serial.println(" ");
  delay(500);
  
  if ( myTime.second() == 0 && flag == 0  ){ //Only enter 1 time each minute
    //Get Gateway Sensors
   
    float temperature = OpenAquarium.readtemperature();
    float resistanceEC = OpenAquarium.readResistanceEC(); //EC Value in resistance
    float EC2 = OpenAquarium.ECConversion(resistanceEC); //EC Value in µS/cm  
    int mvpH = OpenAquarium.readpH(); //Value in mV of pH
    float pH2 = OpenAquarium.pHConversion(mvpH); //Calculate pH value
    
    
    int status1 = OpenAquarium.readwaterlevel(1);
    if(status1==HIGH){
      levelstatus1= "Full";
    }
    else if (status1==LOW){
      levelstatus1= "Empty"; 
    }
    int status2 = OpenAquarium.readwaterlevel(2);
    if(status2==HIGH){
      levelstatus2= "Full";
    }
    else if (status2==LOW){
      levelstatus2= "Empty";
    }
    int waterleak = analogRead(3);
   if (waterleak>1020){
      waterleakstatus="No";
    }
    else if (waterleak<=1020){
      waterleakstatus="Si";
    }
    //Create string of the floats to send it
    dtostrf(temperature,2,2,buffer);
    String temperature_wf = String (buffer);
    dtostrf(EC2,2,2,buffer);
    String EC_wf = String (buffer);
    dtostrf(pH2,2,2,buffer);
    String pH_wf = String (buffer);

    /*
   You must create strings with this structure: "node_id:sensor_type:value;node_id2:sensor_type2:value2;...." 
     Note the ";" between different structures
          
     And where sensor_type:
     0 - Temperature 
     1 - EC
     2 - pH
     3 - Waterlevel1
     4 - Waterlevel2 
     5 - waterleak 
          
     For example: "0:0:56;0:1:17.54;0:2:56.45"
     This means that you send data of the gateway: Soil moisture=56, Soil temperature=17.54 and Air humidity=56.45
     
     */
    //Cut data in several string because the wifi module have problems with longer strings
    wifiString= "0:" + temperature_wf + ";1:" + EC_wf + ";2:" + pH_wf; 
     sendGET();
    wifiString= "3:" + levelstatus1 + ";4:"  + levelstatus2 + ";5:"  + waterleakstatus; 
    sendGET();
    flag = 1;  

  }

  else if (myTime.second() == 30 && flag == 1){
    flag = 0;
  } 
}



//*********************************************************************
//*********************************************************************

void sendGET() {
  int str_len = wifiString.length() + 1; 
  char char_array[str_len];
  wifiString.toCharArray(char_array, str_len);

  snprintf(aux_str, sizeof(aux_str), "AT+iRLNK:\"%s:%s/%s%s\"\r", server, server_port, GET, char_array);
  sendCommand(aux_str,"I/OK",20000);
}


//**********************************************
void wifireset() { 
  Serial.println(F("Setting Wifi parameters"));
  sendCommand("AT+iFD\r","I/OK",2000);
  delay(1000);  
  sendCommand("AT+iDOWN\r","I/OK",2000);
  delay(6000);  
}


//**********************************************
void wificonfig() {
  
   Serial.println(F("Setting Wifi parameters"));
  sendCommand("AT+iFD\r","I/OK",2000);
  delay(2000);  

  snprintf(aux_str, sizeof(aux_str), "AT+iWLSI=%s\r", wifi_ssid);
  answer = sendCommand(aux_str,"I/OK",10000);

  if (answer == 1){

    snprintf(aux_str, sizeof(aux_str), "Joined to \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(5000);
  }

  else {
    snprintf(aux_str, sizeof(aux_str), "Error joining to: \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(1000);
  }

  snprintf(aux_str, sizeof(aux_str), "AT+iWPP0=%s\r", wifi_password);
  sendCommand(aux_str,"I/OK",20000);
  delay(1000);

  if (answer == 1){

    snprintf(aux_str, sizeof(aux_str), "Connected to \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(5000);
  }

  else {
    snprintf(aux_str, sizeof(aux_str), "Error connecting to: \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(1000);
  }

  sendCommand("AT+iDOWN\r","I/OK",2000);
  delay(6000); 
}


//**********************************************
int8_t sendCommand(const char* Command, const char* expected_answer, unsigned int timeout){

  uint8_t x=0,  answer=0;
  unsigned long previous;

  memset(response, 0, 300);    // Initialize the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(Command);    // Send Command 

    x = 0;
  previous = millis();

  // this loop waits for the answer
  do{
    if(Serial.available() != 0){    
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial.read();
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL)    
      {
        answer = 1;
      }
    }
  }
  // Waits for the asnwer with time out
  while((answer == 0) && ((millis() - previous) < timeout));    

  return answer;
}


//**********************************************


