#pragma once
#include "BaseAgent.h"
#include <pt.h>
#include "AgentProtocol.h"

BaseAgent::BaseAgent(String bdId, String bdType) : BaseLocalWirelessClient(bdId, bdType), reqId(-1), respId(0) {}

CoordinatorBuffer BaseAgent::getCurrentBuffer() {
    return this->coBuffer;
}

void BaseAgent::sendMessage(String msg) {
    // this->wifiModule.sendContent(msg);
    this->sendOutput->println("Send: " + msg);  //暂时先这样
}

void BaseAgent::debugPrint(String str) {
    Serial.println("In debug: " + str);  //仅用于debug
}

String BaseAgent::packDataStr() {
    // 计算coordinator的数据
    this->jsonData.clear();
    strBuffer = "";
    this->timeBuffer = micros();
    jsonData[AgentProtocol::DATA_TEMP_FROM_JSON] = compTemp();
    this->timeBuffer = micros() - this->timeBuffer;
    serializeJson(jsonData, strBuffer);
    return strBuffer;
}

String BaseAgent::packAgentMsgStr() {
    //发送agent的信息
    jsonOut.clear();
    jsonOut[AgentProtocol::DEV_ID_FROM_JSON] = this->boardId;
    jsonOut[AgentProtocol::DEV_TYPE_FROM_JSON] = this->boardType;
    jsonOut[AgentProtocol::CMD_TYPE_FROM_JSON] = "SEND";  //目前统统都是send
    jsonOut[AgentProtocol::REQ_ID_FROM_JSON] = this->reqId;
    jsonOut[AgentProtocol::RESP_ID_FROM_JSON] = this->respId;  // AgentProtocol::RESP_ID_FROM_JSON
    jsonOut[AgentProtocol::DATA_FROM_JSON] = packDataStr();
    jsonOut[AgentProtocol::COMPUTE_TIME_FROM_JSON] = this->timeBuffer;  // Arduino Uno上，精度为4微秒
    strBuffer = "";                                                     // serialize对字符串只能追加
    serializeJson(jsonOut, strBuffer);                                  //用scoop库会报错
    return strBuffer;
}

double BaseAgent::compTemp() {
    return -999;
}

void BaseAgent::setParameter() {}
void BaseAgent::setEnvironmentParameter(double wb) {}

boolean BaseAgent::addToBuffer(CoordinatorBuffer cb) {
    //对新解析的buffer进  行处理，通过返回值判断是否需要进行计算并发送

    //逻辑需要考虑，尤其是出现延迟的处理，即ReqId滞后或重传的处理（暂时直接丢弃

    this->debugPrint("RecCB: " + cb.whoAmI());
    if (cb.getReqId() < this->reqId)
        //如果reqId小于则直接丢弃
        return false;
    if (cb.getIsConverge()) {
        //如果收到的信息表明已经收敛，只更新reqId并缓存
        this->coBuffer = cb;
        this->reqId = cb.getReqId();
        this->respId++;
        return false;
    }
    //执行到此表明收到新的信息，且没有收敛
    this->coBuffer = cb;
    this->reqId = cb.getReqId();
    this->respId = 0;
    return true;
    // this->parseBuffer(this->coBuffer);
}

void BaseAgent::parseBuffer(CoordinatorBuffer cb) {
    // parse和add我觉得要重新处理一下
    this->sendMessage(packAgentMsgStr());  //发送打包的data
}

int BaseAgent::threadAgent(struct pt* pt, String msg) {
    // coordinator线程
    PT_BEGIN(pt);
    // this->msgBuffer=*nullptr;
    this->msgBuffer = AgentProtocol::parseFromString(msg);
    debugPrint("this is a msg: [" + msg + "] length: " + msg.length());
    if (!AgentProtocol::isVaildMsg(this->msgBuffer)) {
        this->debugPrint("No need to compute: LocalReq: " + String(this->reqId) +
                         " RecReq: " + String(this->getCurrentBuffer().getReqId()));
        return -1;
    }

    if (!this->addToBuffer(CoordinatorBuffer::msgToCoordinatorBuffer(this->msgBuffer, this->getInputBuffer()))) {
        //如果检测到加入的buffer无需进行计算
        this->debugPrint(this->getCurrentBuffer().whoAmI());
        this->debugPrint("Invaild Msg! " + msg);
    } else {
        this->debugPrint(this->getCurrentBuffer().whoAmI());
        //发送的逻辑要考虑考虑
        this->strBuffer = this->packAgentMsgStr();
        // this->debugPrint("Data will be sent: " + this->strBuffer);
        this->wifiModule.sendContentDirectly(this->strBuffer);  // processCmd(a);
        // this->debugPrint("Send Success!");
    }
    PT_END(pt);  //这个宏定义的真是牛皮，宏里面带个大括号
}