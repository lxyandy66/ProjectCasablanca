
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
#include "BaseCtrlComponent.h"

//作为所有MAS中Agent的基类，包含网络控制相关应用

class BaseNetworkCtrlComponent: public BaseLocalWirelessClient {
   protected:

    AgentMsg msgBuffer;                                           //日后其他NCS系统都可以通用该消息类
    StaticJsonDocument<AgentProtocol::MSG_SIZE> jsonOut;          //发送的buffer
    StaticJsonDocument<AgentProtocol::MSG_SIZE> jsonInputBuffer;  //接收的buffer
    StaticJsonDocument<AgentProtocol::DATA_SIZE> jsonData;        //采集数据的buffer

   public:
    BaseNetworkCtrlComponent(String bdId, String bdType);
    virtual void sendMessage(String msg);


    virtual JsonDocument* getInputBuffer();
    virtual JsonDocument* getOutputBuffer();
    virtual JsonDocument* getDataBuffer();
};