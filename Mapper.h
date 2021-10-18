class Mapper {
    //多项式类，可设置自动更新
   private:
    double* parameter;
    int order;

   public:
    Mapper(int o) {
        if (o < 1)
            o = 1;
        this->order = o;
        parameter = new double[o + 1];
    }

    //设为虚函数，可被重载为其他映射关系
    virtual double mapping(double input) {
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

    void showParameter() {
        for (int i = 0; i < (order + 1); i++) {
            Serial.println(parameter[i]);
        }
    }
};