#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "SHTSensor.h"
#include "Adafruit_PM25AQI.h"
#include "s8_uart.h"
#include "modbus_crc.h"
#include "utils.h"

//config start

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//PIN 5 IS SCL AND PIN 4 IS SDA REMEMBER!

SHTSensor sht;

/*
int PM10_prev = 100;
int PM25_prev = 100;
int PM100_prev = 100;
*/

PM25_AQI_Data data;

const int ADC = A0;
int value = 0;

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
SoftwareSerial pmSerial(12, 13);

#define S8_RX_PIN 14         // Rx pin which the S8 Tx pin is attached to (change if it is needed)
#define S8_TX_PIN 16         // Tx pin which the S8 Rx pin is attached to (change if it is needeTd)

SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);

const char* ssid = "123456";
const char* password = "ppap1542xd";

S8_UART *sensor_S8;
S8_sensor sensor;

int tme = 0;
int order = 1;

ESP8266WebServer server(80);

/*
int ErrorKill(int psize) {
  if(psize == 10){
    if(data.pm10_standard>PM10_prev+300){
      return(PM10_prev);
    }else{
      PM10_prev = data.pm10_standard;
      return(data.pm10_standard);
    }
  }else if(psize == 25){
    if(data.pm25_standard>PM25_prev+300){
      return(PM25_prev);
    }else{
      PM25_prev = data.pm25_standard;
      return(data.pm25_standard);
    }
  }else if(psize == 100){
      if(data.pm100_standard>PM100_prev+300){
      return(PM100_prev);
    }else{
      PM100_prev = data.pm100_standard;
      return(data.pm100_standard);
    }
  }
}
*/


//config end

String GenerateMetrics() {
  sht.readSample();
  aqi.read(&data);
  String message = "";

  
  message += "sht30_getTemperature ";
  message += sht.getTemperature();
  message += "\n";
  
  message += "sht30_getHumidity ";
  message += sht.getHumidity();
  message += "\n";

  message += "data_PM10_std ";
  //message += ErrorKill(10);
  message += data.pm10_standard;
  message += "\n";
  message += "data_PM25_std ";
  //message += ErrorKill(25);
  message += data.pm25_standard;
  message += "\n";
  message += "data_PM100_std ";
  //message += ErrorKill(100);
  message += data.pm100_standard;
  message += "\n";

  message += "senseair_s8_get_co2 ";
  message += sensor.co2 = sensor_S8->get_co2();
  message += "\n";

  message += "VBAT_ADC ";
  message += value;


  return message;
}


void WriteToDisplay() {
  display.clearDisplay();
  aqi.read(&data);
    if(order == 2){
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println("Humidity");
      sht.readSample();
      display.println(sht.getHumidity());
      display.print(" %");
      display.display();
      order++;
    }else if(order == 3){
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println("PM1.0");
      sht.readSample();
      display.println(data.pm10_standard);
      display.print(" ug/m3");
      display.display();
      order++;
    }else if(order == 4){
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println("PM2.5");
      sht.readSample();
      display.println(data.pm25_standard);
      display.print(" ug/m3");
      display.display();
      order++;
    }else if(order == 5){
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println("PM10.0");
      sht.readSample();
      display.println(data.pm100_standard);
      display.print(" ug/m3");
      display.display();
      order++;
    }else if(order == 6){
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println("CO2");
      sensor.co2 = sensor_S8->get_co2();
      display.println(sensor.co2);
      display.print(" ppm");
      display.display();
      order++;
    }else{
      order=1;
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             // Start at top-left corner
      display.println("Temp");
      sht.readSample();
      display.println(sht.getTemperature());
      display.print(" C");
      display.display();
      order++;
    }
    
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
    //while(!Serial); 
    Wire.begin(); 

   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
  }

  display.display();
  display.clearDisplay();

    if (sht.init()) {
      Serial.print("init(): success\n");
  } else {
      Serial.print("init(): failed\n");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);

  S8_serial.begin(S8_BAUDRATE);
  sensor_S8 = new S8_UART(S8_serial);

  sensor_S8->get_firmware_version(sensor.firm_version);
  int len = strlen(sensor.firm_version);
  if (len == 0) {
      Serial.println("SenseAir S8 CO2 sensor not found!");
      //while (1) { delay(1); };
  }
  pmSerial.begin(9600);
  delay(1000);
  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    //delay(500);  // try again in a bit!
    //return;
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


    display.display();
  delay(500); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(500);
  PM10_prev = data.pm10_standard;
  PM25_prev = data.pm25_standard;
  PM100_prev = data.pm100_standard;
}


void loop() { 
    //if(tme == 2){
      WriteToDisplay();
      //tme = 0;
    //}
    value = analogRead(ADC);
    server.handleClient();
    //tme++;
    delay(1000);
}
