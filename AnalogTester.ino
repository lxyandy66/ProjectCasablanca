#include<Arduino.h>
//用于测试模拟IO口的封装类，以及用于校正Mapper的映射关系使用
#define HAL_DAC_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#include "AnalogIOPort.h"
#include "AnalogReader.h"
#include "AnalogWriter.h"
#include <pt.h>
#include <Chrono.h>
#include <LightChrono.h>
#include "Mapper.h"
#include <ArduinoJson.h>
#include "CtrlBoardManager.h"


String tempBuffer = "";
boolean isTakeOver = true;
int writeAnalog;
const int SAMPLING_INTERVAL = 50;
const int OUTPUT_INTERVAL = 1000;
AnalogWriter aOut(A3, 12);
AnalogReader valveReader(A1, 12,10);
// AnalogReader flowRateVolatageReader(A2, 12,20);
AnalogReader flowRateCurrentReader(A0, 12,20);
Mapper flowRateMapper(1, new double[2]{0.0029, -1.7216}, "FRM");
Mapper valveReadMapper(1, new double[2]{0.0432, -25.502}, "VRM");
Mapper valveWriteMapper(1, new double[2]{39.131, -39.758}, "VWM");

StaticJsonDocument<512> jsonMeasureStatusOut;
String strJson;

int loopCount;  //循环计数，便于后续对比不同方法所得采样数据情况

Chrono sampleChrono;    //节拍器,采样用
Chrono outputChrono;    //节拍器,输出用

void setup() {
    Serial.begin(115200);
    Serial.println("Start setup!");
    pinMode(D6, OUTPUT);
    // pinMode(A0, OUTPUT);//INPUT_ANALOG
    analogReadResolution(12);
    analogWriteResolution(12);
    digitalWrite(D6, LOW);
    analogWrite(A3, 0);
    valveReader.setMapper(&valveReadMapper);
    aOut.setMapper(&valveWriteMapper);
    flowRateCurrentReader.setMapper(&flowRateMapper);
    Serial.println("Setup finished!");
}

void loop() {
    //采样部分
    digitalWrite(D6, LOW);
    if(sampleChrono.hasPassed(SAMPLING_INTERVAL)){
        valveReader.updatedReadAnalog();
        flowRateCurrentReader.updatedReadAnalog();
        // flowRateVolatageReader.updatedReadAnalog();
        sampleChrono.restart();
    }

    //串口命令检测
    if (Serial.available()) {
        tempBuffer = Serial.readString();
        tempBuffer.trim();
        // Serial.println(tempBuffer);
        if (tempBuffer == "TO") {
            isTakeOver = !isTakeOver;
            Serial.println(isTakeOver ? "Ture" : "False");
            digitalWrite(D6, isTakeOver);
            // return;
        }
        writeAnalog = tempBuffer.toInt();
        // analogWrite(A3, writeAnalog);
        aOut.outputAnalogDirectly(writeAnalog);
        Serial.println("Set to " + String(writeAnalog));
        // aOut.outputAnalogByMapping(writeAnalog);
        // Serial.println(tempBuffer);
    }

    //输出部分
    if(outputChrono.hasPassed(OUTPUT_INTERVAL)){
        //一直采样，但每一秒输出
        // Serial.println("LoopCount: "+String(loopCount)+" Valveopening:FALSE " + String(valveReader.readAnalogDirectly()));
        // //Serial.println("LoopCount: "+String(loopCount)+" Valveopening:TRUE " + String(valveReader.readAnalogSmoothly(false,true)));
        // Serial.println("LoopCount: "+String(loopCount)+" FlowrateVotage:FALSE " + String(flowRateVolatageReader.readAnalogDirectly()));
        // Serial.println("LoopCount: "+String(loopCount)+" FlowrateVotage:TRUE " + String(flowRateVolatageReader.readAnalogSmoothly(false,true)));
        // Serial.println("LoopCount: "+String(loopCount)+" FlowrateCurrent:FALSE " + String(flowRateCurrentReader.readAnalogDirectly()));
        // //Serial.println("LoopCount: "+String(loopCount)+" FlowrateCurrent:TRUE " + String(flowRateCurrentReader.readAnalogSmoothly(false,true)));

        jsonMeasureStatusOut.clear();
        jsonMeasureStatusOut[CtrlBoardManager::SER_OUT_LOOP]=loopCount;
        jsonMeasureStatusOut["Qr"]=flowRateCurrentReader.readAnalogSmoothly(false,false);
        jsonMeasureStatusOut["Vr"]=valveReader.readAnalogSmoothly(false,false);
        serializeJson(jsonMeasureStatusOut, strJson);
        Serial.println(strJson);
        strJson = "";
        outputChrono.restart();
        loopCount++;
    }
    // Serial.println(loopCount++);
}

// 阀门映射(value*0.0428-35.67)