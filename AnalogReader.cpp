#pragma once
#include "AnalogReader.h"

AnalogReader::AnalogReader(int port, int res=8):AnalogIOPort(port,res){
    movCacu=NumericMovingAverageCaculator(7);
    pinMode(port, INPUT_ANALOG);
}

void setSmoothWindowSize(int size);

double AnalogReader::readAnalogTool(){
    //直接跟IO交互的函数
    //读取的同时仍维持队列的更新
    double value=(double)analogRead(this->portNo);
    this->movCacu.append((double)value);
    return value;
}

//除了直接读取外，默认均不与IO口进行交互，仅通过队列处理
double AnalogReader:: readAnalogByMapping(boolean needUpdated=false){//读取模拟输出至百分比
    if(needUpdated)
        readAnalogTool();
    return mappingValue(this->movCacu.getNewestElement());
}

double AnalogReader:: readAnalogDirectly(boolean needUpdated=false){//读取模拟输出至百分比
    if(needUpdated)
        readAnalogTool();
    return this->movCacu.getNewestElement();
}

double AnalogReader::readAnalogSmoothly(boolean needUpdated=false,boolean needMapping=false){
    if(needUpdated)
        readAnalogTool();
    return needMapping?mappingValue(this->movCacu.getAverage()):this->movCacu.getAverage();
}
