
#ifndef AVERAGE_H
#define AVERAGE_H

#include "NRF24_config.h"

class AverageClass {
public:
    // Calculate new cumulative average CA_{n+1}
    // Formula: CA_{n+1} = CA_{n} + ((x_{n+1} - CA_{n}) / (n + 1))
    // Where n is number of samples
    void calcNewAvg(int32_t* cum_avg, int32_t* new_sample, uint8_t arr_size, uint32_t n_samples) {
        
        for (int i = 0; i < arr_size; ++i) {
            uint32_t new_avg = *(cum_avg+i) + ((*(new_sample+i) - *(cum_avg+i)) / ((int32_t)n_samples + 1));
            *(cum_avg + i) = new_avg;
        }
    }

    void printAvg(uint32_t* cum_avg, uint8_t arr_size, uint32_t iteration) {
        
        Serial.println("-----Average retransmissions-----");
        Serial.print("Iteration: ");
        Serial.println(iteration);
        for (int i = 0; i < arr_size; ++i) {
            Serial.print("Ch");
            Serial.print(i+1);
            Serial.print(": ");
            Serial.println( *(cum_avg + i) );
        }
    }
};

extern AverageClass AVG;

#endif // AVERAGE_H
