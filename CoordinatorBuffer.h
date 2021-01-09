#pragma once
// #include <uClibc++.h>
#include<Arduino.h>
#include <string.h>
#include "AgentProtocol.h"
#include "BaseBuffer.h"
#include<ArduinoJson.h>
//#include "BaseBuffer.cpp"



class CoordinatorBuffer :public BaseBuffer {
	//需要在Agent中缓存的数据
	//这个CoordinatorBuffer 意思是Agent处理从Coordinator来的buffer，不光是信息
private:
	boolean isConv;
	double lambda;//作为实际需要的数据

public:
	CoordinatorBuffer();
	CoordinatorBuffer(String id, String type, long rq, boolean isConv, double lambda);

	boolean getIsConverge();
	void setIsConverge(boolean isCv);

	double getLambda();
	void setLambda(double lm);

	static CoordinatorBuffer msgToCoordinatorBuffer(AgentMsg msg);
	static CoordinatorBuffer msgToCoordinatorBuffer(AgentMsg msg, JsonDocument* jd);
};