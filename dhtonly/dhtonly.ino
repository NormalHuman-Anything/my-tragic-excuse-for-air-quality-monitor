#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>




#include "DHT.h"

#define DHTPIN 13
#define DHTTYPE DHT11
unsigned long delayTime;
int led = 2;
const char* ssid = "123456";
const char* password = "ppap1542xd";

ESP8266WebServer server(80);

DHT dht(DHTPIN, DHTTYPE);
String GenerateMetrics() {
  digitalWrite(led, HIGH);
  String message = "";
  message += "# HELP dht11_measuring_temperature Current sensor temperature in celsius.\n";
  message += "# TYPE dht11_measuring_temperature gauge\n";
  message += "dht11_measuring_temperature ";
  message += dht.readTemperature();
  message += "\n";

  message += "# HELP dht11_measuring_humidity Current sensor humidity in persent.\n";
  message += "# TYPE dht11_measuring_humidity gauge\n";
  message += "dht11_measuring_humidity ";
  message += dht.readHumidity();
  message += "\n";

  message += "dht11_measuring_HeatIndex ";
  message += dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false);
  delay(500);
  digitalWrite(led, LOW);
  return message;
}


void HandleRoot() {

  server.send(200, "text/plain", GenerateMetrics() );
}
void HandleNotFound() {
  String message = "Error\n\n";
  server.send(404, "text/html", message);
}



void setup() {
    Serial.begin(115200);
    dht.begin();
    while(!Serial);  

// BME280
    Serial.println(F("dht11 test"));
    
    
    Serial.println("-- Default Test --");


  //Setup WIFI
  WiFi.begin(ssid, password);
  Serial.println("");

  //Wait for WIFI connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  server.on("/", HandleRoot);
  server.on("/metrics", HandleRoot);
  server.onNotFound( HandleNotFound );

  server.begin();
  Serial.println("HTTP server started at ip " + WiFi.localIP().toString() );


    Serial.println();
}


void loop() { 
    printValues();
    server.handleClient();
    delay(1000);
}


void printValues() {

    Serial.print("Temperature = ");
    Serial.print(dht.readTemperature());
    Serial.println(" *C");



    Serial.print("Humidity = ");
    Serial.print(dht.readHumidity());
    Serial.println(" %");

    Serial.println();
}
