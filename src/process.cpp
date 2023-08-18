#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include<iostream>

#include "process.h"
#include"linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int id) {
    pid = id;
    user = LinuxParser::User(id); 
    ram = LinuxParser::Ram(id);
    uptime = LinuxParser::UpTime(id);
    command = LinuxParser::Command(id);

    float seconds = static_cast<float>(LinuxParser::UpTime() - uptime);
    float totaltime = static_cast<float>(LinuxParser::ActiveJiffies(id)) / sysconf(_SC_CLK_TCK);

    cpuUtilization = totaltime / seconds;
}

// TODO: Return this process's ID
int Process::Pid() { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    return cpuUtilization; 
}

// TODO: Return the command that generated this process
string Process::Command() { return command; }

// TODO: Return this process's memory utilization
string Process::Ram() { 
    // return "abcd";       // succeed
    // return string();      // succeed
    return ram;          // fail
}

// TODO: Return the user (name) that generated this process
string Process::User() { return user; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return a.cpuUtilization < cpuUtilization; 
}