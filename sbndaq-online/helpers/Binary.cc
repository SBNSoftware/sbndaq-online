#include <string>

#include "sbndaq-online/redis-connect/RedisConnectionService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "Binary.h"

void sbndaq::SendBinary(const std::string &key, char *data, unsigned length) {
  art::ServiceHandle<RedisConnectionService> redis;
  redis->Command("SET %s %b", key.c_str(), data, length);
}


