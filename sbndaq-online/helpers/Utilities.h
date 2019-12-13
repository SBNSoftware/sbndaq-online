#ifndef __SBNDAQ_ONLINE_UTILITIES_HH_
#define __SBNDAQ_ONLINE_UTILITIES_HH_

#include <algorithm>
#include <string>

#include "../hiredis/hiredis.h"
#include "../hiredis/async.h"


namespace sbndaq {

redisContext *Connect2Redis(const std::string &server_name, int server_port, const std::string &password="", void **password_reply=NULL) {
  redisContext *context = redisConnect(server_name.c_str(), server_port);
  //_context = redisAsyncConnect(server_name.c_str(), server_port);
  if (context == NULL || context->err) {
    return context;
  }

  // setup password if neccessary
  if (password.size() > 0) {
    void *reply = redisCommand(context, "AUTH %s", password.c_str());
    if (password_reply != NULL) {
      *password_reply = reply;     
    }
  }
  return context;
}

// TODO: implement
std::string ValidateRedisName(const std::string &name) {
  std::string ret = name;
  // remove the dots...
  ret.erase(std::remove(ret.begin(), ret.end(), '.'), ret.end());
  // replace all spaces w/ underscores
  std::replace(ret.begin(), ret.end(), ' ', '_');
  return ret;
}

std::string GetMeta(const std::string &name) {
  for (unsigned i = 0; i < name.size(); i++) {
    if (name[i] == '/') {
      return name.substr(0, i);
    }
  }
  return "";
}

std::string StripMeta(const std::string &name) {
  for (unsigned i = 0; i < name.size(); i++) {
    if (name[i] == '/') {
      if (i+1 == name.size()) return "";
      return name.substr(i+1);
    }
  }
  return name;
}



} // end namespace sbndaq

#endif 
