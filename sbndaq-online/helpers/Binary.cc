#include <string>

#include "sbndaq-online/hiredis/hiredis.h"
#include "sbndaq-online/hiredis/async.h"

#include "Binary.h"

int sbndaq::SendBinary(redisContext *redis, const std::string &key, char *data, unsigned length) {
  void *reply = redisCommand(redis, "SET %s %b", key.c_str(), data, length);
  (void) reply;
  return 0;
}


