cet_make_library( 
  LIBRARY_NAME sbndaq_online_redis_connection
  SOURCE RedisConnection.cc
  LIBRARIES
    sbndaq_online_hiredis
    artdaq_utilities::artdaq-utilities_Plugins
    art::Framework_Core
    art::Framework_Principal
    art::Framework_Services_Registry
    art::Framework_Services_System_FileCatalogMetadata_service
    art::Persistency_Common
    art::Persistency_Provenance
    canvas::canvas
    art_root_io::tfile_support
    art_root_io::TFileService_service
    ROOT::Core
    art::Utilities
    messagefacility::MF_MessageLogger
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
)


cet_build_plugin(RedisConnection art::service
  LIBRARIES
  sbndaq_online_redis_connection
)
install_headers()
install_source()


