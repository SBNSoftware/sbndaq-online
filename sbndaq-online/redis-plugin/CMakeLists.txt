basic_plugin(redis "metric"
  artdaq_utilities::artdaq-utilities_Plugins
  sbndaq_online_redis_connection
  fhiclcpp
  cetlib
  cetlib_except
  MF_MessageLogger
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
)


