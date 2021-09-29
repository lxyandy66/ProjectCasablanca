#pragma once
#include "AnalogReader.h"

double AnalogReader::readAnalogTool(){
    //读取的同时仍维持队列的更新
    return -999;
}

double AnalogReader:: readAnalog(){//读取模拟输出至百分比
    return (analogRead(portNo)-this->lower) / (this->upper-this->lower);
}


double AnalogReader:: readDirectAnalog(){//读取模拟输出至百分比
    return analogRead(portNo);
}
