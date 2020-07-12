IF (APPLE)
 SET(hid_SOURCES hid_macos.cc)
 SET(hid_HEADERS hid_macos.hh)
ELSE (APPLE)
 SET(hid_SOURCES hid_libusb.cc)
 SET(hid_HEADERS hid_libusb.hh)
ENDIF(APPLE)

SET(libdmrconf_SOURCES utils.cc crc32.cc csvwriter.cc
   radio.cc radiointerface.cc ${hid_SOURCES} hid_interface.cc dfu_libusb.cc usbserial.cc
   csvreader.cc dfufile.cc repeaterdatabase.cc userdatabase.cc logger.cc
   config.cc contact.cc rxgrouplist.cc channel.cc zone.cc scanlist.cc gpssystem.cc codeplug.cc
   rd5r.cc rd5r_codeplug.cc uv390.cc uv390_codeplug.cc gd77.cc gd77_codeplug.cc
   opengd77.cc opengd77_interface.cc opengd77_codeplug.cc)
SET(libdmrconf_MOC_HEADERS
   radio.hh radiointerface.hh ${hid_HEADERS} hid_interface.hh dfu_libusb.hh usbserial.hh
   csvreader.hh dfufile.hh repeaterdatabase.hh userdatabase.hh logger.hh
   config.hh contact.hh rxgrouplist.hh channel.hh zone.hh scanlist.hh gpssystem.hh codeplug.hh
   rd5r.hh rd5r_codeplug.hh uv390.hh uv390_codeplug.hh gd77.hh gd77_codeplug.hh
   opengd77.hh opengd77_interface.hh opengd77_codeplug.hh)
SET(libdmrconf_HEADERS libdmrconf.hh utils.hh crc32.hh csvwriter.hh)

configure_file(config.h.in ${PROJECT_BINARY_DIR}/lib/config.h)


qt5_wrap_cpp(libdmrconf_MOC_SOURCES ${libdmrconf_MOC_HEADERS})

add_library(libdmrconf SHARED ${libdmrconf_SOURCES} ${libdmrconf_MOC_SOURCES})
set_target_properties(libdmrconf PROPERTIES
  OUTPUT_NAME dmrconf
  VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
  SOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
target_link_libraries(libdmrconf ${LIBS})

install(TARGETS libdmrconf DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${libdmrconf_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)
install(FILES ${libdmrconf_MOC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)
install(FILES ${PROJECT_BINARY_DIR}/lib/config.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)

