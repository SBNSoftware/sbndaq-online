include_directories($ENV{JSONCPP_INC})

find_library(JSONCPP jsoncpp HINTS $ENV{JSONCPP_LIB})

cet_make_library(LIBRARY_NAME sbndaq_online_storage
  SOURCE
    Histogram.cc Binary.cc MetricConfig.cc EventMeta.cc Alarm.cc
  LIBRARIES
    sbndaq_online_redis_connection
    sbndaq_online_hiredis
    ${ROOT_BASIC_LIB_LIST}
    JSONCPP
)

simple_plugin(InitMetricManager "service"
  sbndaq_online_storage
)

install_source()
install_headers()

