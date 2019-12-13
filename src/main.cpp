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
RTC_DS1307 RTC;                 // Clock
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
void alarm(int count);            // Alram signal
void fishFeeding();               // Feeding fish
void printTime(DateTime now);     // Print time and date

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
bool start = false;           // start flag for feeding
int feed = 0;
int waterLevel = 0;           // water level
bool alarmNow = false;        // now have alarm
bool feedingNow = false;      // now feeding
float temperature = 0;        // temperature from sensor
bool waterLevelFlag = false;	// for first signal
bool tempOverFlag = false;		// for fist signal

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
	const unsigned long ALARM_REPEAT = 60000;     // in minutes
	const int TEMP_MAX = 15;                                // max heating temp
	const int TEMP_MIN = 5;                                 // min heating temp 
	const int FEED_FIRST_H = 7;								// first feeding hour
	const int FEED_FIRST_M = 0;								// first feeding minute
	const int FEED_SECOND_H = 19;							// first feeding hour
	const int FEED_SECOND_M = 0;							// first feeding minute
	
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
	if (not alarmNow) {

		if (start == false) {
			
			Serial.println("Fish feeding!");
			
			alarm(signal.START_FEEDING);

			digitalWrite(feeder,HIGH);  //start
			start = true;

			// while (!(digitalRead(feederpos) == HIGH)) {}   
			delay(2000);      
			while (!(digitalRead(feederpos) == LOW) && (start == true)) {
			//Wait until rotate
			}
			
			digitalWrite(feeder,LOW);   //stop
			start = false;  

			alarm(signal.END_OPERATION);
		}
	feedingNow = false;
	}
}

void alarm(int count) 
{
// if ((not feedingNow) || (not overTemp)) {	
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
// }
}

void printTime(DateTime now) {
	Serial.print(now.month(), DEC);
    Serial.print("/");
    
	Serial.print(now.day(), DEC);
    Serial.print("/");

    Serial.print(now.year(), DEC);
    Serial.print(" ");
	

	// // Friendly printout the weekday
    //     switch (now.dayOfWeek()) 
    // {
	// 	case 1:
	// 	  Serial.print("MON");
	// 	  break;
	// 	case 2:
	// 	  Serial.print("TUE");
	// 	  break;
	// 	case 3:
	// 	  Serial.print("WED");
	// 	  break;
	// 	case 4:
	// 	  Serial.print("THU");
	// 	  break;
	// 	case 5:
	// 	  Serial.print("FRI");
	// 	  break;
	// 	case 6:
	// 	  Serial.print("SAT");
	// 	  break;
	// 	case 7:
	// 	  Serial.print("SUN");
	// 	  break;
	// }
	// Serial.print(" ");
	   
    if(now.hour()<10){
	switch (now.hour()) 
    {
		case 0:
		  Serial.print("00");
		  break;
		case 1:
		  Serial.print("01");
		  break;
		case 2:
		  Serial.print("02");
		  break;
		case 3:
		  Serial.print("03");
		  break;
		case 4:
		  Serial.print("04");
		  break;
		case 5:
		  Serial.print("05");
		  break;
		case 6:
		  Serial.print("06");
		  break;
		case 7:
		  Serial.print("07");
		  break;
		case 8:
		  Serial.print("08");
		  break;
		case 9:
		  Serial.print("09");
		  break;	 
	}
	}
	else{
	Serial.print(now.hour(), DEC);
	}

    Serial.print(":");
   
    if(now.minute()<10){
	switch (now.minute()) 
    {
		case 0:
		  Serial.print("00");
		  break;
		case 1:
		  Serial.print("01");
		  break;
		case 2:
		  Serial.print("02");
		  break;
		case 3:
		  Serial.print("03");
		  break;
		case 4:
		  Serial.print("04");
		  break;
		case 5:
		  Serial.print("05");
		  break;
		case 6:
		  Serial.print("06");
		  break;
		case 7:
		  Serial.print("07");
		  break;
		case 8:
		  Serial.print("08");
		  break;
		case 9:
		  Serial.print("09");
		  break;
		 
	}
	}
	else{
	Serial.print(now.minute(), DEC);
	}

    Serial.print(":");
 
    if(now.second()<10){
	switch (now.second()) 
    {
		case 0:
		  Serial.print("00");
		  break;
		case 1:
		  Serial.print("01");
		  break;
		case 2:
		  Serial.print("02");
		  break;
		case 3:
		  Serial.print("03");
		  break;
		case 4:
		  Serial.print("04");
		  break;
		case 5:
		  Serial.print("05");
		  break;
		case 6:
		  Serial.print("06");
		  break;
		case 7:
		  Serial.print("07");
		  break;
		case 8:
		  Serial.print("08");
		  break;
		case 9:
		  Serial.print("09");
		  break;
		 
	}
	}
	else{
	Serial.print(now.second(), DEC);
	}

    Serial.println("");
    
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

	RTC.begin();

	RTC.adjust(DateTime(__DATE__, __TIME__));

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
//*************************
	// if (millis() - timings.timing > 60000)
	// { 
	// 	timings.timing = millis(); 
	// 	feedingNow = true;          // for remember feeding
	// }
	// //************************
	// if (feedingNow) 
	// {             // fish feeding
	// 	fishFeeding();
	// }


//*** check water level
	waterLevel = digitalRead(level1);      
	if (waterLevel == HIGH) 
	{
		if (waterLevelFlag == false) {		// signal
			Serial.println("WaterLevel low!");
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
			Serial.println("Temperature over limit!");
			Serial.println(temperature);
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
	if ((now.hour() == params.FEED_FIRST_H) && (now.minute() == params.FEED_FIRST_M))
	{
		fishFeeding();
	}
	//night
	if ((now.hour() == params.FEED_SECOND_H) && (now.minute() == params.FEED_SECOND_M))
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

