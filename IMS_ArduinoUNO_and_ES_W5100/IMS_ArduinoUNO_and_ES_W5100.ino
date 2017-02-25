/*****************************************************************************************
 * Purpose : Zabbix Sensor Agent - Environmental Monitoring Solution                     *
 * Author  : Evgeny Levkov                                                               *
 * Credits:                                                                              *
 *           JO3RI www.JO3RI.be/arduino - for web based ethernet config with EEPROM code *
 *           Rob Faludi http://www.faludi.com - for free memory test code                *
 *****************************************************************************************/

// ZSA1-E
//Credits: 
// JO3RI www.JO3RI.be/arduino - for web based ethernet config with EEPROM code
// Rob Faludi http://www.faludi.com - for free memory test code
// Evgeny Levkov - Zabbix Agent implementation http://www.sensor.im

//-----------------INCLUDES--------------------
#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"         // Download at https://github.com/adafruit/DHT-sensor-library
//#include <avr/pgmspace.h>
//--------------------------------------------
#define MAX_CMD_LENGTH   25
#define MAX_LINE 20

int DHT22_PIN = A0; 

float temperature = 23;
float humidity = 37;

#define DHTTYPE DHT22
DHT dht(DHT22_PIN, DHTTYPE);
//--------------------------------------------
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xE3, 0x5B };

IPAddress ip(192, 168, 1, 222);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetServer server = EthernetServer(10050); //Zabbix Agent port //Zabbix agent port
EthernetClient client;



String cmd; //FOR ZABBIX COMMAND
bool cmd_name_started = false;


int led = 13; //LED PORT TO BLINK AFTER RECIEVING ZABBIX COMMAND


void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  pinMode(2, OUTPUT); //???
  pinMode(7, INPUT_PULLUP); //???
  pinMode(led, OUTPUT);    
  digitalWrite(led, LOW);
 
}

void loop()
{
//--------------------------------------------
  client = server.available();
  if (client) {
    //client.flush();
    while (client.connected()) {
      if(client.available() > 0) {
        readTelnetCommand(client.read());
      }
    }
  }
}

//--------------------------------------------
void readTelnetCommand(char c) {

  if(cmd.length() == MAX_CMD_LENGTH) {
    cmd = "";
  }
  
  Serial.println((int)c);
  
  if(c == '\n' || c=='\r') {
    if(cmd.length() > 2) {
      // remove \r and \n from the string
      parseCommand();
    }
  }else{
    cmd += c;
  }
}
//--------------------------------------------
void parseCommand() {  //Commands recieved by agent on port 10050 parsing
  Serial.println(cmd);
  // AGENT.PING      
  if(cmd.equals("agent.ping")) {
      client.println("1");
      client.stop();

 // AGENT.VERSION      
   } else if(cmd.equals("agent.version")) {
      server.println("Sensor.IM 1.0");
      client.stop();
   }
  else if(cmd.equals("humidity")) {
    humidity = dht.readHumidity();
    server.println(humidity);
    client.stop();
    delay(2000);

// TEMPERATURE.READ
} else if(cmd.equals("temperature")) {
      temperature = dht.readTemperature();
       server.println(temperature);
      client.stop();
      delay(2000);
      
// NOT SUPPORTED      
  } else {
    //  server.println("ZBXDZBX_NOTSUPPORTED");
    server.println(cmd);
      client.stop();
  }
  cmd = "";
}

 void UpdateSensors(){
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  //light = lightMeter.readLightLevel();
  
  //ws = analogRead(WaterSensorPin);
  //flooded = (ws > 300);
  
  //motion = (digitalRead(motionPin) == HIGH);

  //gas = analogRead(GasPin);
}
