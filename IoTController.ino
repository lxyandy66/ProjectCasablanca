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
#include "VirtualAnalogIO.h"
#include "IoTCtrlBoardManager.h"
#include "Mapper.h"
#include "PackedPID.h"
#include "DevBoardESP8266.h"

// #define ESP_SSID "TP-LINK_62D8-702"              //"TP-LINK_hvac" "BlackBerry Hotspot"
// #define ESP_PASS "14/702/dfx"       // Your network password here "141242343""iblabwifi"   
// #define SERVER_ADDR "192.168.0.119"  // TCP服务器地址

#define ESP_SSID "BlackBerry Hotspot"              //"TP-LINK_hvac" "BlackBerry Hotspot"
#define ESP_PASS "141242343"           // Your network password here "141242343"
#define SERVER_ADDR "192.168.43.215"  // TCP服务器地址

// #define ESP_SSID "IBlab-Wifi"              //"TP-LINK_hvac" "BlackBerry Hotspot"
// #define ESP_PASS "iblabwifi"           // Your network password here "141242343"
// #define SERVER_ADDR "192.168.1.208"  // UDP服务器地址

#define UDP_SERVER_PORT 2021           // UDP服务器地址
#define UDP_LOCAL_PORT 1995           // UDP服务器地址

#define TCP_SERVER_PORT 1995           // TCP服务器地址

#define IF_WIFI false                   //是否Wi-Fi，若为否，直接从硬串口读取数据，适用于5G模块

DevBoardESP8266 wifi(&Serial1, &Serial, D3);
HardwareSerial Serial1(PA10, PA9);  // RX,TX

String tempBuffer = "";
boolean isTakeOver = true;
int writeAnalog;
long loopCount;  //循环计数，便于后续对比不同方法所得采样数据情况
const int SAMPLING_INTERVAL = 50;
const int OUTPUT_INTERVAL = 1000;

long reqId = -999;//显示是第几个包

boolean isNetwork = true;

AnalogWriter valveOut(A3, 12);  //模拟测试时，直接加Virtual即可
AnalogReader valveReader(A1, 12, 20);
// AnalogReader flowRateVolatageReader(A2, 12, 20);
// AnalogReader flowRateCurrentReader(A0, 12,20);       //本地直接采集流量数据
VirtualAnalogReader flowRateCurrentReader(A0, 1,isNetwork?1:20);    //通过网络获取流量数据


Mapper flowRateMapper(1, new double[2]{0.002962, -1.782667}, "FRM");
Mapper valveReadMapper(1, new double[2]{0.04219, -25.61501}, "VRM");
Mapper valveWriteMapper(1, new double[2]{40.232, -98.526}, "VWM");
IoTCtrlBoardManager ctrlManager;

Chrono sampleChrono;  //节拍器,采样用
Chrono outputChrono;  //节拍器,输出用

//输出保存结果用
double  flowrateMeasure,flowrateSetPoint,valveCtrl,valveOpening;

// PID pidController(&flowrateMeasure,&valveCtrl,&flowrateSetPoint,2,0.21,1.26,DIRECT);
// PackedPID packedPidCtrlPackedPID(&flowRateCurrentReader,&valveOut, 0,34.2,1.08,0,0);//阀门-流量控制
PackedPID packedPidCtrlPackedPID(&flowRateCurrentReader,&valveOut, 0,1.78,0.011,0,0);//阀门-出风温度控制

// TestSystem sys(1);

