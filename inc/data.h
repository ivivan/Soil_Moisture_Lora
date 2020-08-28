#ifndef DATA_H
#define DATA_H

#define I2C_MSG_SIZE 5 //temperature sensor transmits 5 bits at a time
#define SERIAL_DATA_LENGTH 10
#define SDI_DATA_LENGTH 36


struct sensorData {
    time_t sampleTime; //time at which reading was taken
    int sensorID;
    float simpleMoisture; // soil moisure form the dedicated sensor
    float simpleTemp; // temperature from the dedicated sensor
    // data from the Decagon sensor
    float vwc;
    float temperature;
};

/**
 * Takes the readings from the temperature sensor and extracts the temperature 
 * returns the temperature reading
 */
// float convert_i2c_sensor(char* data);

/** 
 * create a packet for transmission over Lora 
 * NOTE project name must start with tdf if tdf packets are to be used
 */
std::vector<uint8_t> makePacket(struct sensorData takeData);

/**
 * populate a sensor data struct with 0s for error handling
 */
void sensorData_init(struct sensorData* emptyData);

/**
 * make an array of sensorNumber sensorData structs and store them in data
 */
struct sensorData * make_data_array(struct sensorData* data, int sensorNumber); 

#endif