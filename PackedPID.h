#pragma once
#include <PID_v1.h>
#include "AnalogReader.h"
#include "AnalogWriter.h"
#include "CtrlAccessory.h"

class PackedPID: public CtrlAccessory {
    // PID控制器的包装类，包装控制器的输入输出及设定点,直接与IO口进行交互
    // 实际上我就是不想让原来的PID的IO直接露出来
   
   private:
    double setPoint, ctrlInput, ctrlOutput;  //作为直接与PID控制器交互的变量
    AnalogReader* inputPort;
    AnalogWriter* outputPort;
    boolean isCtrlByMapping;//实际上对于这个类而言不知道自己的输入输出是模拟信号还是真实的物理量

    public:
    PID pidController; //公有好了，外部方便更新

    //默认不调参则PID为1,0,0
    PackedPID(AnalogReader* inPort, AnalogWriter* outPort, double initSetpoint,int ControllerDirection)
        : inputPort(inPort), outputPort(outPort), setPoint(initSetpoint),
        pidController(&(this->ctrlInput), &(this->ctrlOutput), &(this->setPoint), 1, 0, 0,ControllerDirection) {
        id="C_DEF";
        isCtrlByMapping = true;
    }

    PackedPID(AnalogReader* inPort, AnalogWriter* outPort, double initSetpoint,double kp, double ki, double kd,int ControllerDirection)
        : inputPort(inPort), outputPort(outPort), setPoint(initSetpoint), 
        pidController(&(this->ctrlInput), &(this->ctrlOutput), &(this->setPoint), kp, ki, kd,ControllerDirection){
        id="C_DEF";
        isCtrlByMapping = true;
    }

    void enablePidController(boolean enable) { pidController.SetMode((enable?1:0)); }
    
    void needCtrlByMapping(boolean needMapping){
        this->isCtrlByMapping = needMapping;
    }

    double updatedControl(){
        //每次计算时通过AnalogReader的平滑输出来更新PID控制器交互的变量
        this->ctrlInput = inputPort->readAnalogSmoothly(false, isCtrlByMapping);
        pidController.Compute();
        if (isCtrlByMapping) {
            this->outputPort->outputAnalogByMapping(this->ctrlOutput);
        } else {
            this->outputPort->outputAnalogDirectly(this->ctrlOutput);
        }
        return this->ctrlOutput;
        
    }

    void setSetpoint(double sp) { this->setPoint = sp; }
    double getSetpoint() { return this->setPoint; }

    void showParameters(){
        debugPrint("input: "+String(ctrlInput,2)+", output: "+String(ctrlOutput,2)+", setpoint: "+String(setPoint,2));
        debugPrint("kp: "+String(pidController.GetKp(),2)+", ti: "+String(pidController.GetKi(),2)+", td: "+String(pidController.GetKd(),2));
    }

    void tuningParameter(double k,double i,double d){
        pidController.SetTunings(k, i, d);
    }
};