void setup() {
    Serial.begin(115200);
    Serial.println("Start setup!");

    if(IF_WIFI){
        wifi.hardReset();
    }
    

    // IO初始化
    pinMode(D6, OUTPUT);
    analogReadResolution(12);
    analogWriteResolution(12);
    digitalWrite(D6, HIGH);
    analogWrite(A3, 0);
    

    //映射器的命名

    //映射器加入端口
    valveOut.setMapper(&valveWriteMapper);
    valveReader.setMapper(&valveReadMapper);
    flowRateCurrentReader.setMapper(&flowRateMapper);

    //虚拟读取用
    // flowRateCurrentReader.setAcId("VM_FR");//virtual monitor flowrate
    // flowRateCurrentReader.setValueName("Qr_v");
    //虚拟读取送风温度
    flowRateCurrentReader.setAcId("VM_TS");//virtual monitor flowrate
    flowRateCurrentReader.setValueName("Ts_v");

    //控制器初始化
    //封装考虑优化一下
    packedPidCtrlPackedPID.setAcId("C_TS");//"C_FR"
    packedPidCtrlPackedPID.needCtrlByMapping(true);
    packedPidCtrlPackedPID.pidController.SetMode(MANUAL);
    packedPidCtrlPackedPID.pidController.SetSampleTime(OUTPUT_INTERVAL);
    packedPidCtrlPackedPID.pidController.SetOutputLimits(1, 100);

    packedPidCtrlPackedPID.setCtrlSetPointName("Ts");//Qs
    packedPidCtrlPackedPID.setCtrlInputName("Tsr");//Qr
    packedPidCtrlPackedPID.setCtrlOutputName("Vs");
    packedPidCtrlPackedPID.setCtrlReadActualOutName("Vr");
    packedPidCtrlPackedPID.setReadActualOutputPort(&valveReader);

    packedPidCtrlPackedPID.needSmoothinCtrl(!isNetwork); //采用网络控制不需要平滑

    //控制管理器初始化
    //压入控制部件
    ctrlManager.addMapper(&flowRateMapper);
    ctrlManager.addMapper(&valveReadMapper);
    ctrlManager.addController(&packedPidCtrlPackedPID);

    ctrlManager.addVirtualReader(&flowRateCurrentReader);

    ctrlManager.setTakeOverTriggerPin(D6);
    ctrlManager.setLoopCount(&loopCount);

    //无线模块初始化
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial1.setTimeout(20);  //需要控制串口读取数据的超时设置，否则无法分开多条数据
    Serial.setTimeout(20);


    while (!Serial) {
        Serial.println("Wait for init Serial...");
        ;  // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Debug Serial complete!");

    
    delay(1000);
    // set the data rate for the SoftwareSerial port
    while (!Serial1) {
        Serial.println("Wait for init Serial1...");
    }
    Serial.println(" Wi-Fi Serial initialized!");
    // wifi.hardReset();

    if(IF_WIFI){
        //若通过Wi-Fi模块进行传输，需在此处进行模块设置
        Serial.println(F("Connecting to WiFi..."));
        boolean flag = wifi.connectToAP(F(ESP_SSID), F(ESP_PASS));

        if (flag) {
             Serial.println("Connecting Success");
        } else {
            Serial.println("Connecting Failed");
        }
        delay(1000);
        // wifi.connectTCP(F(TCP_SERVER_ADDR), TCP_SERVER_PORT);
        wifi.connectUDP(F(SERVER_ADDR), UDP_SERVER_PORT, UDP_LOCAL_PORT);
        delay(2500);

        while(Serial1.available()){
            Serial.println(Serial1.readString());
        }

        wifi.setTransparentMode(true);
        delay(2500);
        while(Serial1.available()){
            Serial.println(Serial1.readString());
        }


    }


    // Wi-Fi连接测试
   

    // if(Serial1.available())
    flowrateMeasure = 0;
    valveCtrl = 0;
    Serial.println("Setup finished!");
    delay(1000);
}

void loop() {
    //串口命令检测
    if (Serial1.available()) {
        // Serial.println("something coming serial1");
        tempBuffer = Serial1.readStringUntil('#');
        //用特殊符号来操作似乎比空白符效果更好，可能空白符在发包的时候会被切掉
        tempBuffer.trim();
        if(tempBuffer=="")
            return;
        Serial.println(tempBuffer);  //"udp got"
        //管理器处理指令，在这一步中仅更新参数（如设定点等），不执行操作
        reqId=ctrlManager.commandDistributor(tempBuffer);
    }

    if(Serial.available()){
        // Serial.println("something coming serial");
        tempBuffer = Serial.readStringUntil('\n');
        tempBuffer.trim();
        Serial.println(tempBuffer);
        reqId=ctrlManager.commandDistributor(tempBuffer);
    }


    //采样部分
    if (sampleChrono.hasPassed(SAMPLING_INTERVAL)) {
        //高速采样
        // Serial.println("sampling time trigger");
        // valveReader.setVirtualAnalog(valveCtrl);  //模拟测试用，假设valveCtrl是模拟量
        valveReader.updatedReadAnalog();
        // flowRateVolatageReader.setVirtualAnalog(flowrateMeasure);  //模拟测试用，假设flowrateMeasure是模拟量
        // flowRateCurrentReader.updatedReadAnalog(); //网络更新时
        sampleChrono.restart();
        // Serial.println("sampling time finished");
    }

    //输出与执行部分
    if (outputChrono.hasPassed(OUTPUT_INTERVAL)) {
        //一直采样，但每一秒输出
        //其实这个做法不太好，最好能整合到CtrlBoardManager里面
        // Serial.println("output time trigger");
        flowrateMeasure = flowRateCurrentReader.readAnalogSmoothly(false,true,!isNetwork);  //仅在计算的时候更新PID的输入,是否平滑根据是否由网络控制决定
        flowrateSetPoint = packedPidCtrlPackedPID.getSetpoint();
        valveOpening = valveReader.readAnalogSmoothly(false, true);
        // Serial.println("hello in the act");
        // 控制器更新
        // pidController.Compute();
        valveCtrl=packedPidCtrlPackedPID.updatedControl();//在实际中可以不用赋值，PID直接进行输出了
        Serial.println(ctrlManager.showMeasuredStatus(true));
        outputChrono.restart();
        loopCount++;
    }
}

