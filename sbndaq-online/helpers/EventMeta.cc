#include "sbndaq-online/redis-connect/RedisConnectionService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "EventMeta.h"
#include <chrono>
#include <ctime>

using namespace std::chrono;

void sbndaq::SendEventMeta(const std::string &key, const art::Event &event) {
  art::ServiceHandle<RedisConnectionService> redis;

  //milliseconds ms = duration_cast< milliseconds >(
  //    system_clock::now().time_since_epoch()
  //);
  int64_t time = ((int64_t)std::time(NULL)) * 1000; // s -> ms
  
  int run = event.id().run();
  int subrun = event.id().subRun();
  int eventID = event.id().event();

  redis->Command("HMSET %s %s %i %s %i %s %i %s %lld", key.c_str(), "run", run, "subrun", subrun, "event", eventID, "time", time); 
}
