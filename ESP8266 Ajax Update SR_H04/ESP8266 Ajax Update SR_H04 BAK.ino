#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
const char* ssid = "838812-NJTN-NY";
const char* password = "2katterientrea";
String webSite, javaScript, XML;

//HC-SR04 setup nedan
#define echoPin 14 // Echo Pin
#define trigPin 12 // Trigger Pin

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
float duration, distance; // Duration used to calculate distance

void buildWebsite() {
	buildJavascript();
	webSite = "<!DOCTYPE HTML>\n";
	webSite += javaScript;
	webSite += "<BODY onload='process()'>\n";
	webSite += "<BR>This is the ESP website ...<BR>\n";
	webSite += "Runtime = <A id='runtime'></A>\n";
	webSite += "</BODY>\n";
	webSite += "</HTML>\n";
}

String getDistanceReading() {
	//SR04
	/* The following trigPin/echoPin cycle is used to determine the
	distance of the nearest object by bouncing soundwaves off of it. */
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);

	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);

	digitalWrite(trigPin, LOW);
	duration = pulseIn(echoPin, HIGH);

	/*Calculate the distance (in cm) based on the speed of sound.
	Ljudets hastighet:
	m/s			graderC
	340.276		15
	343.216		20
	346.13		25
	*/
	/*For example, if the object is 10 cm away from the sensor,
	and the speed of the sound is 340 m/s or 0.034 cm / µs the sound wave will need to travel about 294 u seconds.
	But what you will get from the Echo pin will be double that number because the sound wave needs to travel forward and bounce backward.
	So in order to get the distance in cm we need to multiply the received travel time value from the echo pin by 0.034 and divide it by 2.*/

	// ljudets hastighet för 25 graderC, -0,55 är kompensationen, för jag anser att den mäter från högtalarens yttersta kant
	distance = duration*0.034613 / 2 - 0.55;

	Serial.println(distance);

	if (distance >= maximumRange || distance <= minimumRange) {
		return "-1";
	}
	else {
		return String(distance, 2);
	}
}

void buildJavascript() {
	javaScript = "<SCRIPT>\n";
	javaScript += "var xmlHttp=createXmlHttpObject();\n";

	javaScript += "function createXmlHttpObject(){\n";
	javaScript += " if(window.XMLHttpRequest){\n";
	javaScript += "    xmlHttp=new XMLHttpRequest();\n";
	javaScript += " }else{\n";
	javaScript += "    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
	javaScript += " }\n";
	javaScript += " return xmlHttp;\n";
	javaScript += "}\n";

	javaScript += "function process(){\n";
	javaScript += " if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
	javaScript += "   xmlHttp.open('PUT','xml',true);\n";
	javaScript += "   xmlHttp.onreadystatechange=handleServerResponse;\n"; // no brackets?????
	javaScript += "   xmlHttp.send(null);\n";
	javaScript += " }\n";
	javaScript += " setTimeout('process()',1000);\n";
	javaScript += "}\n";

	javaScript += "function handleServerResponse(){\n";
	javaScript += " if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
	javaScript += "   xmlResponse=xmlHttp.responseXML;\n";
	javaScript += "   xmldoc = xmlResponse.getElementsByTagName('response');\n";
	javaScript += "   message = xmldoc[0].firstChild.nodeValue;\n";
	javaScript += "   document.getElementById('runtime').innerHTML=message;\n";
	javaScript += " }\n";
	javaScript += "}\n";
	javaScript += "</SCRIPT>\n";
}

void buildXML() {
	XML = "<?xml version='1.0'?>";
	XML += "<response>";
	//XML += millis2time();
	XML += getDistanceReading();
	XML += "</response>";
}
String millis2time() {
	String Time = "";
	unsigned long ss;
	byte mm, hh;
	ss = millis() / 1000;
	hh = ss / 3600;
	mm = (ss - hh * 3600) / 60;
	ss = (ss - hh * 3600) - mm * 60;
	if (hh<10)Time += "0";
	Time += (String)hh + ":";
	if (mm<10)Time += "0";
	Time += (String)mm + ":";
	if (ss<10)Time += "0";
	Time += (String)ss;
	return Time;
}

void handleWebsite() {
	buildWebsite();
	server.send(200, "text/html", webSite);
}

void handleXML() {
	buildXML();
	server.send(200, "text/xml", XML);
}

void setup() {
	Serial.begin(115200);

	//SR04
	/* The following trigPin/echoPin cycle is used to determine the
	distance of the nearest object by bouncing soundwaves off of it. */
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);

	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);

	digitalWrite(trigPin, LOW);
	duration = pulseIn(echoPin, HIGH);

	/*Calculate the distance (in cm) based on the speed of sound.
	Ljudets hastighet:
	m/s			graderC
	340.276		15
	343.216		20
	346.13		25
	*/
	/*For example, if the object is 10 cm away from the sensor,
	and the speed of the sound is 340 m/s or 0.034 cm / µs the sound wave will need to travel about 294 u seconds.
	But what you will get from the Echo pin will be double that number because the sound wave needs to travel forward and bounce backward.
	So in order to get the distance in cm we need to multiply the received travel time value from the echo pin by 0.034 and divide it by 2.*/

	// ljudets hastighet för 25 graderC, -0,55 är kompensationen, för jag anser att den mäter från högtalarens yttersta kant
	distance = duration*0.034613 / 2 - 0.55;

	Serial.println(distance);


	//Webbsida
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)delay(500);
	WiFi.mode(WIFI_STA);
	Serial.println("\n\nBOOTING ESP8266 ...");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("Station IP address: ");
	Serial.println(WiFi.localIP());
	server.on("/", handleWebsite);
	server.on("/xml", handleXML);
	server.begin();
}

void loop() {
	server.handleClient();
}