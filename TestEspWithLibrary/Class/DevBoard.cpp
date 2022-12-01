#include "DevBoard.h"

DevBoard::DevBoard() {
}

void DevBoard::setThSensor(DHT sensor) {
    this->thSensor = sensor;
}
void DevBoard::setThSensor(int dhtData, String dhtType = "DHT22") {
    this->thSensor = dht22(dhtData, DHT_TYPE);
}
