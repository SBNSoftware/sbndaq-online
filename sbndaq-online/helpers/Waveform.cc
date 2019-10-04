#include <stdlib.h>

#include "sbndaq-online/hiredis/hiredis.h"
#include "sbndaq-online/hiredis/async.h"

#include "Waveform.h"

int sbndaq::SendRedisCommand(redisContext *redis, bool pipeline, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  int ret = 0;
  // TODO: error handling;
  if (pipeline) {
    redisvAppendCommand(redis, fmt, argp); 
  }
  else {
    void *reply = redisvCommand(redis, fmt, argp); 
    (void) reply;
  }
  va_end(argp);
  return ret;
}

int sbndaq::SendRedisCommandArgv(redisContext *c, bool pipeline, int argc, const char **argv, const size_t *argvlen) {
  if (pipeline) {
    redisAppendCommandArgv(c, argc, argv, argvlen);
  }
  else {
    void *reply= redisCommandArgv(c, argc, argv, argvlen);
    std::cout << ((redisReply *)reply)->str << std::endl;
    (void)reply;
  }
  return 0;

}


