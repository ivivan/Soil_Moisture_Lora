/** 
 * had linker error with incompatible libraries
 * now using:
 *  mbed-os: https://github.com/ARMmbed/mbed-os/#16bac101a6b7b4724023dcf86ece1548e3a23cbf
 *  libmDot: https://os.mbed.com/teams/MultiTech/code/libmDot-mbed5/#64982192a2af
 */

#include "mDot.h"
#include <stdio.h>
#include "ChannelPlan.h"
#include "Lora.h"
#include "ChannelPlan_AU915.h"

#include "Sdi5TM.h"
#include "tools.h"
#include "data.h"

#include <iostream>
#include <signal.h>

#define SLEEP_TIME 500 // seconds between samples
#define WAIT_TIME 10 // seconds between attemmpts
#define NUMBER_OF_SENSORS 2

void get_data(struct sensorData* takeDatas, int* sensors);
void send_data(struct sensorData* takeData);

/* use pin mapping on pg 27 of mDot manual*/
DigitalOut statusLed(PA_11, 1);

int main(void) {
    // set_time(); // resets on power off
    time_t initialTime = time(NULL); 
    //voltage();
    init_5tm();
    int sensorIds[] = {49, 50};
    // alternatively to get the sensor ids by scanning the line use this code:
    // std::vector<int> ids = scan();
    // int* sensorIds = ids.data();
    //.


    lora::ChannelPlan* plan = new lora::ChannelPlan_AU915();
    mDot* mdot = mDot::getInstance(plan);
    assert(mdot);

    mdot->resetConfig();
    mdot->resetNetworkSession();
    /* add configs here */
    mdot->setLogLevel(mts::MTSLog::WARNING_LEVEL);
    /* end configs */
#ifdef CONFIG
    /* excluded when testing until configeration is worked out  */
    int joinStatus = mdot->joinNetwork();
    while (joinStatus != mDot::MDOT_OK) {
        error("network join error");
        wait(WAIT_TIME);
        joinStatus = mdot->joinNetwork();
    }
#endif
    printf("%s\n", ctime(&initialTime));

    printf("[sample# time id vwc temp]\n");
    int sampleNo = 0;
    while (1) {
        time_t sampleTime = time(NULL);
        char sampleTimeString[256];
        strftime(sampleTimeString, 256, "%d/%m/%Y %T", localtime(&sampleTime));

        struct sensorData outputArray[NUMBER_OF_SENSORS];
        struct sensorData* sensorOutputArray = make_data_array(outputArray, 
                                                    NUMBER_OF_SENSORS);
        get_data(sensorOutputArray, sensorIds);

        // send to pc with specified format as LoRa is not set up
        printf("\n sample %d ", sampleNo);
        printf("%s ", sampleTimeString);
        // 5 seconds for 2 sensors
        for (int i = 0; i < NUMBER_OF_SENSORS; i++){
            sensorOutputArray[i].sampleTime = sampleTime;
            sensorOutputArray[i].sensorID = sensorIds[i];
            printf(" %d %f %.1f ", sensorIds[i], sensorOutputArray[i].vwc,
                    sensorOutputArray[i].temperature);
        } 
        printf("\n");

        send_data(sensorOutputArray);
        sampleNo ++;

        mdot->sleep(SLEEP_TIME, mDot::RTC_ALARM, 0); 
        // logDebug("Woken up");
        // wait(SLEEP_TIME);
    }
    return 0;
}

/**
 * Format sensor data and send as a packet
 */
void send_data(struct sensorData* sensorOutputArray) {
    /* how this is handled depends on how the data is being recieved, the 
    packet may changed based on this as well. Tdf looks to have a flag for
    sending each sensor's data alone or for sending multiple at the same time. 
    Also not very robust */

    for (int i = 0; i <NUMBER_OF_SENSORS; i++){
        std::vector<uint8_t> data = makePacket(sensorOutputArray[0]);

#ifdef CONFIG
        /* excluded when testing until configeration is worked out  */
        printf("data loaded for sending: %u \n", data.front());

//could add loop to send multiple times in case something is lost
        int returnValue = mdot->send(data);
 
        if (returnValue != mDot::MDOT_OK) {
            error("an Error occured in sending");
        }
#endif
    }
}


/**
 * collect data from each sensor
 * sensorOutputArray: collected data is stored in sensorOutputArray 
 * sensorIds: array holding id of every sensor being used
 */
void get_data(struct sensorData* sensorOutputArray, int* sensorIds){
    blink(statusLed, 1);

    // I think makes mores sense to have each be it's own sensor so 
    //  just keep that in mind
    for(int i = 0; i< NUMBER_OF_SENSORS; i++){
        get_5tm_data(&sensorOutputArray[i], sensorIds[i]);
    } 
    return;
}
