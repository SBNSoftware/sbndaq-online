#include "RedisConnectionService.h"

sbndaq::RedisConnectionService::RedisConnectionService(const fhicl::ParameterSet& pset, art::ActivityRegistry& reg):
  RedisConnection(pset)
{
  bool do_flush_post_event = pset.get<bool>("flush_post_event", true);
  if (do_flush_post_event) {
    reg.sPostProcessEvent.watch(this, &RedisConnectionService::FlushPostEvent);
  }
}

void sbndaq::RedisConnectionService::FlushPostEvent(art::Event const &ev, art::ScheduleContext sched) {
  (void) ev;
  (void) sched;
  Flush();
}

DEFINE_ART_SERVICE(sbndaq::RedisConnectionService)
