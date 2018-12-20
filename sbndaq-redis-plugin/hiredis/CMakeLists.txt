# make the shared library
add_library(sbndaq_redis_plugin_hiredis SHARED
  net.c 
  hiredis.c 
  sds.c 
  async.c 
  read.c
)

# install headers
install_headers()
install_source()



