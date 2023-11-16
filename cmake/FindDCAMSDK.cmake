set(DCAMSDK_INCLUDE_DIRS)
set(DCAMSDK_LIBRARIES)
set(DCAMSDK_DEFINITIONS)

if(WIN32)
  # Missing find_path here
  find_library(DCAMAPI_LIB dcamapi)
  find_library(DCIMGAPI_LIB dcimgapi)
  find_path(DCAMSDK_INCLUDE_DIRS "dcamapi4.h")
  list(APPEND DCAMSDK_LIBRARIES ${DCAMAPI_LIB} ${DCIMGAPI_LIB})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DCAMSDK DEFAULT_MSG
  DCAMSDK_LIBRARIES
  DCAMSDK_INCLUDE_DIRS
)
