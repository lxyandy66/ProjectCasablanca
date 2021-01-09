#pragma once
#include"AgentProtocol.h"
#include"Agent.h"


Agent::Agent(String bdId, String bdType) :CtrlComponent(bdId, bdType) {}


void Agent::setWifiModule(DevBoardESP8266 wifi) {
	this->wifiModule = wifi;
}
void Agent::sendMessage(String msg) {
	// this->wifiModule.sendContent(msg);
	this->sendOutput->println("Send: " + msg);//暂时先这样
}

void Agent::setSendOutput(Stream* s) {
	this->sendOutput = s;
}

void Agent::debugPrint(String str) {
	Serial.println("In debug: " + str);//仅用于debug
}

String Agent::agentCaculate() {
	// 计算coordinator的数据
	this->jsonData.clear();
	strBuffer = "";
	this->timeBuffer = micros();
	jsonData[AgentProtocol::DATA_TEMP_FROM_JSON] = compTemp();
	this->timeBuffer = micros() - this->timeBuffer;
	serializeJson(jsonData, strBuffer);
	return strBuffer;
}

String Agent::packAgentData() {
	//发送agent的信息
	jsonOut.clear();
	jsonOut[AgentProtocol::DEV_ID_FROM_JSON] = this->boardId;
	jsonOut[AgentProtocol::DEV_TYPE_FROM_JSON] = this->boardType;
	jsonOut[AgentProtocol::CMD_TYPE_FROM_JSON] = "SEND";//目前统统都是send
	jsonOut[AgentProtocol::REQ_ID_FROM_JSON] = this->reqId;
	jsonOut[AgentProtocol::RESP_ID_FROM_JSON] = this->respId++;//AgentProtocol::RESP_ID_FROM_JSON
	jsonOut[AgentProtocol::DATA_FROM_JSON] = agentCaculate();
	jsonOut[AgentProtocol::COMPUTE_TIME_FROM_JSON] = this->timeBuffer;//Arduino Uno上，精度为4微秒
	strBuffer = "";//serialize对字符串只能追加
	serializeJson(jsonOut, strBuffer);//用scoop库会报错
	return strBuffer;
}

double Agent::compTemp() {
	return random(250, 350) / 10.0;
}

void Agent::addToBuffer(CoordinatorBuffer cb) {
	//对新解析的buffer进行处理
	//逻辑需要考虑，尤其是出现延迟的处理，即ReqId滞后或重传的处理
	if (cb.getReqId() < this->coBuffer.getReqId())
		//如果reqId小于则直接丢弃
		return;
	if (cb.getIsConverge()) {
		//如果收到的信息表明已经收敛，只更新reqId并缓存
		this->coBuffer = cb;
		this->reqId = cb.getReqId();
		return;
	}
	//执行到此表明收到新的信息，且没有收敛
	this->coBuffer = cb;
	this->reqId = cb.getReqId();
	this->respId = 0;
	this->parseBuffer(this->coBuffer);
}


void Agent::parseBuffer(CoordinatorBuffer cb) {
	//parse和add我觉得要重新处理一下
	this->sendMessage(packAgentData());//发送打包的data
}