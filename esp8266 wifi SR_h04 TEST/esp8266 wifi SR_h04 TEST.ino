#include <ESP8266wifi.h>

const char* ssid = "838812-NJTN-NY";
const char* password = "2katterientrea";


//HC-SR04 nedan
#define echoPin 14 // Echo Pin
#define trigPin 12 // Trigger Pin

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
float duration, distance; // Duration used to calculate distance

WiFiServer server(80);

void setup() {
	Serial.begin(115200);
	delay(10);

	//SR04
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	//WiFi original
	pinMode(5, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(0, OUTPUT);
	pinMode(13, OUTPUT);
	digitalWrite(5, LOW);
	digitalWrite(4, LOW);
	digitalWrite(0, LOW);
	digitalWrite(13, LOW);

	// Connect to WiFi network
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

	// Start the server
	server.begin();
	Serial.println("Server started");

	// Print the IP address
	Serial.print("Use this URL to connect: ");
	Serial.print("http://");
	Serial.print(WiFi.localIP());
	Serial.println("/");

}

void loop() {
	// Check if a client has connected
	WiFiClient client = server.available();
	if (!client) {
		return;
	}

	// Wait until the client sends some data
	Serial.println("new client");
	while (!client.available()) {
		delay(1);
	}

	// Read the first line of the request
	String request = client.readStringUntil('\r');
	Serial.println(request);
	client.flush();

	// Match the request
	if (request.indexOf("/light1on") > 0) {
		digitalWrite(5, HIGH);
	}
	if (request.indexOf("/light1off") > 0) {
		digitalWrite(5, LOW);
	}
	if (request.indexOf("/light2on") > 0) {
		digitalWrite(4, HIGH);
	}
	if (request.indexOf("/light2off") > 0) {
		digitalWrite(4, LOW);

	}
	if (request.indexOf("/light3on") > 0) {
		digitalWrite(0, HIGH);

	}
	if (request.indexOf("/light3off") > 0) {
		digitalWrite(0, LOW);
	}
	if (request.indexOf("/light4on") > 0) {
		digitalWrite(13, HIGH);
	}
	if (request.indexOf("/light4off") > 0) {
		digitalWrite(13, LOW);

	}

	//SR04
	/* The following trigPin/echoPin cycle is used to determine the
	distance of the nearest object by bouncing soundwaves off of it. */
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);

	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);

	digitalWrite(trigPin, LOW);
	duration = pulseIn(echoPin, HIGH);
	delay(500);

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

	if (distance >= maximumRange || distance <= minimumRange) {
		/* Send a negative number to computer and Turn LED ON
		to indicate "out of range" */
		Serial.println("-1");
	}
	else {
		/* Send the distance to the computer using Serial protocol, and
		turn LED OFF to indicate successful reading. */
		Serial.println("Distance: ");
		Serial.println(distance);
	}

	//Originalkod

	// Set ledPin according to the request
	//digitalWrite(ledPin, value);

	// Return the response
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println(""); //  do not forget this one
	client.println("<!DOCTYPE HTML>");
	client.println("<html>");
	client.println("<head>");
	client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
	client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
	client.println("</head>");
	client.println("<body bgcolor = \"#f7e6ec\">");
	client.println("<hr/><hr>");
	client.println("<h4><center> Esp8266 Electrical Device Control </center></h4>");

	//Skriv ut avståndet
	client.println("<h4><center>");
	client.println("Distance: ");
	client.println(distance, 2);
	client.println("</center></h4>");


	client.println("<hr/><hr>");
	client.println("<br><br>");
	client.println("<br><br>");
	client.println("<center>");
	client.println("Device 1");
	client.println("<a href=\"/light1on\"\"><button>Turn On </button></a>");
	client.println("<a href=\"/light1off\"\"><button>Turn Off </button></a><br />");
	client.println("</center>");
	client.println("<br><br>");
	client.println("<center>");
	client.println("Device 2");
	client.println("<a href=\"/light2on\"\"><button>Turn On </button></a>");
	client.println("<a href=\"/light2off\"\"><button>Turn Off </button></a><br />");
	client.println("</center>");
	client.println("<br><br>");
	client.println("<center>");
	client.println("Device 3");
	client.println("<a href=\"/light3on\"\"><button>Turn On </button></a>");
	client.println("<a href=\"/light3off\"\"><button>Turn Off </button></a><br />");
	client.println("</center>");
	client.println("<br><br>");
	client.println("<center>");
	client.println("Device 4");
	client.println("<a href=\"/light4on\"\"><button>Turn On </button></a>");
	client.println("<a href=\"/light4off\"\"><button>Turn Off </button></a><br />");
	client.println("</center>");
	client.println("<br><br>");
	client.println("<center>");
	client.println("<table border=\"5\">");
	client.println("<tr>");
	if (digitalRead(5))
	{
		client.print("<td>Light 1 is ON</td>");

	}
	else
	{
		client.print("<td>Light 1 is OFF</td>");

	}

	client.println("<br />");

	if (digitalRead(4))
	{
		client.print("<td>Light 2 is ON</td>");

	}
	else
	{

		client.print("<td>Light 2 is OFF</td>");

	}

	if (digitalRead(0))

	{
		client.print("<td>Light 3 is ON</td>");

	}

	else

	{
		client.print("<td>Light 3 is OFF</td>");
	}


	if (digitalRead(13))
	{
		client.print("<td>Light 4 is ON</td>");
	}


	else
	{
		client.print("<td>Light 4 is OFF</td>");
	}

	client.println("</tr>");


	client.println("</table>");

	client.println("</center>");
	client.println("</html>");
	delay(1);
	Serial.println("Client disonnected");
	Serial.println("");

}