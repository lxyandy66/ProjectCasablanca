#pragma once

#include<Arduino.h>
#include <string.h>
#include "AgentProtocol.h"
#include "BaseBuffer.h"
#include"CoordinatorBuffer.h"
#include<ArduinoJson.h>


CoordinatorBuffer::CoordinatorBuffer() {}
CoordinatorBuffer::CoordinatorBuffer(String id, String type, long rq, boolean isCv, double lm) :
	BaseBuffer(id, type, rq), isConv(isCv), lambda(lm) {}


boolean CoordinatorBuffer::getIsConverge() { return this->isConv; }
void CoordinatorBuffer::setIsConverge(boolean isCv) { this->isConv = isCv; }

double CoordinatorBuffer::getLambda() { return this->lambda; }
void CoordinatorBuffer::setLambda(double lm) { this->lambda = lm; }


CoordinatorBuffer CoordinatorBuffer::msgToCoordinatorBuffer(AgentMsg msg) {
	//不推荐使用，用动态Json解析对象比较慢
	DynamicJsonDocument jsonTemp(AgentProtocol::DATA_SIZE);
	deserializeJson(jsonTemp, msg.data);
	return CoordinatorBuffer(msg.boardId, msg.boardType, msg.reqId,
		jsonTemp[AgentProtocol::DATA_ISCONV_FROM_JSON].as<boolean>(),
		jsonTemp[AgentProtocol::DATA_LAMBDA_FROM_JSON].as<double>());
}

CoordinatorBuffer CoordinatorBuffer::msgToCoordinatorBuffer(AgentMsg msg, JsonDocument* jd) {
	jd->clear();
	deserializeJson((*jd), msg.data);
	return CoordinatorBuffer(msg.boardId, msg.boardType, msg.reqId,
		(*jd)[AgentProtocol::DATA_ISCONV_FROM_JSON].as<boolean>(),
		(*jd)[AgentProtocol::DATA_LAMBDA_FROM_JSON].as<double>());
}
