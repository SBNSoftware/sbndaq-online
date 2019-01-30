// Author: Gray Putnam
// Last Modified: 18 December 2018
// An implementation of the MetricPlugin interface for Redis

// TODO: error handling
// TODO: check if user provided "name" is a valid stream name for Redis

#include <algorithm>

#include "fhiclcpp/fwd.h"
#include "artdaq-utilities/Plugins/MetricMacros.hh"

#include "hiredis/hiredis.h"
#include "hiredis/async.h"

namespace sbndaq {
  void CallProcessRedisReply(redisAsyncContext *c, void *reply, void *plugin);

  class RedisMetric : public artdaq::MetricPlugin {
  private:
    bool _verbose;
    std::string _server_name;
    std::string _redis_key_postfix;
    unsigned _server_port;
    unsigned _maxlen;
    unsigned _message_buffer_size;
    unsigned _n_buffered_messages;
    redisContext *_context;


    void newMessage() {
      _n_buffered_messages += 1;
      if (_n_buffered_messages >= _message_buffer_size) {
        for (unsigned i = 0; i < _n_buffered_messages; i++) {
          void *reply;
          redisGetReply(_context, &reply);
          ProcessRedisReply(reply);
        }
        _n_buffered_messages = 0;
      }
    }

  public:
    RedisMetric(fhicl::ParameterSet const& pset, std::string const& app_name): 
      MetricPlugin(pset, app_name),
      _verbose(pset.get<bool>("verbose",false)),
      _context(NULL)
    {
      _server_name = pset.get<std::string>("hostname", "localhost");
      _server_port = pset.get<unsigned>("port", 6379 /* default redis port */);
      _maxlen = pset.get<unsigned>("maxlen", 1000); // TODO: should maxlen have a max by default? Would be good to 
      _message_buffer_size = pset.get<unsigned>("message_buffer_size", 1); // don't buffer by default
      _n_buffered_messages = 0;
      _redis_key_postfix = pset.get<std::string>("redis_key_postfix", "");
    }

    virtual ~RedisMetric() {
      if (_context != NULL) {
      //  redisAsyncDisconnect(_context);
      }
    }

    // TODO: implement
    void ProcessRedisReturn(int retval) {}
    // TODO: implement
    void ProcessRedisReply(void *reply) {
      freeReplyObject(reply);
    }
    // TODO: implement
    std::string ValidateRedisName(const std::string &name) { 
      // add the post fix
      std::string ret = name + _redis_key_postfix;
      // remove the dots...
      ret.erase(std::remove(ret.begin(), ret.end(), '.'), ret.end());
      // replace all spaces w/ underscores
      std::replace(ret.begin(), ret.end(), ' ', '_');
      return ret;
    }

    void stopMetrics_() {
      if (_context != NULL) {
      //  redisAsyncDisconnect(_context);
      }
    }

    void startMetrics_() {
      _context = redisConnect(_server_name.c_str(), _server_port);
      //_context = redisAsyncConnect(_server_name.c_str(), _server_port);
      if (_context == NULL || _context->err) {
        if (_context) {
          std::cerr << "Error: " << _context->errstr << std::endl;
        }
        else {
          std::cerr << "Error: cannot allocate redis context." << std::endl;
        }
      }
    }

    void sendMetric_(const std::string &name, const std::string &value, const std::string &units) {
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      if (_verbose) std::cout << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")" << std::endl;
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %s", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %s", redis_name.c_str(), _maxlen, value.c_str());
      newMessage();
    }

    void sendMetric_(const std::string &name, const int &value, const std::string &units) {
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      if (_verbose) std::cout << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")" << std::endl;
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %i", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %i", redis_name.c_str(), _maxlen, value);
      newMessage();
    }

    void sendMetric_(const std::string &name, const double &value, const std::string &units) {
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      if (_verbose) std::cout << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")" << std::endl;
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %f", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %f", redis_name.c_str(), _maxlen, value);
      newMessage();
    }

    void sendMetric_(const std::string &name, const float &value, const std::string &units) {
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      if (_verbose) std::cout << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")" << std::endl;
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %f", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %f", redis_name.c_str(), _maxlen, value);
      newMessage();
    }

    void sendMetric_(const std::string &name, const unsigned long int &value, const std::string &units) {
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      if (_verbose) std::cout << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")" << std::endl;
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %u", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %u", redis_name.c_str(), _maxlen, value);
      newMessage();
    }
  };

  void CallProcessRedisReply(redisAsyncContext *c, void *reply, void *plugin) {
    // "plugin" should be an instance of RedisMetric
    ((RedisMetric *)plugin)->ProcessRedisReply(reply);
  }


} // end namespace sbndaq

DEFINE_ARTDAQ_METRIC(sbndaq::RedisMetric)

