
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Chrono.h>
#include <LightChrono.h>
#include <pt.h>
#include <string.h>
#include <vector>
#include "AgentMsg.h"
#include "AgentProtocol.h"

//作为所有板子的基类，考虑与MAS无关，即所有日后的NCS

class BaseCtrlComponent {
   protected:
    unsigned long pinLed;  // LED不能少
    boolean needBlink;     // LED提示变量
    String boardId;
    String boardType;
    long timeBuffer;
    String strBuffer;

    AgentMsg msgBuffer;                                           //日后其他NCS系统都可以通用该消息类
    StaticJsonDocument<AgentProtocol::MSG_SIZE> jsonOut;          //发送的buffer
    StaticJsonDocument<AgentProtocol::MSG_SIZE> jsonInputBuffer;  //接收的buffer
    StaticJsonDocument<AgentProtocol::DATA_SIZE> jsonData;        //采集数据的buffer

   public:
    BaseCtrlComponent(String bdId, String bdType);
    void setBoardId(String str);
    String getBoardId();
    void setBoardType(String str);
    String getBoardType();
    virtual void sendMessage(String msg) = 0;
    virtual void debugPrint(String str) = 0;  //用于开发中debug的显示

    /*LED相关*/
    int threadBlinker(struct pt* pt);                               // blinker线程,通过needBlink异步控制LED
    int threadBlinkerOnce(struct pt* pt, int blinkDuration = 500);  // blinker线程,不阻塞主线程情况闪烁一次

    void setLedPin(unsigned long p);
    unsigned long getLedPin();
    void setNeedBlink(boolean b);
    boolean getNeedBlink();
    void changeLed(boolean b);
    void changeLed();

    virtual JsonDocument* getInputBuffer();
    virtual JsonDocument* getOutputBuffer();
    virtual JsonDocument* getDataBuffer();
};