basic_plugin(redis "metric"
  LIBRARIES
  artdaq_utilities::artdaq-utilities_Plugins
  sbndaq_online_redis_connection
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  cetlib_except::cetlib_except
  messagefacility::MF_MessageLogger
#  ${Boost_SYSTEM_LIBRARY}
#  ${Boost_FILESYSTEM_LIBRARY}
)


