#pragma once
//本程序包含虚拟物理系统、虚拟模拟IO借口，用于控制相关的测试

class VirtualAnalogReader : public AnalogReader {
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
};

class VirtualAnalogWriter : public AnalogWriter {
   private:
    double virtualAnalogOut;

   public:
    VirtualAnalogWriter(int port, int res) : AnalogWriter(port, res) {}
    void setVirtualAnalog(double input) { this->virtualAnalogOut = input; }
};

class TestSystem {
   public:
    NumericMovingAverageFilter filter;
    double sysOutput;  //输出，被控对象状态

    TestSystem(double initOutput) { sysOutput = initOutput; }

    double updateState(double controllerOutput) {
        filter.append(sysOutput + 0.031 * (controllerOutput) -random(1, 3));
        sysOutput = filter.getAverage();  //(1 / (10 * s + 1))*exp((-2)*s);-random(0,2)
        return sysOutput;
    }
};