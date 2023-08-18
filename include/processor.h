#ifndef PROCESSOR_H
#define PROCESSOR_H

#include<linux_parser.h>

class Processor {
 public:
  float Utilization();  

  
 private:
  long preTotal {-1};
  long preActive {-1};
};

#endif