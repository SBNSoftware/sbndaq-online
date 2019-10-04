#ifndef _sbndaq_online_RedisConnectionService_h_
#define _sbndaq_online_RedisConnectionService_h_
#include "RedisConnection.h"

namespace sbndaq {
class RedisConnectionService : public RedisConnection {
public:
  RedisConnectionService(const fhicl::ParameterSet& pset, art::ActivityRegistry& reg):
    RedisConnection(pset, reg) {}
  explicit RedisConnectionService(const fhicl::ParameterSet& pset):
    RedisConnection(pset) {}
};
} // end namespace sbndaq

DECLARE_ART_SERVICE(sbndaq::RedisConnectionService, LEGACY)
#endif
