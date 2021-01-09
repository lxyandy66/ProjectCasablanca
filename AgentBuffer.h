#pragma once
// #include <uClibc++.h>
#include<Arduino.h>
#include <string.h>
#include <vector>
#include <pt.h>
#include "AgentMsg.h"
#include "AgentProtocol.h"
#include "CtrlComponent.h"
#include <Chrono.h>
#include <LightChrono.h>
#include<ArduinoJson.h>
#include"BaseBuffer.h"


class AgentBuffer :public BaseBuffer {
	//需要在coordinator中缓存的数据
	//这个AgentBuffer 意思是Coordinator处理从agent来的buffer，不光是信息
private:
	double data;//作为实际需要的数据

public:
	AgentBuffer();
	AgentBuffer(String id, String bdType, long rq, double temp);
	double getData();
	void setData(double dt);
	static AgentBuffer msgToAgentBuffer(AgentMsg msg);
	static AgentBuffer msgToAgentBuffer(AgentMsg msg, JsonDocument* jd);
};


class AgentBufferList {
private:
	std::vector<AgentBuffer> list;//作为实际缓存的容器
	boolean isLegalIndex(int i);
	String listType;//用于表明该list对应的是什么类型的设备

public:
	AgentBufferList(String lt);

	String getListType();
	void setListType(String str);

	double getDataById(String id);
	int findAgentById(String id);
	void addAgent(String id);
	AgentBuffer* getAgentBuffer(String id);
	AgentBuffer* getAgentBuffer(int index);
	int listSize();
	void add(AgentBuffer ab);
	void updateAgentBuffer(AgentBuffer ab, int index);//更新对应的boardId的buffer
	void updateAgentBuffer(AgentBuffer ab);
	boolean isNewerThanExist(AgentBuffer ab, int index);

};
