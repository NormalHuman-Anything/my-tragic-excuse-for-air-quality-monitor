#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_PM25AQI.h"
#include "s8_uart.h"
#include "modbus_crc.h"
#include "utils.h"
#include "DHT.h"


#define DHTPIN 15
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


unsigned long delayTime;

PM25_AQI_Data data;

int led = 2;

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

SoftwareSerial pmSerial(13, 12);

#define S8_RX_PIN 14         // Rx pin which the S8 Tx pin is attached to (change if it is needed)
#define S8_TX_PIN 16         // Tx pin which the S8 Rx pin is attached to (change if it is needed)

SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);

const char* ssid = "123456";
const char* password = "ppap1542xd";

S8_UART *sensor_S8;
S8_sensor sensor;

ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

  

String GenerateMetrics() {
  digitalWrite(led, LOW);
  String message = "";
  message += "# HELP dht11_measuring_temperature Current sensor temperature in celsius.\n";
  message += "# TYPE dht11_measuring_temperature gauge\n";
  message += "dht11_measuring_temperature ";
  message += dht.readTemperature();
  message += "\n";

  message += "# HELP dht11_measuring_humidity Current sensor humidity in percent.\n";
  message += "# TYPE dht11_measuring_humidity gauge\n";
  message += "dht11_measuring_humidity ";
  message += dht.readHumidity();
  message += "\n";

  message += "dht11_measuring_HeatIndex ";
  message += dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false);
  
  
  message += "STANDARD concentration units:";

  message += "PM 1.0";
  message += data.pm10_standard;

  message += "PM2.5";
  message += data.pm25_standard;

  message += "PM10";
  message += data.pm100_standard;



  message += "environmental concentration units:";

  message += "PM1.0";
  message += data.pm10_env;

  message += "PM2.5";
  message += data.pm25_env;

  message += "PM10";
  message += data.pm100_env;

  message += "particles > (size) / 0.1L air : ";
  message += "0.3um";
  message += data.particles_03um;

  message += "0.5um";
  message += data.particles_05um;

  message += "1.0um";
  message += data.particles_10um;

  message += "2.5um";
  message += data.particles_25um;

  message += "5.0um";
  message += data.particles_50um;

  message += "10um";
  message += data.particles_100um;


  message += "SenseAir S8 CO2 : (ppm)";
  
  message += sensor.co2 = sensor_S8->get_co2();

  digitalWrite(led, HIGH);
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

   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  S8_serial.begin(S8_BAUDRATE);
  sensor_S8 = new S8_UART(S8_serial);

  sensor_S8->get_firmware_version(sensor.firm_version);
  int len = strlen(sensor.firm_version);
  if (len == 0) {
      Serial.println("SenseAir S8 CO2 sensor not found!");
      while (1) { delay(1); };
  }

  
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }
    
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
