#pragma once

#include "Coordinator.h"
#include<ArduinoJson.h>
#include <string.h>
#include"AgentBuffer.h"
//#include"AgentBuffer.cpp"
#include<pt.h>
#include"AgentProtocol.h"
//#include"AgentProtocol.cpp"
#include<vector>




//Coordinator类


Coordinator::Coordinator(String bdId, String bdType) :CtrlComponent(bdId, bdType) {
	//仅初始化AgentBufferList
	this->bufferListPool = std::vector<AgentBufferList>();
}
// Coordinator::Coordinator() {}
// Coordinator::~Coordinator() {}

Chrono Coordinator::getChrono() { return this->optChrono; }


boolean Coordinator::isConverge() {
	//计算是否收敛
	return true;
}
double Coordinator::compLambda() {
	return 0.01;//计算lamda值
}

String Coordinator::coordinateCalculate() {
	// 计算coordinator的数据
	this->jsonData.clear();
	strBuffer = "";
	this->timeBuffer = micros();
	jsonData[AgentProtocol::DATA_ISCONV_FROM_JSON] = isConverge();
	jsonData[AgentProtocol::DATA_LAMBDA_FROM_JSON] = compLambda();
	this->timeBuffer = micros() - this->timeBuffer;
	serializeJson(jsonData, strBuffer);
	return strBuffer;
}

String Coordinator::packCoordinatorData() {
	//发送coordinator的信息
	jsonOut.clear();
	jsonOut[AgentProtocol::DEV_ID_FROM_JSON] = this->boardId;
	jsonOut[AgentProtocol::DEV_TYPE_FROM_JSON] = this->boardType;
	jsonOut[AgentProtocol::CMD_TYPE_FROM_JSON] = "SEND";//目前统统都是send
	jsonOut[AgentProtocol::REQ_ID_FROM_JSON] = ++this->reqId;
	jsonOut[AgentProtocol::RESP_ID_FROM_JSON] = -1;//AgentProtocol::RESP_ID_FROM_JSON
	jsonOut[AgentProtocol::DATA_FROM_JSON] = coordinateCalculate();
	jsonOut[AgentProtocol::COMPUTE_TIME_FROM_JSON] = this->timeBuffer;//Arduino Uno上，精度为4微秒
	strBuffer = "";//serialize对字符串只能追加
	serializeJson(jsonOut, strBuffer);//用scoop库会报错
	return strBuffer;
}


int Coordinator::threadCoordinate(struct pt* pt) {
	//coordinator线程
	PT_BEGIN(pt);
	while (true) {
		PT_WAIT_UNTIL(pt, this->optChrono.hasPassed(this->optPeriod));
		this->needBlink = true;
		optChrono.restart();//重置计时器
		sendMessage(packCoordinatorData());//发送数据
		needBlink = false;
	}
	PT_END(pt);//这个宏定义的真是牛皮，宏里面带个大括号
}


void Coordinator::sendMessage(String msg) {
	Serial.println(msg);//通过串口发送消息
}

void Coordinator::debugPrint(String str) {
	Serial.println("In debug: " + str);//通过串口发送消息
}


void Coordinator::addToBufferList(AgentBuffer ab) {
	//暂不进行其他处理
	//留个空出来，如果需要对比如接收到agent类型进行判断则在此处操作
	int i = indexOfListType(ab.getBoardType());
	// Serial.println("debug: index is " + String(i) + " ab.getBoardType() is " + ab.getBoardType());
	if (i == -1) {
		// pool中不存在该类型的list，需要新建
		// Serial.println("List 不存在该类型，构建中：" + ab.getBoardType());
		AgentBufferList ablTemp(ab.getBoardType());
		ablTemp.add(ab);

		this->bufferListPool.push_back(ablTemp);//压入一个AgentBufferList

		//压不进去？？？

		// this->bufferListPool[bufferListPool.size() - 1].updateAgentBuffer(ab);
		// Serial.println("List 不存在该类型，已加入, 当前size为" + String(this->bufferListPool.size()) +
		// 	" current BLP is " + bufferListPool.empty());
		return;
	}
	else
		this->bufferListPool[i].updateAgentBuffer(ab);//这个操作直接封装在AgentBufferList中，不在Coordinator进行判断
}

void Coordinator::debugListPrint() {
	Serial.println("In debugListPrint: current BLP is " + String(bufferListPool.empty()) +
		" listType size is " + String(this->bufferListPool.size()));

	// for (int j = 0;j < this->bufferListPool.size();j++) {
	// 	//显示当前list的类型
	// 	Serial.println("In " + this->bufferListPool[j].getListType() + " list");
	// 	for (int i = 0;i < bufferListPool[j].listSize();i++) {
	// 		//显示list每一项的信息
	// 		Serial.println(String(i) + " buffer: boardId: " + this->bufferListPool[j].getAgentBuffer(i)->getBoardId() +
	// 			" reqId: " + this->bufferListPool[j].getAgentBuffer(i)->getReqId());
	// 	}
	// }
}

int Coordinator::indexOfListType(String bdType) {
	for (int i = 0;i < this->bufferListPool.size();i++) {
		// Serial.println("debug: in indexOfListType: current is " + String(i) + " BLP size is " + String(this->bufferListPool.size()));
		if (this->bufferListPool[i].getListType() == bdType)
			return i;
	}
	return -1;//没有命中返回-1
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