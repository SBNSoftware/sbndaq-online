#include "InitMetricManagerService.h"

#include "sbndaq-online/helpers/SBNMetricManager.h"

sbndaq::InitMetricManagerService::InitMetricManagerService(const fhicl::ParameterSet& pset, art::ActivityRegistry& )
{
  sbndaq::InitializeMetricManager(pset.get<fhicl::ParameterSet>("connections"));
}


DEFINE_ART_SERVICE(sbndaq::InitMetricManagerService)
