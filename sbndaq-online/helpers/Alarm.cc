#include "sbndaq-online/redis-connect/RedisConnectionService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "Alarm.h"
#include "EventMeta.h"
#include <chrono>
#include <ctime>
#include <time.h>
#include <unistd.h>

using namespace std::chrono;

void sbndaq::SendAlarm(const std::string &alarm, const art::Event &event, std::string description, bool print) {
  art::ServiceHandle<RedisConnectionService> redis;
  // send meta-data information
  sbndaq::SendEventMeta(alarm, event);

  // update counter
  redis->Command("HINCRBY %s count 1", alarm.c_str());
    
  int64_t time = ((int64_t)std::time(NULL)) * 1000; // s -> ns

  // set the first reported time for this alarm
  redis->Command("HSETNX %s start %lld", alarm.c_str(), time);

  // set description 
  if (description == "") description = "empty";

  // set the description
  redis->Command("HMSET %s description %s", alarm.c_str(), description.c_str());

  // get the log file
  char logfile[1001];
  // NOTE: LUNIX ONLY
  // get the name of the logfile (stdout) from /proc
  ssize_t len = readlink("/proc/self/fd/1", logfile, 1000);
  // readlink does not NULL-terminate
  if (len != -1) {
      logfile[len] = '\0';
  }
  else { // failed
    strcpy(logfile, "UNAVAILABLE");
  }
  redis->Command("HMSET %s logfile %s", alarm.c_str(), logfile);

  // Set expire
  // get now
  system_clock::time_point now = system_clock::now();
  // convert to ctime to set times    
  time_t ctime = std::chrono::system_clock::to_time_t(now);
  struct tm tmval;
  struct tm *timeset = localtime_r(&ctime, &tmval);
  assert(timeset);

  // the two expires
  struct tm next_midday = *timeset;
  struct tm next_midnight = *timeset;

  // set to midnight tomorrow
  next_midnight.tm_hour = 0;
  next_midnight.tm_min = 0;
  next_midnight.tm_sec = 0;
  next_midnight.tm_mday += 1;

  // set to next midday
  if (timeset->tm_hour >= 12) next_midday.tm_mday += 1;
  next_midday.tm_hour = 12;
  next_midday.tm_min = 0;
  next_midday.tm_sec = 0;

  // get us back to a unix timestamp
  time_t time_tval_night = std::mktime(&next_midnight);
  int tstmp_night = static_cast<long int>(time_tval_night);

  time_t time_tval_day = std::mktime(&next_midday);
  int tstmp_day = static_cast<long int>(time_tval_day);

  // this alarm expires at the latter of the two timestamps
  redis->Command("EXPIREAT %s %lld", alarm.c_str(), std::max(tstmp_night, tstmp_day));

  // store the alarm in the list and also expire that at midnight
  redis->Command("SADD ALARMS_MIDNIGHT %s", alarm.c_str());
  redis->Command("EXPIREAT ALARMS_MIDNIGHT %lld", tstmp_night);

  // and midday
  redis->Command("SADD ALARMS_MIDDAY %s", alarm.c_str());
  redis->Command("EXPIREAT ALARMS_MIDDAY %lld", tstmp_day);

  // if configured to, also print the error
  if (print) {
    int run = event.id().run();
    int subrun = event.id().subRun();
    int eventID = event.id().event();
    std::cout << "ALARM: (" << alarm << ").\nWith description: (" << description << ").\nAt timestamp (" << time << ") Run (" << run << ") Subrun (" << subrun << " Event (" << eventID << ")\n";
  }
}
