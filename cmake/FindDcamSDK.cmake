set(DCAMSDK_INCLUDE_DIRS)
set(DCAMSDK_LIBRARIES)
set(DCAMSDK_DEFINITIONS)

if(WIN32)
  # Missing find_path here
  find_library(DCAMSDK_LIBRARIES dcamapi)
  find_path(DCAMSDK_INCLUDE_DIRS "dcamapi.h")
else()
  find_path(DCAMSDK_INCLUDE_DIRS "dcamapi.h")
  find_library(DCAMSDK_LIBRARIES dcamapi)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Andor DEFAULT_MSG
  DCAMSDK_LIBRARIES
  DCAMSDK_INCLUDE_DIRS
)
