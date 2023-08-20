#include "processor.h"

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    float utilization;
    long total = LinuxParser::Jiffies();
    long active = LinuxParser::ActiveJiffies();
    if(preActive <= 0 && preTotal <= 0) {  // first time
        utilization = (float)active / (float)total;
    } else {
        utilization = (float) (active - preActive) / (float) (total - preTotal);
    }
    preTotal = total;
    preActive = active;
    return utilization;
}