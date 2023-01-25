const int ADC = A0;
float value = 0;

void setup(){
    Serial.begin(115200);
}

void loop(){
    value = analogRead(ADC);
    Serial.print("ADC Value : ");
    Serial.println(value/231);

    delay(100);
}