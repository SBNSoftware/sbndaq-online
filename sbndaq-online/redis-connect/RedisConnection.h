#ifndef _sbndaq_online_RedisConnection_h_
#define _sbndaq_online_RedisConnection_h_
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "../hiredis/hiredis.h"
#include "../hiredis/async.h"

namespace sbndaq {
class RedisConnection {
public:
  RedisConnection(const fhicl::ParameterSet& pset, art::ActivityRegistry& reg); 
  explicit RedisConnection(const fhicl::ParameterSet& pset);
  void reconfigure(const fhicl::ParameterSet& pset); 

  void Flush();
  void FlushPostModule(const art::ModuleContext &context);
  void SetFlushPostEvent(bool set) { fFlushPostEvent = set; }

  void Command(const char *fmt, ...);
  void CommandArgv(int argc, const char **argv, const size_t *argvlen);
  virtual ~RedisConnection();

private:

  void DoFlush();
  bool CheckConnection();
  void NewMessage();
  void ProcessRedisReturn(int retval);
  bool ProcessRedisReply(void *reply);

  unsigned fRedisPort;
  unsigned fMessageBufferSize;
  unsigned fNMessages;
  std::string fRedisPassword;
  std::string fRedisHost; 
  redisContext *fRedisContext;
  bool fFailedConnection;
  bool fFlushPostEvent;
};

} // end namespace sbndaq

DECLARE_ART_SERVICE(sbndaq::RedisConnection, LEGACY)
#endif
