#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    long hh, mm, ss;
    hh = seconds / 3600;
    seconds %= 3600;
    mm = seconds / 60;
    ss = seconds % 60;

    std::ostringstream timeStream;
    timeStream << std::setw(2) << std::setfill('0') << hh << ":"
               << std::setw(2) << std::setfill('0') << mm << ":"
               << std::setw(2) << std::setfill('0') << ss;
    return timeStream.str(); 
}