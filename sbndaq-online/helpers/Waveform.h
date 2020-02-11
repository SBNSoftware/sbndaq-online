#ifndef _sbndaq_online_Waveform_h_
#define _sbndaq_online_Waveform_h_

#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include "sbndaq-online/DisplayTypes.h"
#include "sbndaq-online/redis-connect/RedisConnectionService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

namespace sbndaq {

// Public-facing functions

// Send a single waveform to a database
template<typename DataType>
void SendWaveform(
    const std::string &key,
    const std::vector<DataType> &waveform,
    float tick_period=1.);

// Send a number of waveforms (e.g. an OpDetWaveform) to a databse
template<typename DataType, typename OffsetType>
void SendSplitWaveform(
    const std::string &key,
    const std::vector<std::vector<DataType>> &waveforms,
    const std::vector<OffsetType> &offsets,
    float tick_period=1.);

template<typename DataType, typename OffsetType>
void SendSplitWaveform(
    const std::string &key, 
    const std::vector<std::vector<DataType>> &waveforms, 
    const std::vector<OffsetType> &offsets, 
    float tick_period) {
  
  art::ServiceHandle<RedisConnectionService> redis;

  // set the type-names
  redis->Command("HMSET %s DataType %s", key.c_str(), DisplayType<DataType>::name); 
  redis->Command("HMSET %s OffsetType %s", key.c_str(), DisplayType<OffsetType>::name);
  redis->Command("HMSET %s SizeType %s", key.c_str(), DisplayType<uint32_t>::name);

  // set the period size
  redis->Command("HMSET %s TickPeriod %f", key.c_str(), tick_period);

  // mash the waveforms together into a single command
  // copy the waveforms into a single vector
  std::vector<DataType> all_waveforms;
  for (const std::vector<DataType> &wf: waveforms) {
    all_waveforms.insert(all_waveforms.end(), wf.begin(), wf.end());
  }
  redis->Command("HMSET %s Data %b", key.c_str(), &all_waveforms[0], all_waveforms.size() * sizeof(DataType));

  // Save the offsets
  redis->Command("HMSET %s Offsets %b", key.c_str(), &offsets[0], offsets.size() * sizeof(OffsetType));

  // Save the sizes of the waveforms
  std::vector<uint32_t> waveform_sizes;
  for (const std::vector<DataType> &wf: waveforms) {
    waveform_sizes.push_back(wf.size());
  }
  redis->Command("HMSET %s Sizes %b", key.c_str(), &waveform_sizes[0], waveform_sizes.size() * sizeof(uint32_t));
} 

template <typename DataType>
void SendWaveform(
    const std::string &key, 
    const std::vector<DataType> &waveform,
    float tick_period) {

  art::ServiceHandle<RedisConnectionService> redis;
  
  // set the type-names
  redis->Command("HMSET %s DataType %s", key.c_str(), DisplayType<DataType>::name); 

  // save the waveforms
  redis->Command("HMSET %s Data %b", key.c_str(), &waveform[0], waveform.size() * sizeof(DataType));

  // set the period size
  redis->Command("HMSET %s TickPeriod %f", key.c_str(), tick_period);

} 
} // end namespace sbndaq

#endif
