cet_make_library(LIBRARY_NAME sbndaq_online_storage
  SOURCE
    Histogram.cc Binary.cc 
  LIBRARIES
    sbndaq_online_redis_connection
    sbndaq_online_hiredis
    ${ROOT_BASIC_LIB_LIST}
)

install_source()
install_headers()

