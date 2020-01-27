/*
Project: Aquarium controller
Autor: Shpakov Vlad (aka SendSay)
Date: 12.12.2019
Ver. 0.1a
*/

#include <Arduino.h>
#include <main.h>
#include "DallasTemperature.h"
#include "OneWire.h"
#include "RTClib.h"
#include "GravityTDS.h"

/*
.########.##.....##.##....##..######..########.####..#######..##....##
.##.......##.....##.###...##.##....##....##.....##..##.....##.###...##
.##.......##.....##.####..##.##..........##.....##..##.....##.####..##
.######...##.....##.##.##.##.##..........##.....##..##.....##.##.##.##
.##.......##.....##.##..####.##..........##.....##..##.....##.##..####
.##.......##.....##.##...###.##....##....##.....##..##.....##.##...###
.##........#######..##....##..######.....##....####..#######..##....##
*/
void alarm(int count);            	// 	Alarm signal
void fishFeeding();               	// 	Feeding fish
void printTime(DateTime now);     	// 	Print time and date
// void prepareToFeeding();			//	Prepare function fishFeeding

/*
..#######..########........##.########..######..########..######.
.##.....##.##.....##.......##.##.......##....##....##....##....##
.##.....##.##.....##.......##.##.......##..........##....##......
.##.....##.########........##.######...##..........##.....######.
.##.....##.##.....##.##....##.##.......##..........##..........##
.##.....##.##.....##.##....##.##.......##....##....##....##....##
..#######..########...######..########..######.....##.....######.
*/
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature. 
RTC_DS1307 rtc;                 // Clock
DateTime now;                   // Now date time
GravityTDS gravityTds;			// Tds sensor


/*
.########.##.....##.##....##..######..########.####..#######..##....##..######.
.##.......##.....##.###...##.##....##....##.....##..##.....##.###...##.##....##
.##.......##.....##.####..##.##..........##.....##..##.....##.####..##.##......
.######...##.....##.##.##.##.##..........##.....##..##.....##.##.##.##..######.
.##.......##.....##.##..####.##..........##.....##..##.....##.##..####.......##
.##.......##.....##.##...###.##....##....##.....##..##.....##.##...###.##....##
.##........#######..##....##..######.....##....####..#######..##....##..######.
*/
void fishFeeding() 
{
	if (not alarmNow) 
	{          
		if (startFeeding == false) 
		{				
			Serial.print("Fish feeding! > ");
			
			alarm(signal.START_FEEDING);

			digitalWrite(feeder,HIGH);  //startFeeding
			startFeeding = true;

			delay(2000);      
			while (!(digitalRead(feederpos) == LOW) && (startFeeding == true)) 
			{
			//Wait until rotate
			}
			
			digitalWrite(feeder,LOW);   //stop
			// startFeeding = false;  

			alarm(signal.END_OPERATION);
		}
	}
}

void alarm(int count) 
{
	if (not alarmNow) {
		printTime(now);							
		alarmNow = true;
		for (int i = 0; i < count; i++)
		{
			digitalWrite (buzzer,HIGH);
			delay(100);
			digitalWrite (buzzer,LOW);
			delay(100);
		}
		alarmNow = false;
	}
}

void printTime(DateTime now) 
{
	now = rtc.now();			// get time;
	Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();    
}

int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}

/*
..######..########.########.##.....##.########.
.##....##.##..........##....##.....##.##.....##
.##.......##..........##....##.....##.##.....##
..######..######......##....##.....##.########.
.......##.##..........##....##.....##.##.......
.##....##.##..........##....##.....##.##.......
..######..########....##.....#######..##.......
*/
void setup() 
{
	pinMode(feeder, OUTPUT);
	pinMode(feederpos, INPUT);
	pinMode(buzzer, OUTPUT);
	pinMode(level1, INPUT);

    gravityTds.setPin(TdsSensorPin);	//set pin Tds meter
    gravityTds.setAref(5.0);  			//reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  		//1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  				//initialization

	Serial.begin(9600);
	Serial.println("");
	Serial.println("*** Welcome to fish controller! ***");

	rtc.begin();
	// rtc.adjust(DateTime(__DATE__, __TIME__));    // adjust time

	alarm(signal.WELCOME);

	// sensors.setResolution(insideThermometer, 12);	
}

/*
.##........#######...#######..########.
.##.......##.....##.##.....##.##.....##
.##.......##.....##.##.....##.##.....##
.##.......##.....##.##.....##.########.
.##.......##.....##.##.....##.##.......
.##.......##.....##.##.....##.##.......
.########..#######...#######..##.......
*/
void loop() 
{
	now = rtc.now();			// get time;

//*************************
	if (millis() - timings.timing > 5000)
	{ 
		timings.timing = millis(); 	 

		gravityTds.setTemperature(temperature);  	//set the temperature and execute temperature compensation
		gravityTds.update(); 						//sample and calculate 
		tdsValue = gravityTds.getTdsValue();  		// then get the value
		Serial.print(tdsValue, 0);
		Serial.println("ppm");		

	}


//*** check water level
	waterLevel = digitalRead(level1);      
	if (waterLevel == HIGH) 
	{
		if (waterLevelFlag == false) {		// signal
			Serial.print("WaterLevel low! > ");
			alarm(signal.WATER_LEVEL);
			waterLevelFlag = true;	
		}
		if (millis() - timings.timing2 > params.ALARM_REPEAT) //timer
		{ 
			timings.timing2 = millis(); 
			waterLevelFlag = false;
		} 
	} else {
		waterLevelFlag = false;
	}

//*** Check temperature
	sensors.requestTemperatures();
	temperature = sensors.getTempCByIndex(0);

	if ((temperature > params.TEMP_MAX) || (temperature < params.TEMP_MIN)) 
	{
		if (tempOverFlag == false) {			//signal	
			Serial.print("Temperature over limit! > temp: ");
			Serial.print(temperature);
			Serial.println(" > ");
			alarm(signal.TEMPERATURE);
			tempOverFlag = true;
					}
		if (millis() - timings.timing3 > params.ALARM_REPEAT)  //timer
		{ 
			timings.timing3 = millis(); 
			tempOverFlag = false;
		}
	} else {
		tempOverFlag = false;
	}

//*** feed fish
	// morning
	if ((now.hour() == params.FEED_FIRST_H) && (now.minute() == params.FEED_FIRST_M) && (now.second() == 0))
	{
		startFeeding = false;
		// fishFeeding();
		
	}

	// // evening
	// if ((now.hour() == params.FEED_SECOND_H) && (now.minute() == params.FEED_SECOND_M) && (now.second() == 0))
	// {
	// 	startFeeding = false;
	// 	// fishFeeding();	
	
	// } 

	if (startFeeding == false)   // fish feeding MF!!!!!
	{
		fishFeeding();
	}



 


	


}    

/*
.########.##....##.########.
.##.......###...##.##.....##
.##.......####..##.##.....##
.######...##.##.##.##.....##
.##.......##..####.##.....##
.##.......##...###.##.....##
.########.##....##.########.
*/

