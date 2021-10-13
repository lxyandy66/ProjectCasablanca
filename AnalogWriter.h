#pragma once
#include "AnalogIOPort.h"

class AnalogWriter : public AnalogIOPort {
    private:
    void outputAnalogTool(double limitedValue);//根据DAC和上下限设置提供正确的实际输出值
    
    public:
     AnalogWriter(int port, int res);
     void outputAnalogByMapping(double outputValue);  //直接按照百分比输出，具体输出值由内部函数确定
     void outputAnalogDirectly(double outputValue,boolean withLimit);  //直接按值，并且由输出最大值限定
};