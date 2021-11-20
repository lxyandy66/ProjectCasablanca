#pragma once

class CtrlAccessory {
    //作为所有控制辅助模块的基类

    protected:
     String id;

     public:
      CtrlAccessory() { this->id = "X_DEF"; }
      String getId() { return this->id; }
      void setId(String str) { this->id = str; }

      virtual void showParameters() = 0;

      virtual void debugPrint(String str) { Serial.println(str); }
};