#pragma once

#include "Coordinator.h"

#include <ArduinoJson.h>
#include <string.h>

#include "AgentBuffer.h"
//#include"AgentBuffer.cpp"
#include <pt.h>

#include "AgentProtocol.h"
//#include"AgentProtocol.cpp"
#include <math.h>

#include <vector>

// Coordinator类

Coordinator::Coordinator(String bdId, String bdType) : BaseCtrlComponent(bdId, bdType) {
    //本地缓存值初始化
    this->localLambda = -1;  // lambda本地缓存
    this->localConverge = false;

    //初始化AgentBufferList
    this->bufferListPool = std::vector<AgentBufferList>();
}
// Coordinator::Coordinator() {}
// Coordinator::~Coordinator() {}

Chrono Coordinator::getChrono() {
    return this->optChrono;
}

boolean Coordinator::isConverge(double meanChiTemp, double meanCoolingTowerTemp) {
    //计算是否收敛
    if (this->getPoolSize() < 2)
        return false;  //如果当前的pool小于2，显然不会有两类设备
    return fabs(meanCoolingTowerTemp - meanChiTemp) < 0.05;
}

double Coordinator::compLambda(double meanChiTemp, double meanCoolingTowerTemp) {
    if (this->localConverge)
        return localLambda;
    //计算lamda值
    return localLambda + 0.00000005 * 4.2 * 410.6 *
                             this->getListFromPoolByType(AgentProtocol::TYPE_CHILLER)->listSize() *
                             (meanCoolingTowerTemp - meanChiTemp);
}

String Coordinator::coordinateCalculate() {
    // 计算coordinator的数据
    this->jsonData.clear();
    strBuffer = "";
    this->timeBuffer = micros();
    //计算一些前置参数
    double meanChiTemp = this->meanValueByList(AgentProtocol::TYPE_CHILLER);
    double meanCoolingTowerTemp = this->meanValueByList(AgentProtocol::TYPE_COOLING_TOWER);

    this->localConverge = isConverge(meanChiTemp, meanCoolingTowerTemp);
    jsonData[AgentProtocol::DATA_ISCONV_FROM_JSON] = this->localConverge;
    this->localLambda = compLambda(meanChiTemp, meanCoolingTowerTemp);
    jsonData[AgentProtocol::DATA_LAMBDA_FROM_JSON] = this->localLambda;
    this->timeBuffer = micros() - this->timeBuffer;
    serializeJson(jsonData, strBuffer);
    return strBuffer;
}

String Coordinator::packCoordinatorData() {
    //发送coordinator的信息
    jsonOut.clear();
    jsonOut[AgentProtocol::DEV_ID_FROM_JSON] = this->boardId;
    jsonOut[AgentProtocol::DEV_TYPE_FROM_JSON] = this->boardType;
    jsonOut[AgentProtocol::CMD_TYPE_FROM_JSON] = "SEND";  //目前统统都是send
    jsonOut[AgentProtocol::REQ_ID_FROM_JSON] = ++this->reqId;
    jsonOut[AgentProtocol::RESP_ID_FROM_JSON] = -1;  // AgentProtocol::RESP_ID_FROM_JSON
    if (!canStartCoordinateCaculate()) {
        //如果不能直接返回一个随便的
        jsonOut[AgentProtocol::DATA_FROM_JSON] = "{\"cv\":false,\"lm\":-1}";
        jsonOut[AgentProtocol::COMPUTE_TIME_FROM_JSON] = -1;
    } else {
        jsonOut[AgentProtocol::DATA_FROM_JSON] = coordinateCalculate();
        jsonOut[AgentProtocol::COMPUTE_TIME_FROM_JSON] = this->timeBuffer;
    }
    // Arduino Uno上，精度为4微秒
    strBuffer = "";                     // serialize对字符串只能追加
    serializeJson(jsonOut, strBuffer);  //用scoop库会报错
    return strBuffer;
}

int Coordinator::threadCoordinate(struct pt* pt) {
    // coordinator线程
    PT_BEGIN(pt);
    while (true) {
        PT_WAIT_UNTIL(pt, this->optChrono.hasPassed(this->optPeriod));
        this->needBlink = true;
        optChrono.restart();                          //重置计时器
        sendMessage(packCoordinatorData() + "\r\n");  //发送数据
        needBlink = false;
    }
    PT_END(pt);  //这个宏定义的真是牛皮，宏里面带个大括号
}

