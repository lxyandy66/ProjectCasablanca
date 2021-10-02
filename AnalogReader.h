#pragma once
#include "AnalogIOPort.h"
#include "MovingAverageCaculator.h"

class AnalogReader : public AnalogIOPort {
   private:
    NumericMovingAverageCaculator movCacu;   //选7步移动平均
    double readAnalogTool();  //实际读取模拟量的函数

   public:
    AnalogReader(int port,int res);

    //模拟量输出与读取
    double readAnalogByMapping(boolean needUpdated);//按照映射关系读取
    double readAnalogDirectly(boolean needUpdated);//直接按值读取
    double readAnalogSmoothly(boolean needUpdated, boolean needMapping);

    void setSmoothWindowSize(int size);
};