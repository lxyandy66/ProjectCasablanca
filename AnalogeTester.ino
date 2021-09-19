// #include<Arduino.h>
#define HAL_DAC_MODULE_ENABLED
#include"AnalogeIOPort.h"

int writeAnaloge;
AnalogeIOPort aOut(A3, 12);
AnalogeIOPort aIn(A1, 12);

void setup(){
    Serial.begin(115200);
    aOut.setUpper(4055);
        aOut.setLower(50);
        aIn.setUpper(4055);
        aIn.setLower(50);
    // pinMode(A1, INPUT_ANALOG);
}

void loop(){
    if(Serial.available()){
        writeAnaloge=Serial.readString().toInt();
        // analogWrite(A3, writeAnaloge);
        aOut.outputAnalog(writeAnaloge);//按比例写入
        
    }
    delay(1000);
    // Serial.println("aOut: " + String(aOut.getOutputMax()));
        // Serial.println("aIn: " + String(aIn.getOutputMax()));
        Serial.println("Read: "+String(aIn.readAnalog(),3));
        Serial.println("Direct Read: "+String(aIn.readDirectAnalog()));
        // Serial.println("Direct Read from system: "+String(analogRead(A1)));
}