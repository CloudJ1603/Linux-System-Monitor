#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include<iostream>
#include <unistd.h>
#include <filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &fileName) {
  std::string line, key;
  T value;

  std::ifstream filestream(LinuxParser::kProcDirectory + fileName);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key.compare(keyFilter) == 0) {
        return value;
      }
    }
  }

  return value;
}

template<typename T>
T findValueOfFile(std::string const &filename) {
  std::string line;
  T value;
  std::ifstream filestream(LinuxParser::kProcDirectory + filename);
  
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
}

// read and return the OS information
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// read and return the kernel information
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Update this to use std::filesystem
// Read and return the system pids

// vector<int> LinuxParser::Pids() {
//   vector<int> pids;
//   DIR* directory = opendir(kProcDirectory.c_str());
//   struct dirent* file;
//   while ((file = readdir(directory)) != nullptr) {
//     // Is this a directory?
//     if (file->d_type == DT_DIR) {
//       // Is every character of the name a digit?
//       string filename(file->d_name);
//       if (std::all_of(filename.begin(), filename.end(), isdigit)) {
//         int pid = stoi(filename);
//         pids.push_back(pid);
//       }
//     }
//   }
//   closedir(directory);
//   return pids;
// }

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for (auto& p : std::filesystem::directory_iterator(kProcDirectory)) {
    string filename = p.path().filename();
    if (p.is_directory() && std::all_of(filename.begin(), filename.end(), isdigit)) {
      int pid = stoi(filename);
      pids.push_back(pid);
    }
  }
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float res, memTotal, memFree;

  try {
    memTotal = stof(findValueByKey<string>(filterMemTotal, kMeminfoFilename));
    memFree = stof(findValueByKey<string>(filterMemFree, kMeminfoFilename));
    res = (memTotal - memFree) / memTotal;
  } catch (std::invalid_argument &e) {
    // do nothing
  }
  return res; 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  long res = 0;
  string line;
  string uptime, idle;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
  }

  try {
    res = stol(uptime);
  } catch (std::invalid_argument &e) {

  }
  return res; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  long res = 0;
  string line;
  string var;
  string utime, stime, cutime, cstime, starttime;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int i = 0; i < 13; i++) {
      linestream >> var;
    }
    linestream >> utime >> stime >> cutime >> cstime;
  }

  try {
    res = stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
  } catch(std::invalid_argument &e) {
    // do nothing
  }
  return res; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long res = 0;
  vector<string> jiffies = LinuxParser::CpuUtilization();
  try {
    res = stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) +
         stol(jiffies[CPUStates::kSteal_]);
  } catch (std::invalid_argument &e) {
    // do nothing
  }
  return res; 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long res = 0;
  vector<string> jiffies = LinuxParser::CpuUtilization();
  try {
    res = stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
  } catch (std::invalid_argument &e) {
    // do nothing
  }
  return res;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string cpu, num;
  vector<string> jiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    if(cpu.compare(filterCpu) == 0) {
      while(linestream >> num) {
        jiffies.push_back(num);
      }
      return jiffies;
    }
  }
  return {}; 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  long res = 0;
  try {
    res = stoi(findValueByKey<string>(filterProcess, kStatFilename));
  } catch(std::invalid_argument &e) {
    // do nothing
  }
  return res; 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int res = 0; 
  try {
    res = stoi(findValueByKey<string>(filterRunningProcess, kStatFilename));
  } catch (std::invalid_argument &e) {
    // do nothing
  }
  return res; 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  return findValueOfFile<std::string>(to_string(pid) + kCmdlineFilename);
}

// Read and return the virtual memory used by a process
string LinuxParser::Virt(int pid) { 
  return findValueByKey<string>(filterProcessVirtualMemSize, to_string(pid) + kStatusFilename); 
}

// Read and return the physical memory used by a process
string LinuxParser::Ram(int pid) { 
  return findValueByKey<string>(filterProcessPhysicalMemSize, to_string(pid) + kStatusFilename); 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
return findValueByKey<string>(filterUid, to_string(pid) + kStatusFilename); 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid = LinuxParser::Uid(pid);
  string line;
  std::ifstream filestream(kPasswordPath);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      string user, x, id;
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> user >> x >> id) {
        if(id.compare(uid) == 0) {
          return user;
        }
      }
    }
  }
  return string(); 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line;
  string uptime;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int i = 0; i < 22; i++) {
      /*The time the process started after system boot
      NOTE that this is a constant number, to make it dynamic
      subtract it from the system uptime*/
      linestream >> uptime;
    }
  }
  double pid_time {0};
  /*
  Without try and catch, a segmentation fault will happen.  an exception of 
  `invalid_argument` type is thrown due to input provided to stol when calculating 
  process uptime. 
  This is because a process ended after its PID was collected but before the calculation
  for the uptime 
  */
  try {
    pid_time = stol(uptime) / sysconf(_SC_CLK_TCK);
  } catch (std::invalid_argument &e) {
    pid_time = 0;
  }
  return (long) pid_time;
}
