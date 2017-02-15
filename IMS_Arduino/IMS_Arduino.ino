//https://www.arduino.cc/en/Reference/Ethernet
#include <SPI.h>
#include <Ethernet.h>

#include <dht.h>
#include <BH1750.h>
#include "Wire.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
	0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(192, 168, 1, 177);
IPAddress ServerIP(192, 168, 1, 20);

long PrevRefreshTime = 0;



EthernetClient client;

dht dhtSens; // Объект-сенсор
BH1750 lightMeter;
int WaterSensorPin = A3;
int MotionPin = 5;
int DHT22_PIN = A0;
int GasPin = A1;

float temperature = 23;
float humidity = 37;
int light = 10;
bool flooded = false;
int ws = 10;
int gas = 0;
bool motion;


void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(9600);

	Serial.println("\nLightMeter initializing...");
	lightMeter.begin();
	

	Serial.println("\nEthernet initializing...");
	Ethernet.begin(mac);
	Serial.print("My IP address: ");


  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }

  Serial.println();
  delay(1000);
}

void SendRefreshRequest(){
	//client.connect(SITE_ADDR, 80);
	
	//client.print("GET ");
	
	//client.stop();
}


 void UpdateSensors(){
	dhtSens.read22(DHT22_PIN);
	temperature = dhtSens.temperature;
	humidity = dhtSens.humidity;
	light = lightMeter.readLightLevel();
	
	ws = analogRead(WaterSensorPin);
	flooded = (ws > 300);
	
	motion = (digitalRead(MotionPin) == HIGH);

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
	
}
