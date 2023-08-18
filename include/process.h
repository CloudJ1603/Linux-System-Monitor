#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
*/
class Process {
 public:
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization();                 
  std::string Ram();                      
  long int UpTime();                       
  bool operator>(Process const& a) const;  
  Process(int pid);
 
 private:
  int pid = 0;
  std::string user;
  float cpuUtilization;
  std::string ram;
  long int uptime;
  std::string command;
};

#endif