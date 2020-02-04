/*
Project: Aquarium controller
Autor: Shpakov Vlad (aka SendSay)
Date: 12.12.2019
Ver. 0.1a

For convert uS to PPM divide uS on 1.56
For convert PPM to uS mult PPM on 1.56
*/

#include <Arduino.h>
#include <main.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <RTClib.h>
#include <GravityTDS.h>

#define SensorPin A2            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.4          //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;
static float pHValue,voltage;
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
void printTime();     	// 	Print time and date
// void prepareToFeeding();			//	Prepare function fishFeeding
double avergearray(int* arr, int number);  //average pH

/*
..#######..########........##.########..######..########..######.
.##.....##.##.....##.......##.##.......##....##....##....##....##
.##.....##.##.....##.......##.##.......##..........##....##......
.##.....##.########........##.######...##..........##.....######.
.##.....##.##.....##.##....##.##.......##..........##..........##
.##.....##.##.....##.##....##.##.......##....##....##....##....##
..#######..########...######..########..######.....##.....######.
*/
OneWire oneWire(ONE_WIRE_BUS); 			// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);	// Pass our oneWire reference to Dallas Temperature. 
RTC_DS1307 rtc;                 		// Clock
DateTime now;                 			// Now date time
GravityTDS gravityTds;					// Tds sensor

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
    gravityTds.setAref(3.3);  			//reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  		//1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  				//initialization

	Serial.begin(9600);
	Serial.println("");
	Serial.println("*** Welcome to fish controller! ***");

	rtc.begin();						// start clock

//	 rtc.adjust(DateTime(__DATE__, __TIME__));    // adjust time

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
	if (millis() - timings.timing > 2000)
	{ 
		timings.timing = millis(); 	 

		gravityTds.setTemperature(temperature);  	//set the temperature and execute temperature compensation
		gravityTds.update(); 						//sample and calculate 
		tdsValue = gravityTds.getTdsValue();  		// then get the value
		
		Serial.print("TDS: ");
		Serial.print(tdsValue, 0);		
		Serial.print(" ppm   ");	
		Serial.print(tdsValue * 1.56, 0);
		Serial.println(" mS");

    	Serial.print("Voltage:");
        Serial.print(voltage,2);
        Serial.print("    pH value: ");
    	Serial.println(pHValue,2);

		Serial.print("Temp: ");
		Serial.print(temperature);
		Serial.println("^C");


		Serial.println("*****************************************");

	}

//*** check Ph data
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
  }
//   if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
//   {
//     Serial.print("Voltage:");
//         Serial.print(voltage,2);
//         Serial.print("    pH value: ");
//     		Serial.println(pHValue,2);
//         digitalWrite(LED,digitalRead(LED)^1);
//         printTime=millis();
//   }


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

//***Check Tds
	if (phValue >= PhMax or phValue =<)
	{
		/* code */
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
	if (not alarmNow) 
	{
		printTime();							
		alarmNow = true;
		for (int i = 0; i < count; i++)
		{
			digitalWrite (buzzer,HIGH);
			delay(100);
			digitalWrite (buzzer,LOW);
			delay(100);
		}
		delay(500);
		alarmNow = false;
	}
}

void printTime() 
{
	now = rtc.now();			// get time;
	Serial.print(now.year());
    Serial.print('/');
    Serial.print(now.month());
    Serial.print('/');
    Serial.print(now.day(), DEC);
	Serial.print("(");
	Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
	Serial.print(")");
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

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
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

