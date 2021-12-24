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

   void defaultCommandDistributor(DynamicJsonDocument jsonBuffer,String cmdType){
       if(cmdType=="TO"){
        //    {cmd:"TO"}
           isTakeOver = !isTakeOver;
            Serial.println(isTakeOver ? "Ture" : "False");
            digitalWrite(takeOverTrigger, !isTakeOver);
            return;
       }else if(cmdType==CtrlBoardManager::CTRL_ON){
        // 例如{cmd:"CT_ON",id:"C_FR",on:true}
        PackedPID* changedController=findControllerById(jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
        if(changedController == NULL || changedController == nullptr){
            debugPrint("Controller not found according to: " + jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
            return ;
        }
        isTakeOver = jsonBuffer[CtrlBoardManager::CTRL_DATA_ON].as<bool>();
        changedController->pidController.SetMode(isTakeOver);
        Serial.println(isTakeOver ? "Ture" : "False");
        digitalWrite(takeOverTrigger, !isTakeOver);
        return;
    }
   }


   boolean checkReqOrder(long reqId){
       //设为0避免接管等无时序命令被阻碍
       return reqId==0?true:this->localReqId <= reqId;
   }

};
