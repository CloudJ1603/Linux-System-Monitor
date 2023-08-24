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
    virt = LinuxParser::Virt(id);
    ram = LinuxParser::Ram(id);
    // UpTime(id) provides a static number, which is a time stamp when the process start
    // while UpTime() provides a dynamic number, which the the system uptime in real
    uptime = LinuxParser::UpTime() - LinuxParser::UpTime(id);
    command = LinuxParser::Command(id);

    float seconds = static_cast<float>(uptime);
    float totaltime = static_cast<float>(LinuxParser::ActiveJiffies(id)) / sysconf(_SC_CLK_TCK);

    cpuUtilization = totaltime / seconds;
}

// Return this process's ID
int Process::Pid() { return pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() { return cpuUtilization; }

// Return the command that generated this process
string Process::Command() { 
    if(command.length() <= 40) {
        return command;
    } else {
        return command.substr(0,37) + "...";
    }
    return command; 
}

// Return the process's virtual memory utilization
string Process::Virt() {return virt;}

// Return this process's physical memory utilization
string Process::Ram() { return ram; }

// Return the user (name) that generated this process
string Process::User() { return user; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return uptime; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator>(Process const& other) const { 
    return  cpuUtilization > other.cpuUtilization; 
}