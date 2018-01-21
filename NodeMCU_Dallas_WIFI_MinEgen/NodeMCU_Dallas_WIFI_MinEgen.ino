#include <dummy.h>

/*
    Simple HTTP get webclient test
*/

#include <ESP8266WiFi.h>
/*
Man måste ha Original OneWire biblioteket till NodeMCU versionen.

Den kommer inte compilera eller funka med No-PullUp biblioteket.

C:\Users\Kenny\Documents\Arduino\libraries\OneWire (finns i zip-filen där om det är det andra biblioteket som används)
Läste på nätet att one-wire bibblan ska vra en speciell för ESP8266, kanske är det jag har? Den kompilerar i alla fall.

Om denna ligger där så kommer inte det funka för NodeMCU, No resistor är till för noresistor-programmet för "min temperaturmätare" osv
C:\Users\Kenny\Documents\Arduino\libraries\OneWireNoResistor-1.0 

Men jag har ingen pull-up resistor på min och det funkar ändå. Jag är inte helt säker på att det är den här koden som körs dock. Så testa inte på den som funkar.
Testa på en ny nodemcu ifall man verkligen slipper 4,7k resistorn.

Kolla den hittade laptopen och se vad för bibliotek du använt till one wire och hur koden ser ut.
*/
#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

const char* ssid     = "838812-NJTN-NY";
const char* password = "2katterientrea";

const char* host = "api.thingspeak.com";

void setup() {

  // This lines just make it so you can plug a DS18B20 directly into
  // digitial pins D4-D3.
  //  NodeMCU has weird pin mapping.
  // Pin numbers written on the board itself do not correspond to ESP8266 GPIO pin numbers. We have constants defined to make using this board easier:
  //
  //static const uint8_t D0   = 16;
  //static const uint8_t D1   = 5;
  //static const uint8_t D2   = 4;
  //static const uint8_t D3   = 0;
  //static const uint8_t D4   = 2;
  //static const uint8_t D5   = 14;
  //static const uint8_t D6   = 12;
  //static const uint8_t D7   = 13;
  //static const uint8_t D8   = 15;
  //static const uint8_t D9   = 3;
  //static const uint8_t D10  = 1;


  digitalWrite( 2 , LOW );
  pinMode( 2  , OUTPUT );
  digitalWrite( 0 , LOW );
  pinMode( 0 , OUTPUT );

  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  //DS TEMP
  int numberOfDevices; // Number of temperature devices found

  DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

  // For testing purposes, reset the bus every loop so we can see if any devices appear or fall off
  sensors.begin();

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  Serial.print(" Parasite:");
  if (sensors.isParasitePowerMode()) Serial.print("ON ");
  else Serial.print("OFF ");

  Serial.print("Count:");
  Serial.print(numberOfDevices, DEC);
  // report parasite power requirements

  sensors.requestTemperatures(); // Send the command to get temperatures
  //Temperaturen är innanför loopen, så lägger en variabel utanför loopen som sen skickas till thingspeak
  float tempToSend;
  // Loop through each device, print out temperature data
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      // Output the device ID
      Serial.print(" #");
      Serial.print(i, DEC);
      Serial.print("=");


      float tempC = sensors.getTempC(tempDeviceAddress);
      tempToSend = tempC;
      Serial.print(tempC); // Converts tempC to Fahrenheit

    }
    //else ghost device! Check your power requirements and cabling

  }
  //DS TEMP END

  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/update?api_key=7DCLU1WP1R4HRA0R&";
  //Här skickar vi datan med field1 , String(tempToSend(min float som jag gjort), 2 (med hur många decimaler vi ska skicka))
  String field1 = "field1=" + String(tempToSend, 2);
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + field1 + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(500);

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
}



// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
