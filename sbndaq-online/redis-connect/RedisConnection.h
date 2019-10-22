#ifndef _sbndaq_online_RedisConnection_h_
#define _sbndaq_online_RedisConnection_h_

#include <mutex>

#include "fhiclcpp/ParameterSet.h"
#include "../hiredis/hiredis.h"
#include "../hiredis/async.h"

namespace sbndaq {
class RedisConnection {
public:
  explicit RedisConnection(const fhicl::ParameterSet& pset);
  void reconfigure(const fhicl::ParameterSet& pset); 

  void Flush();

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
  int fMessageBufferSize;
  unsigned fNMessages;
  std::string fRedisPassword;
  std::string fRedisHost; 
  redisContext *fRedisContext;
  bool fFailedConnection;
  std::mutex fRedisLock;
};

} // end namespace sbndaq
#endif
