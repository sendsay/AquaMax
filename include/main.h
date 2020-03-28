/*
Project: Aquarium controller
Autor: Shpakov Vlad (aka SendSay)
Date: 12.12.2019
Ver. 0.1a
*/

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
#define feederpos	 16
#define AnalogPin 	  3
#define RTC4		  4
#define RTC5		  5
#define PhMax		  8.5
#define PhMin		  6.5



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


float tdsValue = 0;
#define TdsSensorPin A1

boolean pHOverFlag = false; 

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
const unsigned long ALARM_REPEAT = 15000L;     			// in 15 minutes
	const int TEMP_MAX = 26;                                // max heating temp
	const int TEMP_MIN = 4;                                 // min heating temp 
	const int FEED_FIRST_H = 10;								// first feeding hour
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
