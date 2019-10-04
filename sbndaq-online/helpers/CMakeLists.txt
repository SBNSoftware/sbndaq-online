cet_make_library(LIBRARY_NAME sbndaq_online_storage
  SOURCE
    Histogram.cc Binary.cc Waveform.cc
  LIBRARIES
    sbndaq_online_hiredis
    ${ROOT_BASIC_LIB_LIST}
)

install_source()
install_headers()

