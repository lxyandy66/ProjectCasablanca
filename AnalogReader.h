#pragma once
#include "AnalogIOPort.h"
#include "MovingAverageFilter.h"

class AnalogReader : public AnalogIOPort {
   protected:
    NumericMovingAverageFilter movCacu;   //选7步移动平均
    virtual double readAnalogTool();  //实际读取模拟量的函数


   public:
    AnalogReader(int port,int res,int smoothSize=7);

    //模拟量输出与读取
    void updatedReadAnalog();  //只读取，不输出
    double readAnalogByMapping(boolean needUpdated=false);  //按照映射关系读取
    double readAnalogDirectly(boolean needUpdated=false);//直接按值读取
    double readAnalogSmoothly(boolean needUpdated=false, boolean needMapping=false);

    void setSmoothWindowSize(int size);

    double mappingValue(double originalValue);
};