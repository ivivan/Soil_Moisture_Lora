// #include "Serial5TM.h"


// Serial input(D1, D0);
// DigitalOut power(D12, 0);

// volatile int dataLength;// = 0;
// char inputData[SERIAL_DATA_LENGTH];


// void read_routine(void){
//     __disable_irq();
//     int point = input.getc();
//     // if (point == -1 || point == NULL) {
//     //     printf("gathering\n\n");

//     // }
//     if ((point > 47 && point < 58) ||  point == 32) {
//             inputData[dataLength] = char(point);
//             dataLength++;
//     }
//     __enable_irq();
// }

// void init_5tm(void) {
//     input.baud(1200);
//     input.attach(&read_routine);
//     input.printf("?!\n");
// }
// void get_5tm_data(struct sensorData* takeData) {
//     // input.baud(1200);
//     // input.attach(&read_routine);
//     // input.printf("?!\n");
//     dataLength = 0;

//     power = 1;
//     wait(SAMPLE_WAIT_TIME);

//     __disable_irq();
//     int restartCount = 0;
//     while (!*inputData) {
//         restartCount += 1 ;
//         error("Data collection error\n\n");
//         power = 0;
//         wait(SAMPLE_WAIT_TIME);
//         power = 1;
//         wait(SAMPLE_WAIT_TIME);
//         if (restartCount > 3 ) {
//             //NVIC_SystemReset();
//             strcpy(inputData, "-1");
//             break; 
//         }
//     }
//     inputData[dataLength] = '\0';
//      __enable_irq();

//     power = 0;
//     convert_5tm_sensor(inputData, takeData);

// }

// float convert_vwc(string ecString) {
//     int eRaw = atoi(ecString.c_str());
//     if (eRaw <= 0  || eRaw == 4095){
//         error("error with electric permiativity reading\n");
//         return -1;
//     }
//     float ec = eRaw / 50.0;
//     return ((4.3*pow(10,-6)*pow(ec,3)) - (5.5 * pow(10, -4)* pow(ec,2)) + 
//             (2.92* pow(10, -2)*ec) - (5.3 * pow(10,-2)));

// }

// float convert_temp(string tempString) {
//     int tRaw = atoi(tempString.c_str());;
//     if (tRaw <= 0 || tRaw > 1022) {
//         error("temperature reading error\n");
//         return -1;
//     }
//     int t2;
//     if (tRaw <= 900 ) {
//         t2 = tRaw;
//     } else {
//         t2 = 900 + 5* (tRaw - 900);
//     }
//     return (t2 - 400)/10.0;

// }
// /**
//  * Parse 5tm ascii stream
//  * format: "[ep] 0 [simpleTemp]"
//  * stores information in sensorData struct provided
//  */  
// void convert_5tm_sensor(char* data, struct sensorData* sensorOutput){
//     // will need to do some error handling since null data causes a problem
//     //printf("Converting...\n");
  
//     /* split by spaces to get the valles */
//     // seems like this should be simpler
//     string dataStr(data);
//     string token = " ";
//     size_t total = dataStr.length();
    
//     size_t i = 0;
//     size_t previousI = 0;
//     std::vector<string> words;
//     while(i < total && i!=string::npos) {
        
//         i = dataStr.find(token, previousI);
//         string word = dataStr.substr(previousI, i);
//         previousI = i+1;

//         words.push_back(word);
//     }

//     sensorOutput->vwc = convert_vwc(words.at(0));
//     sensorOutput->temperature = convert_temp(words.at(2));

// }
