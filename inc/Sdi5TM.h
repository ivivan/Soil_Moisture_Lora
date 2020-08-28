#ifndef SDI5TMH_H
#define SDI5TMH_H

#include "mDot.h"
#include <stdio.h>
#include "ChannelPlan.h"
#include "Lora.h"
#include "ChannelPlan_AU915.h"

#include "tools.h"
#include "data.h"

#include <iostream>

/* waiting may not be the best way to deal with it, maybe a flag? should be able 
wait for less than one second */
#define MESSAGE_WAIT_TIME 1
#define SDI_DATA_LENGTH 36

/**
 * required at the start of the program
 * powers and sets up sensor
 */
void init_5tm(void);

/**
 * interupt callback for input from the sensor.
 * excludes all non alphaneumeric inputs and stores the results in inputData 
 * with the length in dataLength.
 */
void read_routine(void);

/**
 * Conduct entire request measurement and request data cycle for sensor at 
 * address addr, store the results in takeData.
 */
void get_5tm_data(struct sensorData* takeData, int addr);

/** 
 * Convert the data reading from the 5TM sensor to two floats temperature and
 * vwc stored in the sensorOutput struct.
 */
void convert_5tm_sensor(char* data, struct sensorData* sensorOutput);

/* struct to store the length and data from any communication with the sensor */
struct com {
    char data[SDI_DATA_LENGTH];
    int length;
};

/** scan all possible addresses and return a vector containing addresses that 
 * gave a response
 */
std::vector<int> scan(void);


#endif

/**
 * EXAMPLE:
 * int main(void) {
 *      init_5tm();
 *      int sensorId = '1';
 *      struct sensorData reading;
 *      get_5tm_data(&reading, sensorId);
 *      printf("%d %f %.1f ", sensorId, reading.vwc, reading.temperature);
 */
