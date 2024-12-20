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

#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "../hiredis/hiredis.h"
#include "../hiredis/async.h"

#include "../helpers/Utilities.h"
#include "../redis-connect/RedisConnectionService.h"

#include "../DisplayTypes.h"

#include "../RedisTrace.h"

namespace sbndaq {
  class RedisMetric : public artdaq::MetricPlugin {
  private:
    std::string _redis_key_postfix;
    std::string _redis_key_prefix;
    unsigned _stream_maxlen;
    RedisConnection *_redis;
    bool _owns_redis_connection;
    std::set<std::string> _sent_metrics;

    template<typename DataType>
    void SendBinaryMetric(const std::string &inp_name, DataType dat) {
      std::string meta = ValidateRedisName(GetMeta(inp_name));
      std::string name = StripMeta(inp_name);
      std::string redis_name = ValidateRedisName(_redis_key_prefix + name + _redis_key_postfix);
      mf::LogDebug("Redis Metric Plugin") << "Adding metric to stream: (" << redis_name << ") with value (" << dat << ")";
      TLOG(REDIS_TRACE_LEVEL_MSG) << "Adding metric to stream: (" << redis_name << ") with value (" << dat << ")";

      if (meta.size()) {
        _redis->Command("XADD %s MAXLEN ~ %i * %s %b %s _", redis_name.c_str(), _stream_maxlen, DisplayType<DataType>::name, &dat, sizeof(DataType), meta.c_str()); 
      }
      else {
        _redis->Command("XADD %s MAXLEN ~ %i * %s %b", redis_name.c_str(), _stream_maxlen, DisplayType<DataType>::name, &dat, sizeof(DataType)); 
      }
    }

  public:
    RedisMetric(fhicl::ParameterSet const& pset, std::string const& app_name, std::string plugin_name="RedisMetricPlugin"):
      MetricPlugin(pset, app_name, plugin_name)
    {
      _redis_key_postfix = pset.get<std::string>("redis_key_postfix", "");
      _redis_key_prefix = pset.get<std::string>("redis_key_prefix", "");
      _stream_maxlen = pset.get<unsigned>("maxlen", 1000); // TODO: should maxlen have a max by default? Would be good to 

      if (pset.has_key("redis") && pset.get<bool>("use_local_redis", true)) {
        _redis = new RedisConnection(pset.get<fhicl::ParameterSet>("redis"));
        _owns_redis_connection = true;
      }
      else {
        art::ServiceHandle<sbndaq::RedisConnectionService> handle;
        _redis = handle.get();
        _owns_redis_connection = false;
      }
    }

    void stopMetrics_() {}

    void startMetrics_() {}
 
    virtual ~RedisMetric() {
      if (_owns_redis_connection && _redis != NULL) delete _redis;
    }

    void sendMetric_(const std::string &inp_name, const std::string &value, const std::string &units, 
		     const std::chrono::system_clock::time_point& ) {
      (void) units;

      if (!_sent_metrics.count(inp_name)) {
        _sent_metrics.insert(inp_name);
        return;
      }

      std::string meta = ValidateRedisName(GetMeta(inp_name));
      std::string name = StripMeta(inp_name);
      std::string redis_name = ValidateRedisName(_redis_key_prefix + name + _redis_key_postfix);
      mf::LogDebug("Redis Metric Plugin") << "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      TLOG(REDIS_TRACE_LEVEL_MSG) <<  "Adding metric to stream: (" << redis_name << ") with value (" << value << ")";
      if (meta.size()) {
        _redis->Command("XADD %s MAXLEN ~ %i * %s %s %s _", redis_name.c_str(), _stream_maxlen, "string", value.c_str(), meta.c_str());
      }
      else {
        _redis->Command("XADD %s MAXLEN ~ %i * %s %s", redis_name.c_str(), _stream_maxlen, "string", value.c_str());
      }
    }

    void sendMetric_(const std::string &name, const int &value, const std::string &units,
		     const std::chrono::system_clock::time_point& ) {
      (void) units;
      if (!_sent_metrics.count(name)) {
        _sent_metrics.insert(name);
        return;
      }
      SendBinaryMetric(name, value);
    }

    void sendMetric_(const std::string &name, const double &value, const std::string &units,
		     const std::chrono::system_clock::time_point& ) {
      (void) units;
      if (!_sent_metrics.count(name)) {
        _sent_metrics.insert(name);
        return;
      }
      SendBinaryMetric(name, (float)value);
    }

    void sendMetric_(const std::string &name, const float &value, const std::string &units,
		     const std::chrono::system_clock::time_point& ) {
      (void) units;
      if (!_sent_metrics.count(name)) {
        _sent_metrics.insert(name);
        return;
      }
      SendBinaryMetric(name, value);
    }

    void sendMetric_(const std::string &name, const unsigned long int &value, const std::string &units,
		     const std::chrono::system_clock::time_point& ) {
      (void) units;
      if (!_sent_metrics.count(name)) {
        _sent_metrics.insert(name);
        return;
      }
      SendBinaryMetric(name, value);
    }
  
  };

} // end namespace sbndaq

DEFINE_ARTDAQ_METRIC(sbndaq::RedisMetric)

