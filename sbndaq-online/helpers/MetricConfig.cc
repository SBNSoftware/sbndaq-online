#include "MetricConfig.h"

#include <string>
#include <vector>
#include <fstream>

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/ParameterSetWalker.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "sbndaq-online/redis-connect/RedisConnectionService.h"
#include "Utilities.h"

#include "json/json.h"
#include "json/writer.h"

void sbndaq::GenerateMetricConfig(const fhicl::ParameterSet &config) {
  // get redis connection from service
  art::ServiceHandle<RedisConnectionService> redis;

  // get all the groups
  fhicl::ParameterSet groups = config.get<fhicl::ParameterSet>("groups", {});
  // get the group names
  std::vector<std::string> group_names = groups.get_names();
  // and the depth of names
  std::vector<std::string> group_keys = groups.get_all_keys();
  // store the instance names
  std::vector<std::vector<std::string>> instance_names(1);
  // get all the instances in each group
  // keep track of which group we are on
  unsigned group_index = 0;
  // start past the first key, which we know is just the name of the group
  for (unsigned i = 1; i < group_keys.size(); i++) {
    const std::string &key = group_keys[i];
    // this key points to the next group
    if (group_index < group_names.size() - 1 && group_names[group_index+1] == key) {
      group_index ++;
      instance_names.push_back({});
    }
    // this key points to a list
    else if (groups.is_key_to_sequence(key)) {
      std::array<int, 2> instance_pair = groups.get<std::array<int, 2>>(key);
      for (int pair_ind = instance_pair[0]; pair_ind < instance_pair[1]; pair_ind++) {
        instance_names[group_index].push_back(std::to_string(pair_ind));
      }
      // jump past the end of the list
      i += 2;
    }
    // this key points to a single item
    else if (groups.is_key_to_atom(key)) {
      std::string instance = groups.get<std::string>(key);
      instance_names[group_index].push_back(instance);
    }
  }

  // make sure names are valid
  for (unsigned i = 0; i < group_names.size(); i++) {
    group_names[i] = sbndaq::ValidateRedisName(group_names[i]);
  }
  for (unsigned i = 0; i < instance_names.size(); i++) {
    for (unsigned j = 0; j < instance_names[i].size(); j++) {
      instance_names[i][j] = sbndaq::ValidateRedisName(instance_names[i][j]);
    }
  }

  // add the groups and their config
  for (unsigned i = 0; i < group_names.size(); i++) {
    AddGroup(redis.get(), group_names[i]);
    AddInstances2Group(redis.get(), group_names[i], instance_names[i]);
  }

  // and set the metric config for each group
  fhicl::ParameterSet metric_config = config.get<fhicl::ParameterSet>("metrics", {});

  // get the metric names and their configs
  std::vector<std::string> metrics = metric_config.get_pset_names();
  std::vector<sbndaq::MetricConfig> metric_configs;
  for (const std::string &metric_name: metrics) {
    fhicl::ParameterSet this_config = metric_config.get<fhicl::ParameterSet>(metric_name);
    // build the metric config object
    sbndaq::MetricConfig redis_metric_config;

    if (this_config.has_key("display_range")) {
      redis_metric_config.display_range = this_config.get<std::array<double, 2>>("display_range");
    }
    else {
      redis_metric_config.display_range = {};
    }

    if (this_config.has_key("warning_range")) {
      redis_metric_config.warning_range = this_config.get<std::array<double, 2>>("warning_range");
    }
    else {
      redis_metric_config.warning_range = {};
    }

    if (this_config.has_key("alarm_range")) {
      redis_metric_config.alarm_range = this_config.get<std::array<double, 2>>("alarm_range");
    }
    else {
      redis_metric_config.alarm_range = {};
    }

    if (this_config.has_key("units")) {
      redis_metric_config.units = this_config.get<std::string>("units");
    }
    else {
      redis_metric_config.units = {};
    }

    if (this_config.has_key("format")) {
      redis_metric_config.format = this_config.get<std::string>("format");
    }
    else {
      redis_metric_config.format = {};
    }

    if (this_config.has_key("title")) {
      redis_metric_config.title = this_config.get<std::string>("title");
    }
    else {
      redis_metric_config.title = {};
    }
    metric_configs.push_back(redis_metric_config);
  }

  // get the list of streams
  std::vector<std::string> streams = config.get<std::vector<std::string>>("streams", {});

  // send the config to each group
  for (const std::string &group_name: group_names) {
    GroupMetricConfig(redis.get(), group_name, metrics, metric_configs, streams);
  }

  // force a flush
  redis->Flush();

  return;
}

void sbndaq::AddInstances2Group(RedisConnection *redis, const std::string &group_name, const std::vector<std::string> &instance_names) {
  redis->Command("DEL GROUP_MEMBERS:%s", group_name.c_str());
  for (unsigned i = 0; i < instance_names.size(); i++) {
    redis->Command("RPUSH GROUP_MEMBERS:%s %s", group_name.c_str(), instance_names[i].c_str());
  }
}

void sbndaq::AddGroup(RedisConnection *redis, const std::string &group_name) {
  redis->Command("SADD GROUPS %s", group_name.c_str());
}

void sbndaq::GroupMetricConfig(RedisConnection *redis, const std::string &group_name, 
    const std::vector<std::string> &metric_names, 
    const std::vector<sbndaq::MetricConfig> &metric_configs,
    const std::vector<std::string> &stream_names) {

  // encode the metric config as a JSON dictionary
  Json::Value root; 
  
  for (unsigned i = 0; i < metric_names.size(); i++) {
    const sbndaq::MetricConfig &config = metric_configs[i];
    const std::string &metric = metric_names[i];

    Json::Value json_config;
    if (config.display_range) {
      json_config["display_range"] = Json::arrayValue;
      json_config["display_range"][0] = config.display_range.value()[0];
      json_config["display_range"][1] = config.display_range.value()[1];
    }
    if (config.warning_range) {
      json_config["warning_range"] = Json::arrayValue;
      json_config["warning_range"][0] = config.warning_range.value()[0];
      json_config["warning_range"][1] = config.warning_range.value()[1];
    }
    if (config.alarm_range) {
      json_config["alarm_range"] = Json::arrayValue;
      json_config["alarm_range"][0] = config.alarm_range.value()[0];
      json_config["alarm_range"][1] = config.alarm_range.value()[1];
    }
    if (config.units) {
      json_config["units"] = config.units.value();
    }
    if (config.format) {
      json_config["format"] = config.format.value();
    }
    if (config.title) {
      json_config["title"] = config.title.value();
    }
    json_config["name"] = metric;
    root["metric_config"][metric] = json_config;
  }
  
  // set up the streams
  root["streams"] = Json::arrayValue;
  for (unsigned i = 0; i < stream_names.size(); i++) {
    root["streams"][i] = stream_names[i];
  }

  // print the JSON config to string
  Json::FastWriter fastWriter;
  std::string json_config_str = fastWriter.write(root);

  // send it out
  redis->Command("SET GROUP_CONFIG:%s %s", group_name.c_str(), json_config_str.c_str());
}
