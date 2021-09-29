#pragma once
#include "AnalogWriter.h"

int AnalogWriter:: outputAnalogTool(double limitedValue) {
    return lower + round(limitedValue * (this->getUpper() - this->getLower()) / 100.0);  //输入的百分比，确认正确范围后，根据DAC位数决定输出
}

void AnalogWriter::outputAnalogByPercentage(double outputValue) {//直接按照百分比输出，具体输出值由内部函数确定
    int actualOut = outputAnalogTool(limitRange(outputValue, 0, 100));
    Serial.println("Actual outSet: "+String(actualOut));
    analogWrite(portNo, actualOut);
}



void AnalogWriter::outputAnalogDirectly(double outputValue,boolean withLimited){//直接按值，并且由输出最大值限定
    analogWrite(portNo, withLimited?limitRange(outputValue, this->getLower(), this->getUpper()):outputValue);
}
