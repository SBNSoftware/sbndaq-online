#ifndef _sbndaq_online_InitMetricManagerService_h_
#define _sbndaq_online_InitMetricManagerService_h_
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

namespace sbndaq {
class InitMetricManagerService {
public:
  InitMetricManagerService(const fhicl::ParameterSet& pset, art::ActivityRegistry& reg);
};
} // end namespace sbndaq

DECLARE_ART_SERVICE(sbndaq::InitMetricManagerService, LEGACY)
#endif
