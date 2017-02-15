//https://www.arduino.cc/en/Reference/Ethernet
#include <SPI.h>
#include <Ethernet.h>

#include <dht.h>
#include <BH1750.h>

#include "ThingSpeak.h"
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
	0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
IPAddress ServerIP(192, 168, 1, 20);

	long PrevRefreshTime = 0;
	const int REFRESHING_PERIOD = 16000;
	//const IPAddress ServerIP(52,4,8,177);

	const char API_WRITE_KEY[] = "RE88TJQMTSM6ZCEO";
	const char SITE_ADDR[] = "api.thingspeak.com";
	const char API_ADDR[] = "/update";
	
	EthernetClient client;
	
	dht dhtSens; // Объект-сенсор
	BH1750 lightMeter;
	int WaterSensorPin = A3;
	int motionPin = 5;
	int DHT22_PIN = A0;
	int GasPin = A1;

	float temperature = 23;
	float humidity = 37;
	int light = 10;
	bool flooded = false;
	int ws = 10;
	int gas = 0;
	bool motion;

//IPAddress RemoteServerIP(52,4,8,177);
// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
//EthernetServer server(80);
void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	lightMeter.begin();
	Serial.println("\n[backSoon]");


	// start the Ethernet connection and the server:
	Ethernet.begin(mac);
	Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  delay(1000);
  ThingSpeak.begin(client);
}

void SendRefreshRequest(){
	client.connect(SITE_ADDR, 80);
	
	client.print("GET ");
	client.print(API_ADDR);
	client.println("HTTP/1.1");
    client.print("Host: "); client.println(SITE_ADDR);
    client.println("Connection: close");
    client.println();
	
	client.print("api_key=");
	client.print(API_WRITE_KEY);
	client.print("&field1=");
	client.print(temperature);
	client.print("&field2=");
	client.print(humidity);
	client.print("&field3=");
	client.print(gas);
	client.print("&field4=");
	client.print(light);
	client.print("&field5=");
	client.print((int)flooded);
	client.print("&field6=");
	client.print((int)motion);
	client.stop();
}

 void SendRefreshRequest_TS(){
	ThingSpeak.setField(1,temperature);
	ThingSpeak.setField(2,humidity);
	ThingSpeak.setField(3,gas);
	ThingSpeak.setField(4,light);
	ThingSpeak.setField(5,(int)flooded);
	ThingSpeak.setField(6,(int)motion);
	 
	 
	ThingSpeak.writeFields(104857, API_WRITE_KEY);  

 }
	 
 void UpdateSensors(){
	dhtSens.read22(DHT22_PIN);
	temperature = dhtSens.temperature;
	humidity = dhtSens.humidity;
	light = lightMeter.readLightLevel();
	
	ws = analogRead(WaterSensorPin);
	flooded = (ws > 300);
	
	motion = (digitalRead(motionPin) == HIGH);

	gas = analogRead(GasPin);
}


void loop() {
	if((millis() - PrevRefreshTime)>REFRESHING_PERIOD){
		PrevRefreshTime = millis();
		UpdateSensors();
		SendRefreshRequest_TS();
		// Serial.print("prev: ");
		// Serial.println(PrevRefreshTime);
		// Serial.print("current: ");
		// Serial.println(millis());
		// Serial.println("Sending the form...");
	}
	
	// // listen for incoming clients
	// client = server.available();
	// if (client) {
		// Serial.println("new client");
		// // an http request ends with a blank line
		// boolean currentLineIsBlank = true;
		// while (client.connected()) {
			// if (client.available()) {
				// char c = client.read();
				// Serial.write(c);
				// // if you've gotten to the end of the line (received a newline
				// // character) and the line is blank, the http request has ended,
				// // so you can send a reply
				// if (c == '\n' && currentLineIsBlank) {
			// Serial.print("GOT AN EMPTY LINE HERE!");
					// UpdateSensors();
					// SendHTMLPageToClient();
				// }
				// if (c == '\n') {
					// // you're starting a new line
					// currentLineIsBlank = true;
				// } else if (c != '\r') {
					// // you've gotten a character on the current line
					// currentLineIsBlank = false;
				// }
			// }
		// }
		// // give the web browser time to receive the data
		// delay(1);
		// // close the connection:
		// client.stop();
		// Serial.println("client disconnected");
	// }
}
