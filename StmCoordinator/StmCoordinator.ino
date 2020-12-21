
#include <pt.h>



#include <Chrono.h>
#include <LightChrono.h>

#include <DHT.h>
#include <stdlib.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include"../Coordinator.h"
#include"../Coordinator.cpp"
#include"../CtrlComponent.h"
#include"../CtrlComponent.cpp"

#define PIN_OLED_SCL A4
#define PIN_OLED_SDA A6 //MOSI
#define PIN_OLED_DC A5
#define PIN_DHT_DATA A3
#define PIN_OLED_RST A2
#define DHT_TYPE DHT22
#define PIN_SS_RX D10
#define PIN_SS_TX D11

#define PIN_LED D2


//用于测试与JAVA程序通信程序

//初始化显示屏
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ PIN_OLED_SCL,
  /* data=*/ PIN_OLED_SDA, /* cs=*/ U8X8_PIN_NONE, /* dc=*/ PIN_OLED_DC, /* reset=*/ PIN_OLED_RST);

//初始化温湿度传感器
DHT dht22(PIN_DHT_DATA, DHT_TYPE);

String cmd;
String tempBuffer;
String tempDataBuffer;
boolean ledOn = false;
const String deviceID = "Coordinator_1";
const String deviceType = "CoolingTower";

// extern "C" 
Coordinator co(deviceID, deviceType); //？？？？




static struct pt trCoordinate;//coordinate线程指针
static struct pt trBlinker;//LED提示用


void printString(String cmd) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 20, cmd.c_str());
  u8g2.sendBuffer();
}




void processCmd(String cmd) {
  Serial.print("CMD: " + cmd);
  if (cmd.indexOf("SET+LED") != -1) {
    // co.changeLed();
    return;
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("BEGIN!");
  pinMode(PIN_LED, OUTPUT);
  u8g2.begin();
  co.setLedPin((unsigned long)PIN_LED);
  Serial.println("Hello! after setledPin");
  printString("Hello!");
  // PT_INIT(&trCoordinate);
  PT_INIT(&trBlinker);
}
void loop() {
  // co.threadCoordinate(&trCoordinate);//启动coordinate线程，每秒进行优化
  // co.threadBlinker(&trBlinker);//启动blinker线程，优化时点亮LED
  while (Serial.available()) {
    cmd = Serial.readString();
    ledOn = !ledOn;
    co.setNeedBlink(ledOn);
    Serial.println("led should be " + String(ledOn ? "On" : "Off"));
    Serial.println("CMD: " + cmd);
    AgentMsg tempMsg = AgentProtocol::parseFromString(cmd);
    // printString(cmd);
    if (tempMsg.boardId != "") {
      Serial.println("received cmdType: " + tempMsg.cmdType);
      co.addToBufferList(AgentBuffer::msgToAgentBuffer(tempMsg, co.getInputBuffer()));
    }
    co.debugListPrint();
    // Serial.
    processCmd(cmd);//接收并格式化JSON对象，更新list
  }
  // Serial.println("out of loop");
}

