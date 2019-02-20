// Author: Gray Putnam
// Last Modified: 20 February 2019
// An implementation of the MetricPlugin interface for Redis

// TODO: error handling
// TODO: check if user provided "name" is a valid stream name for Redis

#include <algorithm>
#include <fstream>  

#include "fhiclcpp/fwd.h"
#include "artdaq-utilities/Plugins/MetricMacros.hh"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "hiredis/hiredis.h"
#include "hiredis/async.h"

#define TRACE_NAME "redis_metric"
#include "trace.h"
#define REDIS_TRACE_LEVEL_ERR 10
#define REDIS_TRACE_LEVEL_MSG 11

namespace sbndaq {
  void CallProcessRedisReply(redisAsyncContext *c, void *reply, void *plugin);

  class RedisMetric : public artdaq::MetricPlugin {
  private:
    std::string _server_name;
    std::string _redis_key_postfix;
    std::string _redis_password;
    unsigned _server_port;
    unsigned _maxlen;
    unsigned _message_buffer_size;
    unsigned _n_buffered_messages;
    bool _failed_connection;
    redisContext *_context;

    void newMessage() {
      _n_buffered_messages += 1;
			TLOG(23) << __func__ << ": _n_buffered_messages=" << _n_buffered_messages ;
      if (_n_buffered_messages >= _message_buffer_size) {
        while (_n_buffered_messages != 0) {
          void *reply = NULL;
          redisGetReply(_context, &reply);
          bool success = ProcessRedisReply(reply);
          // TODO: what to do on failure?
          (void) success;
          _n_buffered_messages -= 1;
        }
      }
    }

  public:
    RedisMetric(fhicl::ParameterSet const& pset, std::string const& app_name): 
      MetricPlugin(pset, app_name),
      _context(NULL)
    {
      _server_name = pset.get<std::string>("hostname", "localhost");
      _server_port = pset.get<unsigned>("port", 6379 /* default redis port */);
      _maxlen = pset.get<unsigned>("maxlen", 1000); // TODO: should maxlen have a max by default? Would be good to 
      _message_buffer_size = pset.get<unsigned>("message_buffer_size", 1); // don't buffer by default
      _n_buffered_messages = 0;
      _redis_key_postfix = pset.get<std::string>("redis_key_postfix", "");
      _failed_connection = false;
      _redis_password = pset.get<std::string>("redis_password", "");
      // also check for file if password not given
      if (_redis_password.size() == 0) {
        std::string redis_password_file = pset.get<std::string>("redis_passfile", "");
        if (redis_password_file.size() > 0) {
          std::ifstream passfile(redis_password_file);
          if (passfile.good()) {
            passfile >> _redis_password;
          }
          else {
            mf::LogError("Redis Metric Plugin") << "Failed to open password file";
            TLOG(REDIS_TRACE_LEVEL_ERR) << "Failed to open password file";
          }
        }
      }

    }

    virtual ~RedisMetric() {
      if (_context != NULL) {
      //  redisAsyncDisconnect(_context);
      }
    }

