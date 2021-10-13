#pragma once
#include "AnalogWriter.h"

AnalogWriter::AnalogWriter(int port, int res):AnalogIOPort(port,res){}

void AnalogWriter:: outputAnalogTool(double limitedValue) {
    // 最终执行模拟量输出的函数，确保传入的值已经是合法值
    analogWrite(this->portNo, limitedValue);
}

void AnalogWriter::outputAnalogByMapping(double outputValue) {//直接按照映射关系输出，具体输出值由内部函数确定
    double actualOut = mappingValue(outputValue);
    // Serial.println("Actual outSet: "+String(actualOut));
    outputAnalogTool(actualOut);
    // outputAnalogTool(mappingValue(outputValue));
}



void AnalogWriter::outputAnalogDirectly(double outputValue,boolean withLimit=true){//直接按值，并且由输出最大值限定
    outputAnalogTool(withLimit ? limitRange(outputValue, this->getLower(),this->getUpper()): outputValue);
}
