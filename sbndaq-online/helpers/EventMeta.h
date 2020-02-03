#ifndef _sbndaq_online_EventMeta_hh
#define _sbndaq_online_EventMeta_hh

#include <string>

#include "art/Framework/Principal/Event.h"

namespace sbndaq {
  void SendEventMeta(const std::string &key, const art::Event &event);
}


#endif
