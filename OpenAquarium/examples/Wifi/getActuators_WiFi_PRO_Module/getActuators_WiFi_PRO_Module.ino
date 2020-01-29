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

//Enter here your data
const char server[] = "http://YOUR_SERVER_IP";
const char server_port[] = "YOUR_SERVER_PORT";
const char wifi_ssid[] = "YOUR_WIFI_SSID";
const char wifi_password[] = "YOUR_WIFI_PASSWORD";
const char GET[] = "openaquarium/get_actuators.php\r";
const char GETfeeder[] = "openaquarium/stop_feeder.php\r";

char recv[512];
int cont;

char pump1_wf;
char pump2_wf;

char peristalticPump1_wf;
char peristalticPump2_wf;
char peristalticPump3_wf;

char socket1_wf;
char socket2_wf;
char socket3_wf;
char socket4_wf;
char socket5_wf;

char feederFish_wf;

char ledLight_wf;


unsigned long on1 = 1597145202;
unsigned long off1 = 1597145204;
unsigned long on2 = 1597145220;
unsigned long off2 = 1597145222;
unsigned long on3 = 1597145112;
unsigned long off3 = 1597145114;
unsigned long on4 = 1597144950;
unsigned long off4 = 1597144952;
unsigned long on5 = 1597144464;
unsigned long off5 = 1597144466;

int8_t answer;
char response[300];
char aux_str[90];

void setup() {
  Serial.begin(115200);
  OpenAquarium.init();    //Initialize sensors power 
  cleanVector();
  wifireset();
  wificonfig();
}

void loop() {
  getActuators();
  Serial.println();
  
   if (pump1_wf == '0'){
    OpenAquarium.pumpOFF(1); //Turn OFF the pump number 1
    Serial.print(F("Pump 1: OFF "));
  }
  else if (pump1_wf == '1'){
    OpenAquarium.pumpON(1); //Turn ON the pump number 1
    Serial.print(F("Pump 1: ON "));
  }
  else{
    Serial.print(F("Pump 1 BAD DATA "));
  }
  
  if (pump2_wf == '0'){
    OpenAquarium.pumpOFF(1); //Turn OFF the pump number 2
    Serial.print(F("Pump 2: OFF "));
  }
  else if (pump2_wf == '1'){
    OpenAquarium.pumpON(2); //Turn ON the pump number 2
   Serial.print(F("Pump 2: ON "));
  }
  else{
    Serial.print(F("Pump 2 BAD DATA "));
  }
    
  if (peristalticPump1_wf == '0'){
    OpenAquarium.perpumpOFF(1); //Turn OFF the peristaltic pump number 1 
    Serial.print(F("Peristaltic pump 1: OFF "));
  }
  else if (peristalticPump1_wf == '1'){
    OpenAquarium.perpumpON(1); //Turn ON the peristaltic pump number 1 
    Serial.print(F("Peristaltic pump 1: ON "));
  }
  else{
    Serial.print(F("Peristaltic pump 1 BAD DATA "));
  }


  if (peristalticPump2_wf == '0'){
    OpenAquarium.perpumpOFF(2); //Turn OFF the peristaltic pump number 2
    Serial.print(F("Peristaltic pump 2: OFF "));
  }
  else if (peristalticPump2_wf == '1'){
    OpenAquarium.perpumpON(2); //Turn ON the peristaltic pump number 2
   Serial.print(F("Peristaltic pump 2: ON "));
  }
  else{
    Serial.print(F("Peristaltic pump 2 BAD DATA "));
  }


  if (peristalticPump3_wf == '0'){
    OpenAquarium.perpumpOFF(3); //Turn OFF the peristaltic pump number 3
     Serial.print(F("Peristaltic pump 3: OFF "));
  }
  else if (peristalticPump3_wf == '1'){
    OpenAquarium.perpumpON(3); //Turn ON the peristaltic pump number 3
    Serial.print(F("Peristaltic pump 3: ON "));
  }
  else{
    Serial.print(F("Peristaltic pump 3 BAD DATA "));
  }
  
  if (socket1_wf == '0'){
    OpenAquarium.sendPowerStrip(off1); //Turn OFF plug 1
    Serial.print(F("Socket 1: OFF "));
  }
  else if (socket1_wf == '1'){
    OpenAquarium.sendPowerStrip(on1); //Turn ON plug 1
   Serial.print(F("Socket 1: ON "));
  }
  else{
    Serial.print(F("Socket 1 BAD DATA "));
  }
  
  if (socket2_wf == '0'){
    OpenAquarium.sendPowerStrip(off2); //Turn OFF plug 2
    Serial.print(F("Socket 2: OFF "));
  }
  else if (socket2_wf == '1'){
    OpenAquarium.sendPowerStrip(on2); //Turn ON plug 2
    Serial.print(F("Socket 2: ON "));
  }
  else{
   Serial.print(F("Socket 2 BAD DATA "));
  }
  
  if (socket3_wf == '0'){
    OpenAquarium.sendPowerStrip(off3); //Turn OFF plug 3
    Serial.print(F("Socket 3: OFF "));
  }
  else if (socket3_wf == '1'){
    OpenAquarium.sendPowerStrip(on3); //Turn ON plug 3
    Serial.print(F("Socket 3: ON "));
  }
  else{
    Serial.print(F("Socket 3 BAD DATA "));
  }
  
  if (socket4_wf == '0'){
    OpenAquarium.sendPowerStrip(off4); //Turn OFF plug 4
    Serial.print(F("Socket 4: OFF "));
  }
  else if (socket4_wf == '1'){
    OpenAquarium.sendPowerStrip(on4); //Turn ON plug 4
    Serial.print(F("Socket 4: ON "));
  }
  else{
    Serial.print(F("Socket 4 BAD DATA "));
  }
  
  if (socket5_wf == '0'){
    OpenAquarium.sendPowerStrip(off5); //Turn OFF plug 5
    Serial.print(F("Socket 5: OFF "));
  }
  else if (socket5_wf == '1'){
    OpenAquarium.sendPowerStrip(on5); //Turn ON plug 5
    Serial.print(F("Socket 5: ON "));
  }
  else{
    Serial.print(F("Socket 5 BAD DATA "));
  }
  
  if (ledLight_wf == '0'){
    OpenAquarium.lightOFF(); //Turn OFF plug 5
    Serial.print(F("Light: OFF "));
  }
  else if (ledLight_wf == '1'){
    OpenAquarium.lightON(); //Turn ON plug 5
    Serial.print(F("Light: ON "));
  }
  else{
    Serial.print(F("Led Light BAD DATA "));
  }
  
  if (feederFish_wf == '1'){
    OpenAquarium.feedfish();
    Serial.print(F("Feeding fishes "));
    sendfeederstatus();
    delay(500);
  }
  else if (feederFish_wf == '0'){
    OpenAquarium.readyforfeeding();    
    Serial.print(F("Feeder OFF "));
  }
  else{
    Serial.print(F("Fish Feeder BAD DATA "));
  }
    
  cleanVector();

  delay(5000);
}




