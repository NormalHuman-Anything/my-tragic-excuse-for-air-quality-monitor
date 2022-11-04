const int ADC = A0;
int value = 0;

void setup(){
    Serial.begin(115200);
}

void loop(){
    value = analogRead(ADC);
    Serial.print("ADC Value : ");
    Serial.print(value);

    delay(100);
}