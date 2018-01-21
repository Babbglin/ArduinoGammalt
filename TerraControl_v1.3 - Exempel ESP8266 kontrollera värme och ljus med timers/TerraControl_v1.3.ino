/*------------------ LIBRARIES ------------------*/
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include "setting.h"

/*------------------ DEFINE DHT SENSOR PIN ------------------*/
#define SENSOR_IN 15 //D8
#define SENSOR_IN_Type DHT22

/*------------------ DEFINE RELAY PINS ------------------*/
#define relayLight 5 //D1
#define relayHeat 4 //D2
#define relayFan 12 //D6
#define relayHeat2 14 //D5

/*------------------ OTHER VARIABLES ------------------*/
float lightOn = (60 * lightOn_hour) + lightOn_min;
float lightOff = (60 * lightOff_hour) + lightOff_min;
float currentTime, lowTemp = 0, highTemp = 0, lowHum = 0, highHum = 0;
float tempStore[24] = {};
float humStore[24] = {};
int hourStore[24] = {};

int i = 0;
boolean lightVal, heatVal, fanVal, heat2Val;
float humIN, tempIN, humOUT, tempOUT;

DHT dht_IN(SENSOR_IN, SENSOR_IN_Type);

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);

/*------------------ NTP SERVERS ------------------*/
//IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

WiFiUDP Udp;
unsigned int localPort = 3232;  // Local port to listen for UDP packets
WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  MDNS.begin(host);

  /*------------------ DEFINE YOUR SERVER ------------------*/

  /*--- UNCOMMENT THIS FOR MANUAL SETTING --
  IPAddress ip(192, 168, 0, 111); //NodeMCU static IP
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  */

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");
  server.begin();
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);

  Serial.print("TerraControl URL: http://");
  Serial.print(host);
  Serial.println(".local");
  Serial.print("TerraControl IP: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  Serial.print("Local gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Local subnet: ");
  Serial.println(WiFi.subnetMask());

  pinMode(relayLight, OUTPUT);
  pinMode(relayHeat, OUTPUT);
  pinMode(relayFan, OUTPUT);
  pinMode(relayHeat2, OUTPUT);

  digitalWrite(relayHeat, 1);
  digitalWrite(relayHeat2, 1);
  digitalWrite(relayFan, 1);
}

/*------------------ FUNCTION FOR MINIMUM OR MAXIMUM VALUE IN ARRAY ------------------*/
float getMinMax(float* array, int size, boolean minMax)
{
  if (minMax == 0) {
    float minimum = array[0];
    for (int p = 0; p < size; p++)
    {
      if (array[p] < minimum) minimum = array[p];
    }
    return minimum;
  }
  else {
    float maximum = array[0];
    for (int p = 0; p < size; p++)
    {
      if (array[p] > maximum) maximum = array[p];
    }
    return maximum;
  }
}

/*------------------ OTHER FUNCTIONS ------------------*/
String holStatus(float val, float valMin, float valMax)
{ String statusVal;
  if (val > valMax) {
    statusVal = "HIGH";
  }
  else if (val < valMin) {
    statusVal = "LOW";
  }
  else {
    statusVal = "OK";
  }
  return statusVal;
}

String ooStatus(boolean val)
{ String oStatus;
  if (val == 1) {
    oStatus = "ON";
  }
  else {
    oStatus = "OFF";
  }
  return oStatus;
}

String ooStatusRev(boolean val)
{
  String oStatusRev;
  if (val == 1) {
    oStatusRev = "OFF";
  }
  else {
    oStatusRev = "ON";
  }
  return oStatusRev;
}

String printMinutes (int minutes)
{ String minPrint;
  if (minutes < 10) {
    minPrint = "0" + String(minutes);
  }
  else {
    minPrint = minutes;
  }
  return minPrint;
}

