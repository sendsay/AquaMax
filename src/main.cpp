/*
Project: Aquarium controller
Autor: Shpakov Vlad (aka SendSay)
Date: 12.12.2019
Ver. 0.1a
*/

#include <Arduino.h>
#include "DallasTemperature.h"
#include "OneWire.h"
#include "RTClib.h"

/*
.########..########.########
.##.....##.##.......##......
.##.....##.##.......##......
.##.....##.######...######..
.##.....##.##.......##......
.##.....##.##.......##......
.########..########.##......
*/
#define FILTER_SAMPLES   7

#define RF			  2
#define pump2 	      3
#define pump1  		  4
#define buzzer 		  5
#define light  		  6
#define level1 		  7
#define level2		  8
#define dpump3 		  9
#define dpump2 		 10
#define dpump1 		 11
#define ONE_WIRE_BUS 12
#define feeder		 13
#define pHLevelPin   A1
#define ecLevelPin 	 A0
#define feederpos	 16
#define AnalogPin 	  3
#define RTC4		  4
#define RTC5		  5

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
.##.....##....###....########.
.##.....##...##.##...##.....##
.##.....##..##...##..##.....##
.##.....##.##.....##.########.
..##...##..#########.##...##..
...##.##...##.....##.##....##.
....###....##.....##.##.....##
*/
int status = 0;
bool startFeeding = true;           // startFeeding flag for feeding
int feed = 0;
int waterLevel = 0;           // water level
bool alarmNow = false;        // now have alarm
// bool feedingNow = false;      // now feeding
float temperature = 0;        // temperature from sensor
bool waterLevelFlag = false;	// for first signal
bool tempOverFlag = false;		// for fist signal
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/*
..######..########.########..##.....##..######..########..######.
.##....##....##....##.....##.##.....##.##....##....##....##....##
.##..........##....##.....##.##.....##.##..........##....##......
..######.....##....########..##.....##.##..........##.....######.
.......##....##....##...##...##.....##.##..........##..........##
.##....##....##....##....##..##.....##.##....##....##....##....##
..######.....##....##.....##..#######...######.....##.....######.
*/
struct Parameters 
{
const unsigned long ALARM_REPEAT = 1500000L;     			// in 15 minutes
	const int TEMP_MAX = 26;                                // max heating temp
	const int TEMP_MIN = 4;                                 // min heating temp 
	const int FEED_FIRST_H = 7;								// first feeding hour
	const int FEED_FIRST_M = 0;								// first feeding minute
	// const int FEED_SECOND_H = 10;							// first feeding hour
	// const int FEED_SECOND_M = 40;							// first feeding minute
	
};
Parameters params;

struct Alarmsignals 
{
	const byte WATER_LEVEL = 3;
	const byte TEMPERATURE = 4;
	const byte TEMPERATURELOW = 4;
	const byte WELCOME = 7;
	const byte END_OPERATION = 1;
	const byte PH_LEVEL = 5;
	const byte EC_LEVEL = 5;
	const byte START_FEEDING = 2;
};
Alarmsignals signal;

struct Timings
{
	unsigned long timing;         
	unsigned long timing2;
	unsigned long timing3;
	unsigned long timing4;
};
Timings timings;

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
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();    
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
	// if (millis() - timings.timing > 5000)
	// { 
	// 	timings.timing = millis(); 	 
		

	// 	// Serial.println(RTC.now());

	// }


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

