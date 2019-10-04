#ifndef _sbndaq_online_Binary_hh
#define _sbndaq_online_Binary_hh
#include <string>

namespace sbndaq {
  void SendBinary(const std::string &key, char *data, unsigned length);
}


#endif