void loop()
{
  WiFiClient client = server.available();
  currentTime = (60 * int(hour())) + int(minute());

  /*------------------ READING TEMP AND HUM VALUES FROM DHT SENSOR ------------------*/
  tempIN = dht_IN.readTemperature();
  humIN = dht_IN.readHumidity();

  /*------------------ READING STATES OF RELAYS ------------------*/
  lightVal = digitalRead(relayLight);
  heatVal = digitalRead(relayHeat);
  heat2Val = digitalRead(relayHeat2);
  fanVal = digitalRead(relayFan);

  /*------------------ STORING TEMP AND HUM VALUES INTO ARRAYS ------------------*/
    if (int(minute()) == 0 && int(second()) == 0) {
  //if (int(second()) == 0) {
    //until the array is full
    if (i < 24) {
      tempStore[i] = tempIN;
      humStore[i] = humIN;
      hourStore[i] = int(hour());
      i++;
    }
    //when the array is filled, move new values to the end
    else {
      for (int k = 0; k < 23; k++) {
        tempStore[k] = tempStore[k + 1];
        humStore[k] = humStore[k + 1];
        hourStore[k] = hourStore[k + 1];
      }
      tempStore[23] = tempIN;
      humStore[23] = humIN;
      hourStore[23] = int(hour());
    }
    lowTemp = getMinMax(tempStore, 24, 0);
    highTemp = getMinMax(tempStore, 24, 1);
    lowHum = getMinMax(humStore, 24, 0);
    highHum = getMinMax(humStore, 24, 1);
  }

  /*------------------ READING THE REQUEST ------------------*/
  String request = client.readStringUntil('\r');
  client.flush();

  /*------------------ MATCHING THE REQUEST ------------------*/
  if (request.indexOf("/HOUR_ON_UP") != -1)  {
    if (lightOn_hour == 23) {
      lightOn_hour = -1;
    }
    lightOn_hour++;
    lightOn = (60 * lightOn_hour) + lightOn_min;
  }

  if (request.indexOf("/HOUR_ON_DOWN") != -1)  {
    if (lightOn_hour == 0) {
      lightOn_hour = 24;
    }
    lightOn_hour--;
    lightOn = (60 * lightOn_hour) + lightOn_min;
  }

  if (request.indexOf("/MIN_ON_UP") != -1)  {
    if (lightOn_min == 55) {
      lightOn_min = -5;
    }
    lightOn_min = lightOn_min + 5;
    lightOn = (60 * lightOn_hour) + lightOn_min;
  }

  if (request.indexOf("/MIN_ON_DOWN") != -1)  {
    if (lightOn_min == 0) {
      lightOn_min = 60;
    }
    lightOn_min = lightOn_min - 5;
    lightOn = (60 * lightOn_hour) + lightOn_min;
  }

  if (request.indexOf("/HOUR_OFF_UP") != -1)  {
    if (lightOff_hour == 23) {
      lightOff_hour = -1;
    }
    lightOff_hour++;
    lightOff = (60 * lightOff_hour) + lightOff_min;
  }

  if (request.indexOf("/HOUR_OFF_DOWN") != -1)  {
    if (lightOff_hour == 0) {
      lightOff_hour = 24;
    }
    lightOff_hour--;
    lightOff = (60 * lightOff_hour) + lightOff_min;
  }

  if (request.indexOf("/MIN_OFF_UP") != -1)  {
    if (lightOff_min == 55) {
      lightOff_min = -5;
    }
    lightOff_min = lightOff_min + 5;
    lightOff = (60 * lightOff_hour) + lightOff_min;
  }

  if (request.indexOf("/MIN_OFF_DOWN") != -1)  {
    if (lightOff_min == 0) {
      lightOff_min = 60;
    }
    lightOff_min = lightOff_min - 5;
    lightOff = (60 * lightOff_hour) + lightOff_min;
  }

  if (request.indexOf("/TEMP_ON_UP") != -1)  {
    tempMin = tempMin + 0.5;
  }

  if (request.indexOf("/TEMP_ON_DOWN") != -1)  {
    tempMin = tempMin - 0.5;
  }

  if (request.indexOf("/TEMP_OFF_UP") != -1)  {
    tempMax = tempMax + 0.5;
  }

  if (request.indexOf("/TEMP_OFF_DOWN") != -1)  {
    tempMax = tempMax - 0.5;
  }

  if (request.indexOf("/FAN_SWITCH") != -1)  {
    fanVal = !fanVal;
    digitalWrite(relayFan, fanVal);
  }

  if (request.indexOf("/HEAT2_SWITCH") != -1)  {
    heat2Val = !heat2Val;
    digitalWrite(relayHeat2, heat2Val);
  }

  if (request.indexOf("/TIMEZONE_SWITCH") != -1)  {
    if (timeZone == 1) {
      timeZone = 2;
      setSyncProvider(getNtpTime);
    }
    else {
      timeZone = 1;
      setSyncProvider(getNtpTime);
    }
  }

  /*------------------ CHECKING TEMPERATURE ------------------*/
  if (tempIN < tempMin)
  {
    if (heatVal == 0)
    {
      digitalWrite(relayHeat, 1);
      heatVal = 1;
    }
  }
  else if (tempIN >= tempMax)
  {
    if (heatVal == 1)
    {
      digitalWrite(relayHeat, 0);
      heatVal = 0;
    }
  }

  /*------------------ LIGHT TIMER ------------------*/
  if (lightOn < lightOff) {
    if (lightOn <= currentTime < lightOff)
    {
      if (lightVal == 0)
      {
        digitalWrite(relayLight, 1);
        lightVal = 1;
      }
    }
    if (lightOn > currentTime || lightOff <= currentTime)
    {
      if (lightVal == 1)
      {
        digitalWrite(relayLight, 0);
        lightVal = 0;
      }
    }
  }
  else if (lightOn > lightOff) {
    if (lightOn >= currentTime >= lightOff)
    {
      if (lightVal == 1)
      {
        digitalWrite(relayLight, 0);
        lightVal = 0;
      }
    }
    if (lightOn < currentTime || lightOff > currentTime)
    {
      if (lightVal == 0)
      {
        digitalWrite(relayLight, 1);
        lightVal = 1;
      }
    }
  }

  /****--------------- HTML CODE ---------------****/
  client.println("<!doctype html>");
  client.println("<html lang='en'>");
  client.println("<head>");
  client.println("<meta charset='utf-8'>");
  client.print("<title>TerraControl v");
  client.print(tCVersion);
  client.println("</title>");
  client.print("<meta name='WiFi controlled terrarium monitoring' content='TerraControl v");
  client.print(tCVersion);
  client.println("'>");
  client.println("<!--[if lt IE 9]>");
  client.println("<script src='http://html5shiv.googlecode.com/svn/trunk/html5.js'></script>");
  client.println("<![endif]-->");
  client.println("<style>");
  client.println("body {font-family: Arial, Helvetica, sans-serif;background: #4f5055;}");
  client.println(".container {position: relative;margin: 3% 10% 3% 10%;width: 1000px;height: 600px;background: white;background: -webkit-linear-gradient(grey, #420000);background: -o-linear-gradient(grey, #420000);background: -moz-linear-gradient(grey, #420000);background: linear-gradient(grey, #420000);box-shadow: 10px 10px 10px 0 black;}");
  client.println(".header, .light, .heat, .heat2, .fan, .timezone, .tempGraph, .humGraph {position: absolute;background: black;background: -webkit-linear-gradient(grey, orange, grey);background: -o-linear-gradient(grey, orange, grey);background: -moz-linear-gradient(grey, orange, grey);background: linear-gradient(grey, orange, grey);");
  client.println("box-shadow: 5px 5px 15px 0 black;}");
  client.println(".header {margin: 20px 0 0 20px;width: 550px;height: 130px;}");
  client.println(".headerState {position: absolute;width:360px;height:130px;}");
  client.println(".headerStatus {position: absolute;margin:5px 0 0 390px;width:190px;height:130px;}");
  client.println(".light {margin: 170px 0 0 20px;width: 350px;height: 195px;}");
  client.println(".heat {position: absolute;margin: 385px 0 0 20px;width: 350px;height: 195px;}");
  client.println(".heat2 {margin: 170px 0 0 390px;width: 180px;height: 125px;}");
  client.println(".fan {margin: 315px 0 0 390px;width: 180px;height: 130px;}");
  client.println(".timezone {margin: 465px 0 0 390px;width: 180px;height: 115px;}");
  client.println(".tempGraph {position: absolute;margin: 20px 0 0 590px;width: 390px;height: 270px;}");
  client.println(".humGraph {margin: 310px 0 0 590px;width: 390px;height: 270px;}");
  client.println(".graph {position: absolute;margin: 20px 0 0 15px;width: 360px;height: 200px;background: grey;box-shadow: 5px 5px 15px 0 black;}");
  client.println(".title {padding:5px 0px 0px 0px;text-shadow: 1px 1px 3px #990000;font-weight: bold;font-size: 28px;text-align:center;}");
  client.println(".title2 {padding:5px 0px 0px 0px;text-shadow: 1px 1px 3px #990000;font-weight: bold;font-size: 22px;text-align:center;}");
  client.println(".buttonUp, .buttonDown {width: 0; height: 0; border-left: 20px solid transparent;border-right: 20px solid transparent;}");
  client.println(".buttonUp {border-bottom: 20px solid #990000;}");
  client.println(".buttonUp:hover {border-bottom: 20px solid red;}");
  client.println(".buttonDown {border-top: 20px solid #990000;}");
  client.println(".buttonDown:hover {border-top: 20px solid red;}");
  client.println(".buttonSwitch {background: #990000;background: -webkit-linear-gradient(grey, #990000);background: -o-linear-gradient(grey, #990000);background: -moz-linear-gradient(grey, #990000);background: linear-gradient(grey, #990000);width: 80px;height: 30px;color: black;text-align:center;font-size: 20px;font-weight: bold;padding-top: 6px; box-shadow: 3px 3px 10px 0 black;}");
  client.println(".buttonSwitch:hover {background: grey;  background: -webkit-linear-gradient(#990000, grey); background: -o-linear-gradient(#990000, grey); background: -moz-linear-gradient(#990000, grey);background: linear-gradient(#990000, grey);}");
  client.println("a:link {text-decoration: none; color:grey;}");
  client.println("</style></head><body>");
  client.println("<div class='container'>");

  /*------------------ HEADER & STATUS------------------*/
  client.println("<div class='header'>");
  client.println("<div class='headerState'>");
  client.print("<div class='title'>TerraControl v");
  client.print(tCVersion);
  client.println("</div>");
  client.print("<div style='position:absolute;margin: 0 0 0 160px'>");
  client.print(hour());
  client.print(":");
  client.print(printMinutes(minute()));
  client.println("</div>");
  client.println("<div style='position:absolute;margin: 20px 0 0 30px'>temperature</div>");
  client.println("<div style='position:absolute;margin: 20px 0 0 270px'>humidity</div><br>");
  client.print("<div style='position:absolute;margin: 30px 0 0 40px'>");
  client.print(tempIN);
  client.print("&#8451;");
  client.println("</div>");
  client.print("<div style='position:absolute;margin: 30px 0 0 275px'>");
  client.print(humIN);
  client.print("%");
  client.println("</div>");
  client.println("</div>");
  client.println("<div class='headerStatus'>");
  client.print("<div>Temperature: ");
  client.print(holStatus(tempIN, tempMin, tempMax));
  client.println("</div>");
  client.print("<div>Humidity: ");
  client.print(holStatus(humIN, humMin, humMax));
  client.println("</div>");
  client.print("<div>Light:  ");
  client.print(ooStatus(lightVal));
  client.println("</div>");
  client.print("<div>Heat:  ");
  client.print(ooStatus(heatVal));
  client.println("</div>");
  client.print("<div>Heat 2: ");
  client.print(ooStatusRev(heat2Val));
  client.println("</div>");
  client.print("<div>Fan: ");
  client.print(ooStatusRev(fanVal));
  client.println("</div></div></div>");

  /*------------------ LIGHT CONTROL ------------------*/
  client.println("<div class='light'>");
  client.println("<div class='title2'>Light</div>");
  client.println("<div style='position:absolute;margin: 10px 0 0 10px'>Status</div>");
  client.println("<div style='position:absolute;margin: 10px 0 0 125px'>ON Time</div>");
  client.println("<div style='position:absolute;margin: 10px 0 0 265px'>OFF Time</div>");
  client.println("<div style='position:absolute;margin: 30px 0 0 115px'><a href=\'/HOUR_ON_UP\'\'><div class='buttonUp'></div></a></div>");
  client.println("<div style='position:absolute;margin: 30px 0 0 160px'><a href=\'/MIN_ON_UP\'\'><div class='buttonUp'></div></a></div>");
  client.println("<div style='position:absolute;margin: 30px 0 0 260px'><a href=\'/HOUR_OFF_UP\'\'><div class='buttonUp'></div></a></div>");
  client.println("<div style='position:absolute;margin: 30px 0 0 305px'><a href=\'/MIN_OFF_UP\'\'><div class='buttonUp'></div></a></div>");
  client.println("<div style='position:absolute;margin: 45px 0 0 10px'><h2>");
  client.print(ooStatus(lightVal));
  client.print("</h2></div>");
  client.print("<div style='position:absolute;margin: 45px 0 0 120px'><h2>");
  client.print(lightOn_hour);
  client.println("</h2></div>");
  client.println("<div style='position:absolute;margin: 45px 0 0 152px'><h2>:</h2></div>");
  client.print("<div style='position:absolute;margin: 45px 0 0 165px'><h2>");
  client.print(printMinutes(lightOn_min));
  client.println("</h2></div>");
  client.print("<div style='position:absolute;margin: 45px 0 0 263px'><h2>");
  client.print(lightOff_hour);
  client.println("</h2></div>");
  client.println("<div style='position:absolute;margin: 45px 0 0 298px'><h2>:</h2></div>");
  client.print("<div style='position:absolute;margin: 45px 0 0 313px'><h2>");
  client.print(printMinutes(lightOff_min));
  client.println("</h2></div>");
  client.println("<div style='position:absolute;margin: 105px 0 0 115px'><a href=\'/HOUR_ON_DOWN\'\'><div class='buttonDown'></div></a></div>");
  client.println("<div style='position:absolute;margin: 105px 0 0 160px'><a href=\'/MIN_ON_DOWN\'\'><div class='buttonDown'></div></a></div>");
  client.println("<div style='position:absolute;margin: 105px 0 0 260px'><a href=\'/HOUR_OFF_DOWN\'\'><div class='buttonDown'></div></a></div>");
  client.println("<div style='position:absolute;margin: 105px 0 0 305px'><a href=\'/MIN_OFF_DOWN\'\'><div class='buttonDown'></div></a></div>");
  client.println("</div>");

  /*------------------ HEAT CONTROL ------------------*/
  client.println("<div class='heat'>");
  client.println("<div class='title2'>Heat</div>");
  client.println("<div style='position:absolute;margin: 10px 0 0 10px'>Status</div>");
  client.println("<div style='position:absolute;margin: 10px 0 0 125px'>ON Temp.</div>");
  client.println("<div style='position:absolute;margin: 10px 0 0 255px'>OFF Temp.</div>");
  client.println("<div style='position:absolute;margin: 30px 0 0 140px'><a href=\'/TEMP_ON_UP\'\'><div class='buttonUp'></div></a></div>");
  client.println("<div style='position:absolute;margin: 30px 0 0 270px'><a href=\'/TEMP_OFF_UP\'\'><div class='buttonUp'></div></a></div>");
  client.print("<div style='position:absolute;margin: 45px 0 0 10px'><h2>");
  client.print(ooStatus(heatVal));
  client.println("</h2></div>");
  client.print("<div style='position:absolute;margin: 45px 0 0 130px'><h2>");
  client.print(tempMin);
  client.println("</h2></div>");
  client.println("<div style='position:absolute;margin: 41px 0 0 190px'><h2>&#8451;</h2></div>");
  client.print("<div style='position:absolute;margin: 45px 0 0 260px'><h2>");
  client.print(tempMax);
  client.println("</h2></div>");
  client.println("<div style='position:absolute;margin: 41px 0 0 320px'><h2>&#8451;</h2></div>");
  client.println("<div style='position:absolute;margin: 105px 0 0 140px'><a href=\'/TEMP_ON_DOWN\'\'><div class='buttonDown'></div></a></div>");
  client.println("<div style='position:absolute;margin: 105px 0 0 270px'><a href=\'/TEMP_OFF_DOWN\'\'><div class='buttonDown'></div></a></div>");
  client.println("</div>");

  /*------------------ HEAT 2 CONTROL ------------------*/
  client.println("<div class='heat2'>");
  client.println("<div class='title2'>Heat 2</div>");
  client.print("<div style='position:absolute;margin: 20px 0 0 20px'><h2>");
  client.print(ooStatusRev(heat2Val));
  client.println("</h2></div>");
  client.println("<div style='position:absolute;margin: 35px 0 0 85px'><a href=\'/HEAT2_SWITCH\'\'><div class='buttonSwitch'>Switch</div></a></div>");
  client.println("</div>");

  /*------------------ FAN CONTROL ------------------*/
  client.println("<div class='fan'>");
  client.println("<div class='title2'>Fan</div>");
  client.print("<div style='position:absolute;margin: 20px 0 0 20px'><h2>");
  client.print(ooStatusRev(fanVal));
  client.println("</h2></div>");
  client.println("<div style='position:absolute;margin: 35px 0 0 85px'><a href=\'/FAN_SWITCH\'\'><div class='buttonSwitch'>Switch</div></a></div>");
  client.println("</div>");

  /*------------------ TIMEZONE CONTROL ------------------*/
  client.println("<div class='timezone'>");
  client.println("<div class='title2'>Timezone</div>");
  client.println("<div style='position:absolute;margin: 0 0 0 70px'>(GMT)</div>");
  client.print("<div style='position:absolute;margin: 10px 0 0 20px'><h2>+");
  client.print(timeZone);
  client.println("</h2></div>");
  client.println("<div style='position:absolute;margin: 25px 0 0 85px'><a href=\'/TIMEZONE_SWITCH\'\'><div class='buttonSwitch'>Switch</div></a></div>");
  client.println("</div>");

  /*------------------ TEMPERATURE GRAPH ------------------*/
  client.println("<div class='tempGraph'>");
  client.println("<div class='title2'>Temperature</div>");
  client.println("<div style='position:absolute;margin: -30px 0 0 20px'>min</div>");
  client.print("<div style='position:absolute;margin: -10px 0 0 15px'>");
  client.print(lowTemp);
  client.println("&#8451;</div>");
  client.println("<div style='position:absolute;margin: 0 0 0 130px'>(over last 24 hours)</div>");
  client.println("<div style='position:absolute;margin: -30px 0 0 340px'>max</div>");
  client.print("<div style='position:absolute;margin: -10px 0 0 330px'>");
  client.print(highTemp);
  client.println("&#8451;</div>");
  client.println("<div class='graph'>");
  client.println("<svg width='350' height='270' background='grey'>");
  client.println("<line x1='10' y1='10' x2='10' y2='190' style='stroke:black;stroke-width:2' />");
  client.println("<line x1='10' y1='190' x2='350' y2='190' style='stroke:black;stroke-width:2' />");

  long int x = 10;
  for (int y = 0; y < i  ; y++) {
    client.print("<circle id='circleTemp");
    client.print(y);
    client.print("' cx='");
    client.print(x);
    client.print("' cy='");
    client.print(330 - 10 * (tempStore[y]));
    client.println("' r='4' fill='#420000' />");
    client.print("<text id='tempPopup' x='");
    client.print(x - 5);
    client.print("' y='");
    client.print(320 - 10 * (tempStore[y]));
    client.print("' font-size='10' fill='yellow' visibility='hidden'>");
    client.print(hourStore[y]);
    client.print("h/");
    client.print(tempStore[y]);
    client.print("&#8451;<set attributeName='visibility' from='hidden' to='visible' begin='circleTemp");
    client.print(y);
    client.print(".mouseover' end='circleTemp");
    client.print(y);
    client.println(".mouseout'/></text>");
    if (y < 23 && y < (i - 1)) {
      client.print("<line x1='");
      client.print(x);
      client.print("' y1='");
      client.print(330 - 10 * (tempStore[y]));
      client.print("' x2='");
      client.print(x + 14);
      client.print("' y2='");
      client.print(330 - 10 * (tempStore[y + 1]));
      client.println("' style='stroke:#420000;stroke-width:2' />");
    }
    x = x + 14;
  }
  client.println("</svg></div></div>");

  /*------------------ HUMIDITY GRAPH ------------------*/
  client.println("<div class='humGraph'>");
  client.println("<div class='title2'>Humidity</div>");
  client.println("<div style='position:absolute;margin: -30px 0 0 20px'>min</div>");
  client.print("<div style='position:absolute;margin: -10px 0 0 15px'>");
  client.print(lowHum);
  client.println("%</div>");
  client.println("<div style='position:absolute;margin: 0 0 0 130px'>(over last 24 hours)</div>");
  client.println("<div style='position:absolute;margin: -30px 0 0 340px'>max</div>");
  client.print("<div style='position:absolute;margin: -10px 0 0 330px'>");
  client.print(highHum);
  client.println("%</div>");
  client.println("<div class='graph'>");
  client.println("<svg width='350' height='270' background='grey'>");
  client.println("<line x1='10' y1='10' x2='10' y2='190' style='stroke:black;stroke-width:2' />");
  client.println("<line x1='10' y1='190' x2='350' y2='190' style='stroke:black;stroke-width:2' />");

  x = 10;
  for (int y = 0; y < i; y++) {
    client.print("<circle id='circleHum");
    client.print(y);
    client.print("' cx='");
    client.print(x);
    client.print("' cy='");
    client.print(200 - 2 * (humStore[y]));
    client.println("' r='4' fill='#420000' />");
    client.print("<text id='humPopup' x='");
    client.print(x - 5);
    client.print("' y='");
    client.print(190 - 2 * (humStore[y]));
    client.print("' font-size='10' fill='yellow' visibility='hidden'>");
    client.print(hourStore[y]);
    client.print("h/");
    client.print(humStore[y]);
    client.print("%<set attributeName='visibility' from='hidden' to='visible' begin='circleHum");
    client.print(y);
    client.print(".mouseover' end='circleHum");
    client.print(y);
    client.println(".mouseout'/></text>");
    if (y < 23 && y < (i - 1)) {
      client.print("<line x1='");
      client.print(x);
      client.print("' y1='");
      client.print(200 - 2 * (humStore[y]));
      client.print("' x2='");
      client.print(x + 14);
      client.print("' y2='");
      client.print(200 - 2 * (humStore[y + 1]));
      client.println("' style='stroke:#420000;stroke-width:2' />");
    }
    x = x + 14;
  }
  x = 10;
  client.println("</svg></div></div>");

  /*------------------ FOOTER ------------------*/
  /*------------------ DO NOT REMOVE THIS ------------*/
  client.println("<div style='color:grey;position:absolute;margin:582px 0 0 720px'><a href='http://www.instructables.com/id/TerraControl-V10-With-NodeMCU-Webserver/'>Instructables &#169; 2016 Tomas Kuchler</a></div></div>");
  client.println("</body>");
  client.println("</html>");
  /****--------------- END OF HTML CODE ---------------****/
}

/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 5000) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress & address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

