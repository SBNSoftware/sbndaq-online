#include "RedisConnectionService.h"

#include "sbndaq-online/helpers/SBNMetricManager.h"

sbndaq::RedisConnectionService::RedisConnectionService(const fhicl::ParameterSet& pset, art::ActivityRegistry& reg):
  RedisConnection(pset)
{
  bool do_flush_post_event = pset.get<bool>("flush_post_event", true);
  if (do_flush_post_event) {
    reg.sPostProcessEvent.watch(this, &RedisConnectionService::FlushPostEvent);
  }

  // stop down any metric manager before the service destructors run
  reg.sPostEndJob.watch(this, &RedisConnectionService::StopMetrics);
}

void sbndaq::RedisConnectionService::FlushPostEvent(art::Event const &ev, art::ScheduleContext sched) {
  (void) ev;
  (void) sched;
  Flush();
}

void sbndaq::RedisConnectionService::StopMetrics() {
  sbndaq::stopMetrics();
  Flush();
}


DEFINE_ART_SERVICE(sbndaq::RedisConnectionService)
