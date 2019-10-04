#ifndef _sbndaq_online_Binary_hh
#define _sbndaq_online_Binary_hh
#include <string>

class redisContext;

namespace sbndaq {
  int SendBinary(redisContext *redis, const std::string &key, char *data, unsigned length);
}


#endif
