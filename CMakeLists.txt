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


cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

find_package(cetmodules)
set(CET_CETBUILDTOOLS_COMPAT TRUE)
include(Compatibility)
set(CET_WARN_DEPRECATED)

project(sbndaq_online VERSION 1.01.00 ) #LANGUAGES CXX)

#SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
#IF (NOT CETBUILDTOOLS_VERSION)
#    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
#ENDIF()
#set(CMAKE_MODULE_PATH $ENV{CETBUILDTOOLS_DIR}/Modules
#                      $ENV{ARTDAQ_UTILITIES_DIR}/Modules
#		      ${CMAKE_MODULE_PATH})


set(CMAKE_MODULE_PATH
  $ENV{ARTDAQ_UTILITIES_DIR}/Modules
  ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
#  EXTRA_FLAGS -pedantic
)
cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions

find_ups_product( artdaq_utilities v1_00_00 )
find_ups_boost(v1_53_0)
find_ups_product (cetbuildtools v4_09_02)
find_ups_product( TRACE v3_03_00 )
find_ups_product( cetlib v1_08_00 )
find_ups_product( cetpkgsupport v1_08_03 )
find_ups_product( fhiclcpp v3_04_00 )
find_ups_product( messagefacility v1_14_01 )
find_ups_product( art )
find_ups_root()

#find_package( art)


#find_ups_product( art_root_io )
#find_ups_product( artdaq_utilities )
#find_ups_product( TRACE )
#find_ups_boost()
#find_ups_product( cetbuildtools v7_13_02 )

link_directories("$ENV{ARTDAQ_UTILITIES_LIB}")

# macros for dictionary and simple_plugin
#include(ArtDictionary)
#include(BasicPlugin)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(sbndaq-online)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
