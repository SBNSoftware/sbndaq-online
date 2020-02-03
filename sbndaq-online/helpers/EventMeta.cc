#include "sbndaq-online/redis-connect/RedisConnectionService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "EventMeta.h"
#include <chrono>

using namespace std::chrono;

void sbndaq::SendEventMeta(const std::string &key, const art::Event &event) {
  art::ServiceHandle<RedisConnectionService> redis;

  milliseconds ms = duration_cast< milliseconds >(
      system_clock::now().time_since_epoch()
  );
  int run = event.getRun().run();
  int subrun = event.getSubRun().subRun();
  int eventID = event.id().event();

  redis->Command("HMSET %s %s %i %s %i %s %i %s %i", key.c_str(), "run", run, "subrun", subrun, "event", eventID, "time", (int)ms.count()); 
}
