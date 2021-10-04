# - Try to find yaml-cpp
# Once done this will define
#
# YAMLCPP_FOUND - system has yaml-cpp
# YAMLCPP_INCLUDE_DIRS - the yaml-cpp include directory
# YAMLCPP_LIBRARIES - Link these to use yaml-cpp
# YAMLCPP_DEFINITIONS - Compiler switches required for using yaml-cpp

if (YAMLCPP_LIBRARIES AND YAMLCPP_INCLUDE_DIRS)
  # in cache already
  set(YAMLCPP_FOUND TRUE)
else (YAMLCPP_LIBRARIES AND YAMLCPP_INCLUDE_DIRS)
  find_path(YAMLCPP_INCLUDE_DIR
    NAMES
    yaml.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local
      /sw/include
    PATH_SUFFIXES
      yaml-cpp
  )

  find_library(YAMLCPP_LIBRARY
    NAMES
      yaml-cpp
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local
      /sw/lib
  )

  set(YAMLCPP_INCLUDE_DIRS
    ${YAMLCPP_INCLUDE_DIR}
  )
  set(YAMLCPP_LIBRARIES
    ${YAMLCPP_LIBRARY}
)

  if (YAMLCPP_INCLUDE_DIRS AND YAMLCPP_LIBRARIES)
     set(YAMLCPP_FOUND TRUE)
  endif (YAMLCPP_INCLUDE_DIRS AND YAMLCPP_LIBRARIES)

  if (YAMLCPP_FOUND)
    if (NOT YAMLCPP_FIND_QUIETLY)
      message(STATUS "Found yalm-cpp:")
      message(STATUS " - Includes: ${YAMLCPP_INCLUDE_DIRS}")
      message(STATUS " - Libraries: ${YAMLCPP_LIBRARIES}")
    endif (NOT YAMLCPP_FIND_QUIETLY)
  else (YAMLCPP_FOUND)
    if (YAMLCPP_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find yaml-cpp")
    endif (YAMLCPP_FIND_REQUIRED)
  endif (YAMLCPP_FOUND)

  # show the YAMLCPP_INCLUDE_DIRS and YAMLCPP_LIBRARIES variables only in the advanced view
  mark_as_advanced(YAMLCPP_INCLUDE_DIRS YAMLCPP_LIBRARIES)
endif (YAMLCPP_LIBRARIES AND YAMLCPP_INCLUDE_DIRS)
