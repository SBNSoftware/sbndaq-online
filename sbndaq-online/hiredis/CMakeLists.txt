# make the shared library
cet_make_library( LIBRARY_NAME sbndaq_online_hiredis
        SOURCE
  net.c 
  hiredis.c 
  sds.c 
  async.c 
  read.c
)

# install headers
install_headers()
install_source()



