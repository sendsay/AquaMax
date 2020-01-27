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
// #define pHLevelPin   A1
// #define ecLevelPin 	 A0
#define feederpos	 16
#define AnalogPin 	  3
#define RTC4		  4
#define RTC5		  5

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

// Ph meter
float calibration = 8.90; //change this value to calibrate
const int analogInPin = A0; 
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],temp;

float tdsValue = 0;
#define TdsSensorPin A1
// #define VREF 5.0      // analog reference voltage(Volt) of the ADC
// #define SCOUNT  30           // sum of sample point
// int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
// int analogBufferTemp[SCOUNT];
// int analogBufferIndex = 0,copyIndex = 0;
// float averageVoltage = 0;
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
