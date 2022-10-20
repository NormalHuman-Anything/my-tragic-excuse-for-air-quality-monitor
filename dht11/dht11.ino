#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EduIntro.h>


DHT11 dht11(13);

unsigned long delayTime;

const char* ssid = "123456";
const char* password = "ppap1542xd";

ESP8266WebServer server(80);

String GenerateMetrics() {
  String message = "";
  message += "# HELP dht11_measuring_temperature Current sensor temperature in celsius.\n";
  message += "# TYPE dht11_measuring_temperature gauge\n";
  message += "dht11_measuring_temperature ";
  message += dht11.readCelsius();
  message += "\n";

  message += "# HELP dht11_measuring_humidity Current sensor humidity in persent.\n";
  message += "# TYPE dht11_measuring_humidity gauge\n";
  message += "dht11_measuring_humidity ";
  message += dht11.readHumidity();
  message += "\n";

  

  return message;
}


void HandleRoot() {
  dht11.update();
  server.send(200, "text/plain", GenerateMetrics() );
}
void HandleNotFound() {
  String message = "Error\n\n";
  server.send(404, "text/html", message);
}



void setup() {
    Serial.begin(115200);
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
    dht11.update();
    Serial.print("Temperature = ");
    Serial.print(dht11.readCelsius());
    Serial.println(" *C");

    //Serial.print("Pressure = ");

    //Serial.print(bme.readPressure() *0.007500617);
    //Serial.println(" hPa");

    //Serial.print("Approx. Altitude = ");
    //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    //Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(dht11.readHumidity());
    Serial.println(" %");

    Serial.println();
}
