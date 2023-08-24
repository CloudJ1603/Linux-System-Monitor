#include <string>
#include<chrono>
#include "format.h"

using std::string;
using namespace std::chrono;



// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long sec) { 
    // long hh, mm, ss;
    // hh = s / 3600;
    // s %= 3600;
    // mm = s / 60;
    // ss = s % 60;

    // std::ostringstream timeStream;
    // timeStream << std::setw(2) << std::setfill('0') << hh << ":"
    //            << std::setw(2) << std::setfill('0') << mm << ":"
    //            << std::setw(2) << std::setfill('0') << ss;
    // return timeStream.str(); 

    seconds s{sec};
    hours h =  duration_cast<hours>(s);
    s -= duration_cast<seconds>(h);

    minutes m = duration_cast<minutes>(s);
    s -= duration_cast<seconds>(m);

    std::stringstream ss{};
    ss << std::setw(2) << std::setfill('0') << h.count() << ':'
       << std::setw(2) << std::setfill('0') << m.count() << ':'
       << std::setw(2) << std::setfill('0') << s.count();

    return ss.str();


}