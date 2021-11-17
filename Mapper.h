#pragma once
#include <ArduinoJson.h>
#include "AgentProtocol.h"

class Mapper {
    //多项式类，可设置自动更新
   private:
    String mapperName;
    double* parameter;
    int order;

   public:
    Mapper(int o,String n) {
        this->mapperName = n;
        if (o < 1)
            o = 1;
        this->order = o;
        parameter = new double[o + 1];
    }

    // 直接对多项式参数进行初始化的方法，但不对传入参数合法性进行检查
    Mapper(int o,double* p,String n) {
        this->mapperName = n;
        if (o < 1)
            o = 1;
        this->order = o;
        parameter = new double[o + 1];
        //不对传入的p进行检查
        for (int i = 0; i < o + 1;i++){
            parameter[i] = p[i];
        }
    }

    void setMapperName(String n) { this->mapperName = n; }
    String getMapperName() { return this->mapperName; }

    //设为虚函数，可被重载为其他映射关系
    double mapping(double input) {
        double result = 0;
        double multiple = 1;
        for (int i = 0; i < (order + 1); i++) {
            for (int j = (order - i); j > 0; j--) 
                multiple *= input;
            result += parameter[i] * multiple;
            multiple = 1;
        }
        return result;
    }

    //按实际的次数更新参数，例如一阶则pos=1，更改二阶方程中二阶参数为99则为2,99
    boolean updateParameter(int orderChange, double value) {
        if (orderChange < 0 || orderChange > this->order)
            return false;  //不小于0且不大于阶数
        parameter[orderChange - order] = value;
        return true;
    }

    //用于批量设置多项式的参数
    void setParameter(double* para) {
        for (int i = 0; i < (order + 1); i++) {
            parameter[i] = para[i];
        }
    }

    //用于通过通讯的json字符串设置参数
    boolean setParameter(String jsonStr){
        //JSON解析
        Serial.println(jsonStr);
        DynamicJsonDocument jsonBuffer(AgentProtocol::MSG_SIZE);
        DeserializationError t = deserializeJson(jsonBuffer, jsonStr);
        if (t) {
            return false;
        }
        //检测指令类型
        //默认为二次函数，k，b
        parameter[0]=jsonBuffer["k"].as<double>();
        parameter[1]=jsonBuffer["b"].as<double>();
        return true;
    }

    void showParameter() {
        for (int i = 0; i < (order + 1); i++) {
            Serial.println(parameter[i]);
        }
    }
};