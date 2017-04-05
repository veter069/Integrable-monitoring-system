
#include <ESP8266WiFi.h>
#include "DHT.h"

const char* ssid = "";
const char* password = "";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(10050);

#define DHTTYPE DHT22
int DHT22_PIN = D2; 
float temperature = 23;
float humidity = 37;
DHT dht(DHT22_PIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  
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
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  for(int i=0; i< req.length(); i++){
    Serial.println((int)(req[i]));  
  }

  if (req.indexOf("agent.ping") != -1){
    client.println("1");
  } 
  else if (req.indexOf("agent.version") != -1){
    client.println("EnviMon 0.0.2");
  } 
  else if (req.indexOf("environment.temperature") != -1){
    UpdateSensors();
    client.println(temperature);
  } 
  else if (req.indexOf("environment.humidity") != -1){
    UpdateSensors();
    client.println(humidity);
  } 
  else {
    server.println("ZBXDZBX_NOTSUPPORTED");
  }
  
  client.flush();

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
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

