#include <array>
#include <string>
#include <experimental/optional>

#include "fhiclcpp/ParameterSet.h"

namespace sbndaq {
  class RedisConnection;

  void GenerateMetricConfig(const fhicl::ParameterSet &config);

  struct MetricConfig {
    std::experimental::optional<std::array<double, 2>> display_range;
    std::experimental::optional<std::array<double, 2>> warning_range;
    std::experimental::optional<std::array<double, 2>> alarm_range;
    std::experimental::optional<std::string> units;
    std::experimental::optional<std::string> title;
    std::experimental::optional<std::string> format;
  };

  void AddGroup(RedisConnection *redis, const std::string &group_name);
  void GroupMetricConfig(RedisConnection *redis, const std::string &group_name, 
    const std::vector<std::string> &metric_names, const std::vector<MetricConfig> &metric_configs,
    const std::vector<std::string> &stream_names);
  void AddInstances2Group(RedisConnection *redis, const std::string &group_name, const std::vector<std::string> &instance_names);
}
