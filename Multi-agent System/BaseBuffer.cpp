#pragma once
// #include <uClibc++.h>
#include "BaseBuffer.h"
#include <Arduino.h>
#include <string.h>
#include "AgentProtocol.h"

BaseBuffer::BaseBuffer() {
    this->rqId = -999;
}

BaseBuffer::BaseBuffer(String id, String bdType, long rq) : boardId(id), boardType(bdType), rqId(rq) {}

String BaseBuffer::getBoardId() {
    return this->boardId;
}
long BaseBuffer::getReqId() {
    return this->rqId;
}
String BaseBuffer::getBoardType() {
    return this->boardType;
}