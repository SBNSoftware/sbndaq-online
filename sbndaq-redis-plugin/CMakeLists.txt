add_subdirectory(hiredis)




basic_plugin(redis "metric"
  artdaq-utilities_Plugins
  sbndaq_redis_plugin_hiredis
  ${FHICLCPP}
  ${CETLIB}
  ${CETLIB_EXCEPT}
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
14	
)

install_headers()
install_source()
install_fhicl()
