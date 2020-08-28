#include "mDot.h"
#include "data.h"
#include "MTSLog.h"
#include <math.h>
#include <iostream>
using namespace std;

void sensorData_init(struct sensorData* emptyData) {
    emptyData->sampleTime =  0; 
    emptyData->sensorID = 0;
    emptyData->simpleMoisture = 0; 
    emptyData->simpleTemp = 0; 
    emptyData->vwc = 0;
    emptyData->temperature = 0;
}

std::vector<uint8_t> makePacket(struct sensorData takeData) {
    //TODO create a proper packet
    uint8_t sensorID = -1;
    std::vector<uint8_t> data;
    // these need to be modified to fit the correct lengths
    data.clear();
    data.push_back(0b1100);
    data.push_back(sensorID); //12 bits
    data.push_back(takeData.sampleTime); // 6 bits
    data.push_back(takeData.temperature*10); // must make entry for these
    data.push_back(takeData.vwc*100);
    return data;
}

struct sensorData * make_data_array(struct sensorData* data, int sensorNumber) {
    for (int i = 0; i< sensorNumber; i++) {
        struct sensorData takeData;
        sensorData_init(&takeData);
        data[i] = takeData;
    }
    return data;
}

