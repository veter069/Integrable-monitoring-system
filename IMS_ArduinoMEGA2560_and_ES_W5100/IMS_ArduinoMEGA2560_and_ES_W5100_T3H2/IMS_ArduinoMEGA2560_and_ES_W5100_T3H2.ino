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
#include <Wire.h> //Standart library for using BMP180
#include <Adafruit_BMP085.h> // Download at https://github.com/adafruit/Adafruit-BMP085-Library

//#include <avr/pgmspace.h>
//--------------------------------------------
#define MAX_CMD_LENGTH   50
#define MAX_CMD_LENGTH   25

// Connect VCC of the BMP_085/180 sensor to 3.3V (NOT 5.0V!)
// Mega2560 -> BMP180; 20 -> (SDA), 21 -> (SCL)

int LED_PIN = 13;
int DHT11_PIN = A1; 
int DHT22_PIN = A2; 
int WATER_PIN = 26;
int MOTION_PIN = 50;
int CO2_PIN = A0;

DHT* dht11;
DHT* dht22;

float temperature1 = 666;
float humidity1 = 666;
float temperature2 = 666;
float temperature3 = 666;
float pressure = 666;
float humidity2 = 666;
bool flooding = false;
bool motion = false;
int co2 = 666;

#define DHTTYPE_11 DHT11
#define DHTTYPE_22 DHT22
//--------------------------------------------
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xE3, 0x5B };

IPAddress ip(192, 168, 1, 134);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetServer server = EthernetServer(10050); //Zabbix Agent port //Zabbix agent port
EthernetClient client;


String cmd; //FOR ZABBIX COMMAND
bool cmd_name_started = false;
Adafruit_BMP085 bmp;

void setup()
{
  dht11 = new DHT(DHT11_PIN, DHTTYPE_11);
  dht22 = new DHT(DHT22_PIN, DHTTYPE_22);
  bmp.begin();
  
  Serial.begin(9600);
  Ethernet.begin(mac, ip, gateway, subnet);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");}

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  pinMode(2, OUTPUT); //???
  pinMode(7, INPUT_PULLUP); //???
  
  pinMode(LED_PIN, OUTPUT);    
  digitalWrite(LED_PIN, LOW);
  
  // prepare DHT22
  pinMode(DHT22_PIN, OUTPUT);
  digitalWrite(DHT22_PIN, 0);

  // prepare DHT11
  pinMode(DHT11_PIN, OUTPUT);
  digitalWrite(DHT11_PIN, 0);
  
  // prepare Water Sensor pin
  pinMode(WATER_PIN, INPUT);

  // prepare PIR motion Sensor pin
  pinMode(MOTION_PIN, INPUT);

  // prepare CO2 Sensor pin
  pinMode(CO2_PIN, INPUT);

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
  
  //Serial.println((int)c);
  
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
  }
  // AGENT.HOSTNAME
  else if(cmd.equals("agent.hostname")) {
      client.println("ARDUINO_MEGA_2560_WITH_ES_W5100");
      client.stop();
   } 
   // AGENT.VERSION  
   else if(cmd.equals("agent.version")) {
      client.println("EnviMon 0.0.3");
      client.stop();
   }
   // HUMIDITY 1
  else if(cmd.equals("environment.humidity1")) {
    UpdateHumidity1();
    client.println(humidity1);
    client.stop();
    delay(2000);
  } 
   // TEMPERATURE 1
  else if(cmd.equals("environment.temperature1")) {
    UpdateTemperature1();
    client.println(temperature1);
    client.stop();
    delay(2000);
  }   
  // HUMIDITY 2
  else if(cmd.equals("environment.humidity2")) {
    UpdateHumidity2();
    client.println(humidity2);
    client.stop();
    delay(2000);
  } 
   // TEMPERATURE 2
  else if(cmd.equals("environment.temperature2")) {
    UpdateTemperature2();
    client.println(temperature2);
    client.stop();
    delay(2000);
  }  
  // TEMPERATURE 3 (BMP180)
  else if(cmd.equals("environment.temperature3")) {
    UpdateTemperature3();
    client.println(temperature3);
    client.stop();
    delay(500);
  }
  // Pressure (BMP180) mm Hg
  else if(cmd.equals("environment.pressure")) {
    UpdatePressure();
    client.println(pressure);
    client.stop();
    delay(500);
  }      
   // FLOODING
  else if(cmd.equals("environment.flooding")) {
    UpdateFlooding();
    client.println(flooding);
    client.stop();
  }  
   // MOTION
  else if(cmd.equals("environment.motion")) {
    UpdateMotion();
    client.println(motion);
    client.stop();
  }  
   // CO2
  else if(cmd.equals("environment.co2")) {
    UpdateCO2();
    client.println(co2);
    client.stop();
  }   
   // NOT SUPPORTED      
  else {
    //  server.println("ZBXDZBX_NOTSUPPORTED");
    client.println(cmd);
    client.stop();
  }
  cmd = "";
}

void UpdateHumidity1(){
  humidity1 = dht11->readHumidity();
}
void UpdateTemperature1(){
  temperature1 = dht11->readTemperature();
}
void UpdateHumidity2(){
  humidity2 = dht22->readHumidity();
}
void UpdateTemperature2(){
  temperature2 = dht22->readTemperature();
}
void UpdateTemperature3(){
  temperature3 = bmp.readTemperature();
}
void UpdatePressure(){
  pressure = ((bmp.readPressure())/133.3224);
}
void UpdateFlooding(){
  flooding = (digitalRead(WATER_PIN) == HIGH);
}

void UpdateMotion(){
  motion = (digitalRead(MOTION_PIN) == HIGH);
}

void UpdateCO2(){
  co2 = analogRead(CO2_PIN);
}
