#ifndef _sbndaq_online_RedisConnectionService_h_
#define _sbndaq_online_RedisConnectionService_h_
#include "RedisConnection.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace sbndaq {
class RedisConnectionService : public RedisConnection {
public:
  RedisConnectionService(const fhicl::ParameterSet& pset, art::ActivityRegistry& reg);
  void FlushPostEvent(art::Event const &ev, art::ScheduleContext sched);
  void StopMetrics();
};
} // end namespace sbndaq

DECLARE_ART_SERVICE(sbndaq::RedisConnectionService, LEGACY)
#endif
