#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    // float idle = LinuxParser::IdleJiffies();
    return (float)LinuxParser::ActiveJiffies() / (float)LinuxParser::Jiffies();
}