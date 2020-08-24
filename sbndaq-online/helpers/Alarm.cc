#include "sbndaq-online/redis-connect/RedisConnectionService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "Alarm.h"
#include "EventMeta.h"
#include <chrono>
#include <ctime>
#include <time.h>

using namespace std::chrono;

void sbndaq::SendAlarm(const std::string &alarm, const art::Event &event, std::string description, bool print) {
  art::ServiceHandle<RedisConnectionService> redis;
  // send meta-data information
  sbndaq::SendEventMeta(alarm, event);

  // update counter
  redis->Command("HINCRBY %s count 1", alarm.c_str());

  // set description 
  if (description == "") description = "empty";

  // set the description
  redis->Command("HMSET %s description %s", alarm.c_str(), description.c_str());

  // set to expire at the end of the day
  // get now
  system_clock::time_point now = system_clock::now();
  // convert to ctime to set times    
  time_t ctime = std::chrono::system_clock::to_time_t(now);
  struct tm tmval;
  struct tm *timeset = localtime_r(&ctime, &tmval);
  assert(timeset);
  // set to midnight tomorrow
  timeset->tm_hour = 0;
  timeset->tm_min = 0;
  timeset->tm_sec = 0;
  timeset->tm_mday += 1;

  // get us back to a unix timestamp
  time_t time_tval = std::mktime(timeset);
  int tstmp = static_cast<long int>(time_tval);

  redis->Command("EXPIREAT %s %lld", alarm.c_str(), tstmp);

  // store the alarm in the list and also expire that at midnight
  redis->Command("SADD ALARMS %s", alarm.c_str());
  redis->Command("EXPIREAT ALARMS %lld", tstmp);

  // if configured to, also print the error
  if (print) {
    int64_t time = ((int64_t)std::time(NULL)) * 1000; // s -> ms
    int run = event.getRun().run();
    int subrun = event.getSubRun().subRun();
    int eventID = event.id().event();
    std::cerr << "ALARM: (" << alarm << ").\nWith description: (" << description << ").\nAt timestamp (" << time << ") Run (" << run << ") Subrun (" << subrun << " Event (" << eventID << ")\n";
  }
}
