#pragma once
#include "CtrlAccessory.h"


class VirtualAnalogReader : public AnalogReader , public CtrlAccessory{
    //通过virtualAnalogRead来设定输出值
   private:
    double virtualAnalogRead;
    double initalValue = 0;//避免在初始过程中队列为空导致异常

   protected:
    double readAnalogTool() {
        this->movCacu.append((double)virtualAnalogRead);
        Serial.println("Virtual Read:"+String(virtualAnalogRead,3));
        return virtualAnalogRead;
    }

   public:
    VirtualAnalogReader(int port, int res, int smoothSize) : AnalogReader(port, res, smoothSize) {
        this->movCacu.append(initalValue);//避免在初始过程中队列为空导致异常
    }

    double readAnalogSmoothly(boolean needUpdated,boolean needMapping,boolean needSmooth){
    //作为总入口好了
        Serial.println("in Virtual AnalogRead readAnalogSmoothly");
        return virtualAnalogRead;//*需要进一步优化一下*/
    }

    double getNewestValue() { return this->movCacu.getNewestElement(); }

    void setVirtualAnalog(double input) { this->virtualAnalogRead = input; }

    void showParameters(){
        this->debugPrint(this->acId + ": "+String(virtualAnalogRead, 3));
        this->debugPrint(this->acId + " Queue size: "+this->movCacu.getWindowSize());
    }

    void outputStatus(JsonDocument* jsonDoc) {
        (*jsonDoc)[AgentProtocol::DEV_ID_FROM_JSON] = this->acId;
        (*jsonDoc)[this->valueName] = CtrlAccessory::roundDecimal(this->virtualAnalogRead,3) ;
    }
    
};

class VirtualAnalogWriter : public AnalogWriter , public CtrlAccessory{
   private:
    double virtualAnalogOut;

   public:
    VirtualAnalogWriter(int port, int res) : AnalogWriter(port, res) {}
    void setVirtualAnalog(double input) { this->virtualAnalogOut = input; }

    void showParameters(){
        this->debugPrint(this->acId + ": "+String(virtualAnalogOut, 3));
    }

    void outputStatus(JsonDocument* jsonDoc) {
        (*jsonDoc)[AgentProtocol::DEV_ID_FROM_JSON] = this->acId;
        (*jsonDoc)[this->valueName] = CtrlAccessory::roundDecimal(this->virtualAnalogOut,3) ;
    }
};