#pragma once
#include "CtrlAccessory.h"


class VirtualAnalogReader : public AnalogReader , public CtrlAccessory{
    //通过virtualAnalogRead来设定输出值
   private:
    double virtualAnalogRead;

   protected:
    double readAnalogTool() {
        this->movCacu.append((double)virtualAnalogRead);
        return virtualAnalogRead;
    }

   public:
    VirtualAnalogReader(int port, int res, int smoothSize) : AnalogReader(port, res, smoothSize) {}
    
    void setVirtualAnalog(double input) { this->virtualAnalogRead = input; }

    void showParameters(){
        this->debugPrint(this->acId + ": "+String(virtualAnalogRead, 3));
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