//*********************************************************************
//*********************************************************************
void sendfeederstatus(){
  snprintf(aux_str, sizeof(aux_str), "AT+iRLNK:\"%s:%s/%s\"\r", server, server_port, GETfeeder);
  sendCommand(aux_str,"I/OK",5000); 
  delay(100);
}


//**********************************************
void cleanVector(){
  recv[0] = 0; 
  recv[1] = 0;
  recv[2] = 0;
  recv[3] = 0; 
  recv[4] = 0;
  recv[5] = 0;
  recv[6] = 0;
  recv[7] = 0;
  recv[8] = 0;
  recv[9] = 0;
  recv[10] = 0;
  recv[11] = 0;
}


//**********************************************
void wifireset() {
  
  //Serial.println(F("Setting Wifi parameters"));
  sendCommand("AT+iFD\r","I/OK",2000);
  delay(1000);  
  sendCommand("AT+iDOWN\r","I/OK",2000);
  delay(6000);  
}


//**********************************************
void wificonfig() {
  
 // Serial.println(F("Setting Wifi parameters"));
  sendCommand("AT+iFD\r","I/OK",2000);
  delay(2000);  

  snprintf(aux_str, sizeof(aux_str), "AT+iWLSI=%s\r", wifi_ssid);
  answer = sendCommand(aux_str,"I/OK",10000);
/*
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
*/
  snprintf(aux_str, sizeof(aux_str), "AT+iWPP0=%s\r", wifi_password);
  sendCommand(aux_str,"I/OK",20000);
  delay(1000);
/*
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
*/
  sendCommand("AT+iDOWN\r","I/OK",2000);
  delay(6000); 


}


//**********************************************
void getActuators(){

  Serial.println(F(" "));
  snprintf(aux_str, sizeof(aux_str), "AT+iRLNK:\"%s:%s/%s\"\r", server, server_port, GET);
  sendCommand(aux_str,"I/OK",10000); 
  delay(100);
  checkData();
}



//**********************************************
void checkData(){
  cont=0;   
  delay(3000);
  while (Serial.available()>0)
  {
    recv[cont]=Serial.read(); 
    delay(10);
    cont++;
  }
  recv[cont]='\0';

  pump1_wf= recv[8]; 
  pump2_wf= recv[9];
  peristalticPump1_wf= recv[10];
  peristalticPump2_wf= recv[11]; 
  peristalticPump3_wf= recv[12];
  socket1_wf= recv[13];
  socket2_wf= recv[14];
  socket3_wf= recv[15];
  socket4_wf= recv[16];
  socket5_wf= recv[17];
  feederFish_wf= recv[18]; 
  ledLight_wf= recv[19];

  //Serial.print("Received:");
  //Serial.print(recv);

  delay(100);
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
