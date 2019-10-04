cet_make_library( 
  LIBRARY_NAME sbndaq_online_redis_connection
  SOURCE RedisConnection.cc
  LIBRARIES
    sbndaq_online_hiredis
    ${ART_FRAMEWORK_CORE}
    ${ART_FRAMEWORK_PRINCIPAL}
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
    art_Persistency_Common canvas
    art_Persistency_Provenance canvas
    ${ART_FRAMEWORK_SERVICES_BASIC}
    ${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
    ${ART_ROOT_IO_TFILESERVICE_SERVICE}
    ${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}
    art_Utilities canvas
    ${MF_MESSAGELOGGER}
    fhiclcpp 
    cetlib cetlib_except
    ${ROOT_BASIC_LIB_LIST}
)


simple_plugin(RedisConnection "service"
  sbndaq_online_redis_connection
)
install_headers()
install_source()


