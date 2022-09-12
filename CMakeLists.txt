cmake_minimum_required(VERSION 3.0.0)
project(qdmr VERSION 0.11.0)

set(RELEASE_SUFFIX "")

option(BUILD_TESTS "Build test programs" OFF)
option(BUILD_DOCS  "Build API documentation" OFF)
option(BUILD_MAN   "Build man page for dmrconf" OFF)
option(INSTALL_UDEV_RULES "Install udev rules file." ON)
option(INSTALL_BUNDLE "Installs QDMR as an AppBundle under MacOS X" OFF)
option(BUNDLE_PATH "Where to install the MacOS X application bundle." "~/Applications")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5UiTools REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Positioning REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(LIBUSB_1 REQUIRED)
find_package(YAMLCPP REQUIRED)

if (${BUILD_MAN})
  find_program(XSLTPROC_EXECUTABLE xsltproc DOC "xsltproc for man-page generation." REQUIRED)
endif(${BUILD_MAN})

if (${BUILD_DOCS})
  find_package(Doxygen REQUIRED dot)
endif(${BUILD_DOCS})

if (${BUILD_TESTS})
  find_package(Qt5Test REQUIRED)
endif(${BUILD_TESTS})

ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
#add_definitions("-DQT_EVENT_DISPATCHER_CORE_FOUNDATION=1")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5UiTools_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Network_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5SerialPort_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Positioning_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Test_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LIBUSB_1_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${YAMLCPP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/src)

set(CORE_LIBS ${Qt5Core_LIBRARIES} ${Qt5Core_QTMAIN_LIBRARIES} ${Qt5Network_LIBRARIES}
  ${Qt5Positioning_LIBRARIES} ${Qt5SerialPort_LIBRARIES} ${LIBUSB_1_LIBRARIES}
  ${YAMLCPP_LIBRARIES})
set(LIBS ${CORE_LIBS} ${Qt5Widgets_LIBRARIES} ${Qt5UiTools_LIBRARIES})

IF (UNIX AND APPLE)
 SET(CORE_LIBS ${CORE_LIBS} "-framework IOKit -framework CoreFoundation")
ENDIF(UNIX AND APPLE)

set(PROJECT_VERSION_STRING "\"${PROJECT_VERSION}\"")
message(STATUS "Build version ${PROJECT_VERSION}")

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} -Wall -Wsign-compare")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}  -O0 -ggdb -fstack-protector -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")


# Get default install directories under Linux
IF(UNIX AND NOT APPLE)
 INCLUDE(GNUInstallDirs)
ENDIF(UNIX AND NOT APPLE)
IF(UNIX AND APPLE)
 SET(CMAKE_INSTALL_LIBDIR "lib")
 SET(CMAKE_INSTALL_FULL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
 SET(CMAKE_INSTALL_INCLUDEDIR "include")
 SET(CMAKE_INSTALL_FULL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
 # Set RPATH under MacOS
 SET(CMAKE_SKIP_RPATH FALSE)
 SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
 SET(CMAKE_SKIP_INSTALL_RPATH FALSE)
 SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
 SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
 SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
 message(STATUS "RPATH: ${CMAKE_INSTALL_RPATH}")
ENDIF(UNIX AND APPLE)

# Sources...
add_subdirectory(lib)
add_subdirectory(cli)
add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(dist)

if(BUILD_TESTS)
 enable_testing()
 add_subdirectory(test)
endif(BUILD_TESTS)

# Source distribution packages:
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}${RELEASE_SUFFIX}")
set(CPACK_SOURCE_IGNORE_FILES
  "/build/;/doc/html;/doc/latex;/doc/reveng;/doc/dmr-intro;/examples/*.dfu;/.git/;~$;.qm;*.user$;${CPACK_SOURCE_IGNORE_FILES}")

include(CPack)
