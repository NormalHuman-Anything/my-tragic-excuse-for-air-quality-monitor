#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

char* wifi_ssid = "UCA";
char* password = "0527080001"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(wifi_name, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting...");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http = "http://127.0.0.1:8000";

    String name = "wonjun";
    String age = "50";

    getRequestURL += "/name/"; //http://127.0.0.1:8000/name/
    getRequestURL += name; //http://127.0.0.1:8000/name/wonjun
    getRequestURL += "/age/"; //http://127.0.0.1:8000/name/wonjun/age/
    getRequestURL += age; //http://127.0.0.1:8000/name/wonjun/age/50

    http.begin(getRequestURL);

    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    }
    http.end();
  }
  delay(3000);
}
