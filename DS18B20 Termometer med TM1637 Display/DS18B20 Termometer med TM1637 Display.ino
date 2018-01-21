/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 9 on the Arduino 
#define ONE_WIRE_BUS 9


/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS);

/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/


//Displaydelen under här
/********************************************************************/

#include <TM1637Display.h>

const int CLK = 3; //Set the CLK pin connection to the display
const int DIO = 2; //Set the DIO pin connection to the display

int numCounter = 0;

TM1637Display display(CLK, DIO); //set up the 4-Digit Display.
/********************************************************************/

void setup()
{
	//Display
	display.setBrightness(7);

	// start serial port 
	Serial.begin(9600);
	
	// Start up the library 
	sensors.begin();
}

void loop()
{
	//Get temeratures for all devices
	sensors.requestTemperatures();
	
	Serial.print("Temperature is: ");
	Serial.print(sensors.getTempCByIndex(0));
	Serial.println();
	// Why "byIndex"?  
	// You can have more than one DS18B20 on the same bus.  
	// 0 refers to the first IC on the wire

	//Visa temperaturen på TM1637 display genom att anropa min metod.
	printTempOnDisplay(sensors.getTempCByIndex(0));
	
	delay(1000);
}

void printTempOnDisplay(float temperatureC)
{
	//temperatureC * 100, man tar alltså gånger 100 för att få det i tusental 
	//istället för att ha decimaler så har vi nu en int som är med 
	//"decimaler" 26,25 * 100 = 2625 => 26:25
	int floatTempCtoInt = temperatureC * 100;

	// B11100011 Sätt på båda punkterna
	byte rawData;
	rawData = B11100011;

	//går också att stoppa in B11100011 bara rakt av
	display.showNumberDecEx(floatTempCtoInt, rawData);
}