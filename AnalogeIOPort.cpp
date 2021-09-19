#pragma once
#include "AnalogeIOPort.h"

AnalogeIOPort::AnalogeIOPort(int port) : portNo(port){
    this->resolutionBit = 8;//默认为8位
    setOutputRange();
    analogReadResolution(this->resolutionBit);
    analogWriteResolution(this->resolutionBit);
}

AnalogeIOPort::AnalogeIOPort(int port,int res) : portNo(port),resolutionBit(res)  {
    setOutputRange();
    analogReadResolution(this->resolutionBit);
    analogWriteResolution(this->resolutionBit);
}

void AnalogeIOPort::outputAnalog(double outputValue) {//直接按照百分比输出，具体输出值由内部函数确定
    int actualOut = outputAnalogTool(limitRange(outputValue, 0, 100));
    Serial.println("Actual outSet: "+String(actualOut));
    analogWrite(portNo, actualOut);
}

int AnalogeIOPort:: outputAnalogTool(double limitedValue) {
    return lower + round(limitedValue * (this->upper - this->lower) / 100.0);  //输入的百分比，确认正确范围后，根据DAC位数决定输出
}

void AnalogeIOPort:: setResolutionBit(int res){//设置DAC的分辨率
    this->resolutionBit = res;
    setOutputRange();
}


int AnalogeIOPort:: getOutputMax() {
    return outputMax;
}

void AnalogeIOPort:: setOutputRange() {
    //直接用resolutionBit来调用，不开放对外直接设置接口，避免两者不统一
    this->outputMax = (2 << (this->resolutionBit-1)) -1;
    this->upper = this->outputMax;
    this->lower = 0;//默认的输出最小值为0
}

double AnalogeIOPort:: limitRange(double value,double lower,double upper) { //范围限制
    if (value >= lower && value <= upper)
        return value;
    else if (value > upper)
        return upper;
    else
        return lower;
}

double AnalogeIOPort:: readAnalog(){//读取模拟输出至百分比
    return (analogRead(portNo)-this->lower) / (this->upper-this->lower);
}

void AnalogeIOPort::outputDirectAnalog(double outputValue){//直接按值，并且由输出最大值限定
    analogWrite(portNo, limitRange(outputValue, this->lower, this->upper));
}

double AnalogeIOPort:: readDirectAnalog(){//读取模拟输出至百分比
    return analogRead(portNo);
}

//设置实际的输入输出限制
double AnalogeIOPort:: getUpper(){
    return this->upper;
}
void AnalogeIOPort:: setUpper(double up){
    this->upper = up > outputMax ? outputMax: up;
}
double AnalogeIOPort:: getLower(){
    return this->lower;
}
void AnalogeIOPort:: setLower(double low){
    this->lower = low > 0 ? low : 0;
}