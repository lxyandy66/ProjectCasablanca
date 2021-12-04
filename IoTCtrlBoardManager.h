#pragma once
#include "CtrlBoardManager.h"

class IoTCtrlBoardManager : public CtrlBoardManager {
    //CtrlBoardManager 的子类，实际使用于该项目中IoT控制板
   private:
    int takeOverTrigger;//用于接管原控制的数字输出针脚
    boolean isTakeOver = false;

   public:
   IoTCtrlBoardManager():CtrlBoardManager(){}
   IoTCtrlBoardManager(int takeOverTriggerPin):CtrlBoardManager(),takeOverTrigger(takeOverTriggerPin){}

   void setTakeOverTriggerPin(int pinNo) { this->takeOverTrigger = pinNo; }

   void defaultCommandDistributor(DynamicJsonDocument str,String cmdType){
       if(cmdType=="TO"){
        //    {cmd:"TO"}
           isTakeOver = !isTakeOver;
            Serial.println(isTakeOver ? "Ture" : "False");
            digitalWrite(takeOverTrigger, isTakeOver);
            return;
       }
   }
};
