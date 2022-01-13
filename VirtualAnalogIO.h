#pragma once

class VirtualAnalogReader : public AnalogReader {
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
};

class VirtualAnalogWriter : public AnalogWriter {
   private:
    double virtualAnalogOut;

   public:
    VirtualAnalogWriter(int port, int res) : AnalogWriter(port, res) {}
    void setVirtualAnalog(double input) { this->virtualAnalogOut = input; }
};