    // TODO: implement
    void ProcessRedisReturn(int retval) {}
    // TODO: implement
    bool ProcessRedisReply(void *r) {
      if (r == NULL) {
        mf::LogError("Redis Metric Plugin") << "Redis connection NULL reply";
        TLOG(REDIS_TRACE_LEVEL_ERR) << "Redis connection NULL reply";
        return false;
      }

      redisReply *reply = (redisReply *)r;
      switch (reply->type) {
        case REDIS_REPLY_ERROR:
          mf::LogError("Redis Metric Plugin") << "Redis connection error reply: " << reply->str;
          TLOG(REDIS_TRACE_LEVEL_ERR) << "Redis connection error reply: " << reply->str;
          return false;
        case REDIS_REPLY_STATUS:
          mf::LogDebug("Redis Metric Plugin") << "Message reply status: " << reply->str;
          TLOG(REDIS_TRACE_LEVEL_MSG) << "Message reply status: " << reply->str;
          break;
        default:
          break;
      }
      freeReplyObject(reply);
      return true;
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
          mf::LogError("Redis Metric Plugin") << "Redis connection error reply: " << _context->errstr;
          TLOG(REDIS_TRACE_LEVEL_ERR) << "Redis connection error reply: " << _context->errstr;
        }
        else {
          mf::LogError("Redis Metric Plugin") << "Cannot allocate redis context.";
          TLOG(REDIS_TRACE_LEVEL_ERR) << "Cannot allocate redis context.";
        }
        _failed_connection = true;
      }
      // setup password if neccessary
      if (!_failed_connection && _redis_password.size() > 0) {
        TLOG(REDIS_TRACE_LEVEL_MSG) << "Authenticating redis connection";
        mf::LogInfo("Redis Metric Plugin") << "Authenticating redis connection";
        void *reply = redisCommand(_context, "AUTH %s", _redis_password.c_str());
        bool success = ProcessRedisReply(reply);
        if (!success) {
          _failed_connection = true;
          TLOG(REDIS_TRACE_LEVEL_ERR) << "Redis authentication failed";
          mf::LogError("Redis Metric Plugin") << "Redis connection failed";
        }
        else {
          TLOG(REDIS_TRACE_LEVEL_MSG) << "Redis authentication succeeded";
          mf::LogInfo("Redis Metric Plugin") << "Redis authentication succeeded";
        }
      }
    }

    void sendMetric_(const std::string &name, const std::string &value, const std::string &units) {
      if (_failed_connection) {
        mf::LogWarning("Redis Metric Plugin") << "Attempting to send metric when connection failed.";
        TLOG(REDIS_TRACE_LEVEL_ERR) << "Attempting to send metric when connection failed.";
        return;
      }
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      mf::LogDebug("Redis Metric Plugin") << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      TLOG(REDIS_TRACE_LEVEL_MSG) <<  "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %s", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %s", redis_name.c_str(), _maxlen, value.c_str());
      newMessage();
    }

    void sendMetric_(const std::string &name, const int &value, const std::string &units) {
      if (_failed_connection) {
        mf::LogWarning("Redis Metric Plugin") << "Attempting to send metric when connection failed.";
        TLOG(REDIS_TRACE_LEVEL_ERR) << "Error in redis metric manager: attempting to send metric when connection failed.";
        return;
      }
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      mf::LogDebug("Redis Metric Plugin") << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %i", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %i", redis_name.c_str(), _maxlen, value);
      newMessage();
    }

    void sendMetric_(const std::string &name, const double &value, const std::string &units) {
      if (_failed_connection) {
        mf::LogWarning("Redis Metric Plugin") << "Attempting to send metric when connection failed.";
        TLOG(REDIS_TRACE_LEVEL_ERR) << "Error in redis metric manager: attempting to send metric when connection failed.";
        return;
      }
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      mf::LogDebug("Redis Metric Plugin") << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %f", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %f", redis_name.c_str(), _maxlen, value);
      newMessage();
    }

    void sendMetric_(const std::string &name, const float &value, const std::string &units) {
      if (_failed_connection) {
        mf::LogWarning("Redis Metric Plugin") << "Attempting to send metric when connection failed.";
        TLOG(REDIS_TRACE_LEVEL_ERR) << "Error in redis metric manager: attempting to send metric when connection failed.";
        return;
      }
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      mf::LogDebug("Redis Metric Plugin") << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      //int ret = redisAsyncCommand(_context, CallProcessRedisReply, this, "XADD %s MAXLEN ~ %i * dat %f", redis_name.c_str(), _maxlen, value);
      //ProcessRedisReturn(ret);
      redisAppendCommand(_context, "XADD %s MAXLEN ~ %i * dat %f", redis_name.c_str(), _maxlen, value);
      newMessage();
    }

    void sendMetric_(const std::string &name, const unsigned long int &value, const std::string &units) {
      if (_failed_connection) {
        mf::LogWarning("Redis Metric Plugin") << "Attempting to send metric when connection failed.";
        TLOG(REDIS_TRACE_LEVEL_ERR) << "Error in redis metric manager: attempting to send metric when connection failed.";
        return;
      }
      (void) units;
      std::string redis_name = ValidateRedisName(name);
      mf::LogDebug("Redis Metric Plugin") << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
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

