
#include <ESP8266WiFi.h>
#include "DHT.h"

const char* ssid = "Frosters";
const char* password = "!#DidYouTryHard?!A";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(10050);

#define DHTTYPE DHT22
int DHT22_PIN = D2; 
float temperature = 23;
float humidity = 37;
DHT dht(DHT22_PIN, DHTTYPE);

int WATER_PIN = D3;
bool flooding = false;

int MOTION_PIN = D7;
bool motion = false;

int GAS_PIN = A0;
int gas = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare DHT22 pin
  pinMode(DHT22_PIN, OUTPUT);
  digitalWrite(DHT22_PIN, 0);

  // prepare Water Sensor pin
  pinMode(WATER_PIN, INPUT);

  // prepare PIR motion Sensor pin
  pinMode(MOTION_PIN, INPUT);

  // prepare Gas Sensor pin
  pinMode(GAS_PIN, INPUT);
  
  
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
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("");
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
//  for(int i=0; i< req.length(); i++){
//    Serial.println((int)(req[i]));  
//  }

  if (req.indexOf("agent.ping") != -1){
    client.println("1");
    Serial.println();
  } 
  else if (req.indexOf("agent.version") != -1){
    client.println("EnviMon 0.0.2");
    Serial.println("EnviMon 0.0.2");
  } 
  else if (req.indexOf("environment.temperature") != -1){
    UpdateDHT();
    client.println(temperature);
    Serial.println(temperature);
  } 
  else if (req.indexOf("environment.humidity") != -1){
    UpdateDHT();
    client.println(humidity);
    Serial.println(humidity);
  } 
  else if (req.indexOf("environment.flooding") != -1){
    UpdateSensors();
    client.println(flooding);
    Serial.println(flooding);
  }
  else if (req.indexOf("environment.motion") != -1){
    UpdateSensors();
    client.println(motion);
    Serial.println(motion);
  }
  else if (req.indexOf("environment.gas") != -1){
    UpdateSensors();
    client.println(gas);
    Serial.println(gas);
  }
  else {
    server.println("ZBXDZBX_NOTSUPPORTED");
    Serial.println("ZBXDZBX_NOTSUPPORTED");
  }
  
  client.flush();

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void UpdateDHT(){
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}

void UpdateSensors(){
  //light = lightMeter.readLightLevel();
  
  flooding = (digitalRead(WATER_PIN) == HIGH);
  //flooding = (ws > 300);
  
  motion = (digitalRead(MOTION_PIN) == HIGH);

  gas = analogRead(GAS_PIN);
}

