#include <Arduino.h>
//用于测试模拟IO口的封装类使用
#define HAL_DAC_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#include <Chrono.h>
#include <LightChrono.h>
#include <PID_v1.h>
#include <math.h>
#include "AnalogIOPort.h"
#include "AnalogReader.h"
#include "AnalogWriter.h"
#include "IoTCtrlBoardManager.h"
#include "Mapper.h"
#include "PackedPID.h"
#include "DevBoardESP8266.h"

#define ESP_SSID "IBlab-Wifi"              //"TP-LINK_hvac" "BlackBerry Hotspot"
#define ESP_PASS "iblabwifi"           // Your network password here "141242343"
#define TCP_SERVER_ADDR "192.168.1.233"  // TCP服务器地址
#define TCP_SERVER_PORT 1995           // TCP服务器地址

DevBoardESP8266 wifi(&Serial1, &Serial, D3);
HardwareSerial Serial1(PA10, PA9);  // RX,TX

String tempBuffer = "";
boolean isTakeOver = true;
int writeAnalog;
int loopCount;  //循环计数，便于后续对比不同方法所得采样数据情况
const int SAMPLING_INTERVAL = 50;
const int OUTPUT_INTERVAL = 1000;

AnalogWriter valveOut(A3, 12);  //模拟测试时，直接加Virtual即可
AnalogReader valveReader(A1, 12, 20);
// AnalogReader flowRateVolatageReader(A2, 12, 20);
AnalogReader flowRateCurrentReader(A0, 12,20);



// Mapper flowRateMapper(1,new double[2]{0.0024,-2.3482},"FRM");
// Mapper valveReadMapper(1,new double[2]{0.0428,-35.67},"VM");
Mapper flowRateMapper(1, new double[2]{0.0029, -1.7216}, "FRM");
Mapper valveReadMapper(1, new double[2]{0.0432, -25.502}, "VRM");
Mapper valveWriteMapper(1, new double[2]{39.131, -39.758}, "VWM");
IoTCtrlBoardManager ctrlManager;

Chrono sampleChrono;  //节拍器,采样用
Chrono outputChrono;  //节拍器,输出用

// double  flowrateMeasure, valveCtrl;//flowrateSetPoint,
// PID pidController(&flowrateMeasure,&valveCtrl,&flowrateSetPoint,2,0.21,1.26,DIRECT);
PackedPID packedPidCtrlPackedPID(&flowRateCurrentReader,&valveOut, 3,2,0.21,1.26,0);

// TestSystem sys(1);

void setup() {
    Serial.begin(115200);
    Serial.println("Start setup!");

    // IO初始化
    pinMode(D6, OUTPUT);
    analogReadResolution(12);
    analogWriteResolution(12);
    digitalWrite(D6, HIGH);
    analogWrite(A3, 0);
    
    //映射器加入端口
    valveOut.setMapper(&valveWriteMapper);
    valveReader.setMapper(&valveReadMapper);
    flowRateCurrentReader.setMapper(&flowRateMapper);
    

    //控制器初始化
    //封装考虑优化一下
    packedPidCtrlPackedPID.setAcId("C_FR");
    packedPidCtrlPackedPID.needCtrlByMapping(true);
    packedPidCtrlPackedPID.pidController.SetMode(AUTOMATIC);
    packedPidCtrlPackedPID.pidController.SetSampleTime(OUTPUT_INTERVAL);
    packedPidCtrlPackedPID.pidController.SetOutputLimits(1, 100);

    //控制管理器初始化
    //压入映射器
    ctrlManager.addMapper(&flowRateMapper);
    ctrlManager.addMapper(&valveReadMapper);
    ctrlManager.addController(&packedPidCtrlPackedPID);

    //无线模块初始化
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial1.setTimeout(20);  //需要控制串口读取数据的超时设置，否则无法分开多条数据
    Serial.setTimeout(20);
    while (!Serial) {
        Serial.println("Goodnight moon!");
        ;  // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Debug Serial complete!");

    // set the data rate for the SoftwareSerial port
    while (!Serial1) {
        ;
    }
    Serial.println(" Wi-Fi Serial initialized!");
    // wifi.hardReset();

    // Wi-Fi连接测试
    Serial.println(F("Connecting to WiFi..."));
    boolean flag = wifi.connectToAP(F(ESP_SSID), F(ESP_PASS));

    if (flag) {
        Serial.println("Connecting Success");
    } else {
        Serial.println("Connecting Failed");
    }

    wifi.connectTCP(F(TCP_SERVER_ADDR), TCP_SERVER_PORT);

    wifi.setTransparentMode(true);

    flowrateMeasure = 0;
    valveCtrl = 0;
    Serial.println("Setup finished!");
}

void loop() {
    //串口命令检测
    if (Serial1.available()) {
        // Serial.println("something coming");
        tempBuffer = Serial1.readStringUntil('\n');
        tempBuffer.trim();
        Serial.println(tempBuffer);
        //管理器处理指令
        ctrlManager.commandDistributor(tempBuffer);
       
    }


    //采样部分
    if (sampleChrono.hasPassed(SAMPLING_INTERVAL)) {
        //高速采样
        // valveReader.setVirtualAnalog(valveCtrl);  //模拟测试用，假设valveCtrl是模拟量
        valveReader.updatedReadAnalog();
        // flowRateVolatageReader.setVirtualAnalog(flowrateMeasure);  //模拟测试用，假设flowrateMeasure是模拟量
        flowRateCurrentReader.updatedReadAnalog();
        sampleChrono.restart();
    }

    

    //输出与执行部分
    if (outputChrono.hasPassed(OUTPUT_INTERVAL)) {
        //一直采样，但每一秒输出
        //"LoopCount: "+String(loopCount)+
        flowrateMeasure = flowRateCurrentReader.readAnalogSmoothly(false,true);  //仅在计算的时候更新PID的输入
        // Serial.println("hello in the act");
        // 控制器更新
        // pidController.Compute();
        valveCtrl=packedPidCtrlPackedPID.updatedControl();//在实际中可以不用赋值，PID直接进行输出了

        if (valveCtrl < 1) {
            Serial.println("WARNING! ");
        }

        // 以下为模拟测试结果用，可直接用串口绘图器绘图
        // Serial.println("FlowrateVotage: " + String(flowRateVolatageReader.readAnalogSmoothly(false,true)));
        // Serial.println();
        // Serial.println("Valveopening: " + String(valveReader.readAnalogSmoothly(false,true)));
        // Serial.println();
        // Serial.println("Vset: " + String(valveCtrl));
        // Serial.println("Vset Analog: " + String(valveOut.getOutputValve()));
        // Serial.println();
        // Serial.println("Qset: " + String(ctrlManager.getSetpointById("C_FR")));

        //以下为实际用，直接用于测试中结果到处及数据处理
        // Serial.println("LoopCount:"+String(loopCount)+" FlowrateVotage " +String(flowrateMeasure)); 
        // Serial.println("LoopCount:"+String(loopCount)+" Valveopening " +String(valveReader.readAnalogSmoothly(false,true))); 
        // Serial.println("LoopCount:"+String(loopCount)+" Vset "+String(valveCtrl));
        // Serial.println("LoopCount:"+String(loopCount)+" Qset"+String(flowrateSetPoint));

        // PID计算后自动更新valveCtrl
        // flowrateMeasure = sys.updateState(valveCtrl);  //测试用
        // analogWrite(A3, valveCtrl);
        outputChrono.restart();
        loopCount++;
    }
}

// 阀门映射(value*0.0428-35.67)