void Coordinator::sendMessage(String msg) {
    Serial.println(msg);  //通过串口发送消息
}

void Coordinator::debugPrint(String str) {
    Serial.println("In debug: " + str);  //通过串口发送消息
}

void Coordinator::addToBufferList(AgentBuffer ab) {
    //暂不进行其他处理
    //留个空出来，如果需要对比如接收到agent类型进行判断则在此处操作
    int i = indexOfListType(ab.getBoardType());
    // Serial.println("debug: index is " + String(i) + " ab.getBoardType() is "
    // + ab.getBoardType());
    if (i == -1) {
        // pool中不存在该类型的list，需要新建
        // Serial.println("List 不存在该类型，构建中：" + ab.getBoardType());
        AgentBufferList ablTemp(ab.getBoardType());
        ablTemp.add(ab);

        this->bufferListPool.push_back(ablTemp);  //压入一个AgentBufferList

        //压不进去？？？

        // this->bufferListPool[bufferListPool.size() -
        // 1].updateAgentBuffer(ab); Serial.println("List 不存在该类型，已加入,
        // 当前size为" + String(this->bufferListPool.size()) + 	" current BLP is
        // " + bufferListPool.empty());
        return;
    } else
        this->bufferListPool[i].updateAgentBuffer(ab);  //这个操作直接封装在AgentBufferList中，不在Coordinator进行判断
}

void Coordinator::debugListPrint() {
    Serial.println("In debugListPrint: current BLP is " + String(bufferListPool.empty()) + " listType size is " +
                   String(this->bufferListPool.size()));

    // for (int j = 0;j < this->bufferListPool.size();j++) {
    // 	//显示当前list的类型
    // 	Serial.println("In " + this->bufferListPool[j].getListType() + " list");
    // 	for (int i = 0;i < bufferListPool[j].listSize();i++) {
    // 		//显示list每一项的信息
    // 		Serial.println(String(i) + " buffer: boardId: " +
    // this->bufferListPool[j].getAgentBuffer(i)->getBoardId() + 			" reqId: " +
    // this->bufferListPool[j].getAgentBuffer(i)->getReqId());
    // 	}
    // }
}

int Coordinator::indexOfListType(String bdType) {
    for (int i = 0; i < this->bufferListPool.size(); i++) {
        // Serial.println("debug: in indexOfListType: current is " + String(i) +
        // " BLP size is " + String(this->bufferListPool.size()));
        if (this->bufferListPool[i].getListType() == bdType)
            return i;
    }
    return -1;  //没有命中返回-1
}

int Coordinator::getPoolSize() {
    //返回BufferListPool的size
    return this->bufferListPool.size();
}

AgentBufferList* Coordinator::getListFromPoolById(int i) {
    if (i >= bufferListPool.size())
        return nullptr;
    return &(this->bufferListPool[i]);
}

AgentBufferList* Coordinator::getListFromPoolByType(String ty) {
    if (this->bufferListPool.size() == 0)
        return nullptr;
    for (int i = 0; i < bufferListPool.size(); i++) {
        if (this->bufferListPool[i].getListType() == ty)
            return &(this->bufferListPool[i]);
    }
    return nullptr;
}

int Coordinator::getTotalAgentNumber() {
    int sum = 0;
    for (int i = 0; i < this->bufferListPool.size(); i++) {
        sum = sum + bufferListPool[i].listSize();
    }
    return sum;
}

double Coordinator::meanValueByList(String type) {
    AgentBufferList* list = this->getListFromPoolByType(type);
    if (list == nullptr)
        return -999;
    return list->meanValue();
}

boolean Coordinator::canStartCoordinateCaculate() {
    if (this->bufferListPool.size() == 0)
        return false;  // pool里面起码不能为空
    if (this->getListFromPoolByType(AgentProtocol::TYPE_CHILLER) == nullptr ||
        this->getListFromPoolByType(AgentProtocol::TYPE_COOLING_TOWER) == nullptr)
        return false;  //显然两者起码不能同时为空
    return true;
}