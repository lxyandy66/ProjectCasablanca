#pragma once
#include "AnalogIOPort.h"

class AnalogReader : public AnalogIOPort {
   private:
    
    double readAnalogTool();            //实际读取模拟量的函数

   public:
   //模拟量输出与读取 
    double readAnalog();//直接按照百分比读取
    double readDirectAnalog();//直接按值读取
};