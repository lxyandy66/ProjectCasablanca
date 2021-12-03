#pragma once

class CtrlAccessory {
    //作为所有控制辅助模块的基类

    protected:
     String acId;

     public:
      CtrlAccessory() { this->acId = "X_DEF"; }
      CtrlAccessory(String id) { this->acId = id; }
      String getAcId() { return this->acId; }
      void setAcId(String str) { this->acId = str; }

      virtual void showParameters() = 0;

      virtual void debugPrint(String str) { Serial.println(str); }
};