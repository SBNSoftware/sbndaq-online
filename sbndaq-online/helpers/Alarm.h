#ifndef _sbndaq_online_Alarm_hh
#define _sbndaq_online_Alarm_hh

#include <string>

#include "art/Framework/Principal/Event.h"

namespace sbndaq {
  void SendAlarm(const std::string &alarm, const art::Event &event, std::string description="", bool print=true);
}


#endif
