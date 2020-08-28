/** contains very little validation of whether the input is correct */
#include "Sdi5TM.h"

Serial input(D1, D0);

volatile int dataLength = 0;
char inputData[SDI_DATA_LENGTH];
DigitalOut statLed(PA_8, 1);

void read_routine(void){
    statLed = 0;
    __disable_irq();
    int point = input.getc();
    if ((point > 47 && point < 58) || (point > 64 && point < 91) || 
            (point > 96 && point < 123) || point == 32) {
        inputData[dataLength] = char(point);
        dataLength++;
    }
    __enable_irq();
}

void init_5tm(void) {
    input.baud(9600);
    input.attach(&read_routine);
    input.printf("?!\n");
    // if all sensors on the bus are addressed there will be no output on start
    // however wait in case just one sensor with 0 as address
    // alternatively could disable interrupts until they are necessary
    wait(1);
}




struct com send_command(char* command, int length) {
    struct com com;
    for (int i = 0; i < length; i++) {
        input.putc(command[i]);
    }
    wait(MESSAGE_WAIT_TIME);
    inputData[dataLength] = '\0';
    com.length = dataLength;
    string astring(inputData);
    strcpy(com.data, astring.c_str());
    return com;
}

struct com send_info(int addr) {
    struct com info;
    dataLength = 0;
    // input.putc(addr);
    // input.putc('I');
    // input.putc('!');
    // input.putc('\r');
    // input.putc('\n');
    char send[] = {(char)addr, 'I', '!', '\r', '\n'};
    info = send_command(send, 5);
    // wait(MESSAGE_WAIT_TIME);
    // // maybe disable here
    // inputData[dataLength] = '\0';
    // info.length = dataLength;
    // strcpy(info.data, inputData);
    return info;
}

struct com send_measure(int addr) {
    struct com measure;
    dataLength = 0;
    char send[] = {(char)addr, 'M', '!', '\r', '\n'};
    measure = send_command(send, 5);
    return measure;
}

/**
 * Use only when disconnected form a bus to see which sensor currently 
 * communicating with, if more than one sensor is connected get a corrupted line
 */
struct com send_query(void) {
    struct com response;
    dataLength = 0;
    char send[] = {'?', '!', '\r', '\n'};
    response = send_command(send, 4);
    return response;
}

/** for changin the address of a sensor from the oldAddr to the newAddr
 */
struct com send_addr(int oldAddr, int newAddr) {
    struct com response;
    dataLength = 0;
    char send[] = {(char)oldAddr, 'A', (char)newAddr, '!', '\r', '\n'};
    response = send_command(send, 6);
    return response;
}


struct com send_data(int addr) {
    struct com info;
    dataLength = 0;
    char send[] = {(char)addr, 'D', '0', '!', '\r', '\n'};
    info = send_command(send, 6);
    return info;
}

/**
 * returns 1 if response is invalid
 * 0 otherwise
 */
int noResponse(char* message) {
    if(!*message) {
        return 1;
    } else if(strcmp(message, "No Response") == 0){
        return 1;
    }
    return 0;
}

void get_5tm_data(struct sensorData* sensorOutput, int addr) {
    dataLength = 0;

    struct com info;
    struct com mes;
    struct com data;

    // unsused and unnecesary, used for testing
    info = send_info(addr);
    logTrace("Sensor adr: %c num: %d valu: %s", addr, info.length, info.data);
    if(info.data[0] != addr) {
        logWarning("Wrong sensor response, addressed to %d, response from %d %s", 
                addr, info.data[0], info.data);
    }
    
    // request measurement
    mes = send_measure(addr);
    logTrace("Sensor adr: %c num: %d valu: %s", addr, mes.length, mes.data);
    // wait amount of time for measurment to be made
    char waitTime[4];
    for (int i = 1; i < 4; i++){
        waitTime[i-1] = mes.data[1];
    }
    waitTime[3] = '\0';
    logDebug("Waiting %s seconds for data collection", waitTime);
    wait(atoi(waitTime));

    // request data
    data = send_data(addr);
    logTrace("Sensor adr: %c num: %d valu: %s\n", addr, data.length, data.data);

    // deal with no response, restarts 3 times
    // sensors do not respond unless addressed
    // __disable_irq();
    // int restartCount = 0;
    // while (!*inputData) {
        
    //     restartCount += 1 ;
    //     error("Data collection error\n%s%d\n", inputData, dataLength);
    //     // power = 0;
    //     // wait(SAMPLE_WAIT_TIME);
    //     // power = 1;
    //     // wait(SAMPLE_WAIT_TIME);
    //     // if (restartCount > 3 ) {
    //     //     //NVIC_SystemReset();
    //     //     strcpy(inputData, "-1");
    //     //     break; 
    //     // }
    // }
    //  __enable_irq();

    if(noResponse(data.data)) {
        logError("No response from sensor. ID: %d", addr);
        logError("Data collection error %s %d\n", data.data, data.length);
        sensorOutput->vwc = -1;
        sensorOutput->temperature = -1;
        return;
    }
    convert_5tm_sensor(data.data, sensorOutput);
    return;
}


float convert_vwc(char* ecString) {
    float ec = atoi(ecString)/100.0;
    if (ec == 0) {
        return -1;
    }
    return ((4.3*pow(10,-6)*pow(ec,3)) - (5.5 * pow(10, -4)* pow(ec,2)) + 
            (2.92* pow(10, -2)*ec) - (5.3 * pow(10,-2)));
}

float convert_temp(char*  tempString) {
    float t  = atoi(tempString)/10.0;
    if (t == 0) {
        return -1;
    }
    return t;
}

void convert_5tm_sensor(char* data, struct sensorData* sensorOutput){
    // will need to do some error handling since null data causes a problem
    // as per response format from 5tm
    char ec[4];
    for(int i = 1; i < 4; i++) {
        ec[i-1] = data[i];
    }
    ec[3] = '\0';

    char temp[4];
    for(int i =4; i < 7; i++) {
        temp[i-4] = data[i];
    }
    temp[3] = '\0';

    sensorOutput->vwc = convert_vwc(ec);
    sensorOutput->temperature = convert_temp(temp);
}


std::vector<int> scan(void) {
    printf("SCANNING...");
    std::vector<int> ids;
    for (int i = 48; i < 58; i++) {
        dataLength = 0;
        wait(0.5);
        input.putc(i);
        input.putc('I');
        input.putc('!');
        input.putc(' ');
        input.putc('\r');
        input.putc('\n');
        wait(0.5);
        
        inputData[dataLength] = '\0';
        if(noResponse(inputData) == 0) {
            ids.push_back(i);
        }
    }
   
    for (int i = 65; i < 91; i++) {
        dataLength = 0;
        wait(0.5);
        input.putc(i);
        input.putc('I');
        input.putc('!');
        input.putc(' ');
        input.putc('\r');
        input.putc('\n');
        wait(0.5);
        inputData[dataLength] = '\0';
        if(noResponse(inputData) == 0) {
            ids.push_back(i);
        }
    }
    for (int i = 97; i < 123; i++) {
        dataLength = 0;
        wait(0.5);
        input.putc(i);
        input.putc('I');
        input.putc('!');
        input.putc(' ');
        input.putc('\r');
        input.putc('\n');
        wait(0.5);
        inputData[dataLength] = '\0';
        if(noResponse(inputData) == 0) {
            ids.push_back(i);
        }
       
    }
    for (std::vector<int>::const_iterator i = ids.begin(); i != ids.end(); ++i){
        printf("%c, ", *i);
    }
    return ids;
}