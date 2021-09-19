#pragma once
#include <Arduino.h>

class AnalogeIOPort {
   private:
    
    int resolutionBit;
    int outputMax;  //根据DAC的位数决定输出的最大值
    int portNo;
    double lower;
    double upper;   //实际使用的最大值
    
    double limitRange(double value,double lower,double upper);
    void setOutputRange();
    int outputAnalogTool(double limitedValue);

   public:
    AnalogeIOPort(int port);
    AnalogeIOPort(int port, int res);

    //设置输出的上下限，默认与DAC的分辨率相同，即0-DAC分辨率
    //防止部分情况下读取的模拟量无法达到理想情况
    double getUpper();
    void setUpper(double up);
    double getLower();
    void setLower(double low);
    
    ///DAC设定，仅支持设定DAC位数，自动决定输出最大值
    int getOutputMax();
    void setResolutionBit(int res);

    //模拟量输出与读取
    void outputAnalog(double outputValue); //直接按照百分比输出，具体输出值由内部函数确定
    void outputDirectAnalog(double outputValue); //直接按值，并且由输出最大值限定
    double readAnalog();//直接按照百分比读取
    double readDirectAnalog();//直接按值读取
};
