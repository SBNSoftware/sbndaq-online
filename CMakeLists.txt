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


# use cmake 2.8 or later
cmake_minimum_required (VERSION 3.14)

project(sbndaq-online)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)
list(APPEND CMAKE_MODULE_PATH $ENV{ART_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
#  EXTRA_FLAGS -pedantic
)
cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( art v3_02_05 )
find_ups_product( art_root_io v1_00_05)
find_ups_product( artdaq_utilities  v1_05_02)
find_ups_product( TRACE v3_15_03 )
find_ups_product( cetbuildtools v7_13_02 )


# macros for dictionary and simple_plugin
#include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(sbndaq-online)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
