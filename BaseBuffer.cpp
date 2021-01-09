#pragma once
// #include <uClibc++.h>
#include<Arduino.h>
#include <string.h>
#include "AgentProtocol.h"
#include "BaseBuffer.h"

BaseBuffer::BaseBuffer() {}
BaseBuffer::BaseBuffer(String id, String bdType, long rq) :boardId(id), boardType(bdType), rqId(rq) {}

String BaseBuffer::getBoardId() { return this->boardId; }
int BaseBuffer::getReqId() { return this->rqId; }
String BaseBuffer::getBoardType() { return this->boardType; }