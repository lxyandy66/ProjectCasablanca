

#include <pt.h>

#include <Chrono.h>
#include <LightChrono.h>

#include <ArduinoJson.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <stdlib.h>
#include <string.h>
#include "Coordinator.h"
//#include<Coordinator.cpp>
//#include"MasComponent.h"
//#include"MasComponent.cpp"

//#include"../Coordinator.cpp"
//#include"../MasComponent.h"
//#include"../MasComponent.cpp"
//#include"../Coordinator.h"
//#include"../Coordinator.cpp"
//#include"../MasComponent.h"
//#include"../MasComponent.cpp"

#define PIN_OLED_SCL A4
#define PIN_OLED_SDA A6  // MOSI
#define PIN_OLED_DC A5
#define PIN_DHT_DATA A3
#define PIN_OLED_RST D12
#define DHT_TYPE DHT22
#define PIN_SS_RX D10
#define PIN_SS_TX D11

#define PIN_LED D2

//用于测试与JAVA程序通信程序

//初始化显示屏
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0,
                                            /* clock=*/PIN_OLED_SCL,
                                            /* data=*/PIN_OLED_SDA,
                                            /* cs=*/U8X8_PIN_NONE,
                                            /* dc=*/PIN_OLED_DC,
                                            /* reset=*/PIN_OLED_RST);

String cmd;
String tempBuffer;
String tempDataBuffer;
boolean ledOn = false;

// extern "C"
Coordinator co("Co_1", AgentProtocol::TYPE_COORDINATOR);

static struct pt trCoordinate;  // coordinate线程指针
static struct pt trBlinker;     // LED提示用

void printString(String cmd) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 20, cmd.c_str());
    u8g2.sendBuffer();
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(500);
    // Serial.println("BEGIN!");
    pinMode(PIN_LED, OUTPUT);
    u8g2.begin();

    // coordinator初始值和引脚配置
    co.setParameter(0.0151, false);
    co.setLedPin((unsigned long)PIN_LED);

    printString("Hello!");

    PT_INIT(&trCoordinate);
    PT_INIT(&trBlinker);
    delay(10000);  //延迟10秒等待数据库
    co.threadBlinkerOnce(&trBlinker);
}
void loop() {
    co.threadCoordinate(&trCoordinate);  //启动coordinate线程，每秒进行优化
    while (Serial.available()) {
        cmd = Serial.readString();
        AgentMsg tempMsg = AgentProtocol::parseFromString(cmd);
        // printString(cmd);
        if (AgentProtocol::isVaildMsg(tempMsg)) {
            co.threadBlinkerOnce(&trBlinker);  //收到合法的消息时闪烁一次
            co.addToBufferList(AgentBuffer::msgToAgentBuffer(tempMsg, co.getInputBuffer()));
        }
        printString("DevType:" + String(co.getPoolSize()));
        // co.debugListPrint();
        // Serial.
        // processCmd(cmd);//接收并格式化JSON对象，更新list
    }
    // Serial.println("out of loop");
}
