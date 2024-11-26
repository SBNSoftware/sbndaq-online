# ======================================================================
#  sbndaq-online main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/sbndaq-online/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


cmake_minimum_required (VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)

set(CET_CETBUILDTOOLS_COMPAT TRUE)
include(Compatibility)
set(CET_WARN_DEPRECATED)

project(sbndaq_online VERSION 1.01.00 ) #LANGUAGES CXX)

set(CMAKE_MODULE_PATH
  $ENV{ARTDAQ_UTILITIES_DIR}/Modules
  ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -Wno-pedantic
)
cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions

find_package( artdaq_utilities REQUIRED) 
find_package( Boost COMPONENTS system filesystem REQUIRED )
find_package( jsoncpp REQUIRED )
find_package( TRACE REQUIRED )
find_package( cetlib REQUIRED )
#find_package( cetpkgsupport v1_08_03 )
find_package( fhiclcpp REQUIRED )
find_package( messagefacility REQUIRED )
find_package( art REQUIRED )
find_package( art_root_io REQUIRED )
find_package( ROOT REQUIRED )

#link_directories("$ENV{ARTDAQ_UTILITIES_LIB}")

# macros for dictionary and simple_plugin
#include(BasicPlugin)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(sbndaq-online)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
#cet_cmake_config()
