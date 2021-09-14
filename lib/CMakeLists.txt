IF (APPLE)
 SET(hid_SOURCES hid_macos.cc)
 SET(hid_HEADERS hid_macos.hh)
ELSE (APPLE)
 SET(hid_SOURCES hid_libusb.cc)
 SET(hid_HEADERS hid_libusb.hh)
ENDIF(APPLE)

SET(libdmrconf_SOURCES
    utils.cc crc32.cc csvwriter.cc signaling.cc codeplugcontext.cc addressmap.cc radiointerface.cc
    radio.cc ${hid_SOURCES} dfu_libusb.cc usbserial.cc
    configreader.cc csvreader.cc dfufile.cc repeaterdatabase.cc userdatabase.cc logger.cc
    configobject.cc configreference.cc config.cc contact.cc rxgrouplist.cc channel.cc zone.cc
    scanlist.cc gpssystem.cc codeplug.cc roaming.cc callsigndb.cc talkgroupdatabase.cc radioid.cc
    tyt_radio.cc tyt_codeplug.cc tyt_callsigndb.cc tyt_extensions.cc
    md2017.cc md2017_codeplug.cc md2017_callsigndb.cc md2017_filereader.cc
    uv390.cc uv390_codeplug.cc uv390_callsigndb.cc uv390_filereader.cc
    radioddity_radio.cc radioddity_codeplug.cc radioddity_interface.cc
    rd5r.cc rd5r_codeplug.cc rd5r_filereader.cc
    gd77.cc gd77_codeplug.cc gd77_callsigndb.cc gd77_filereader.cc
    opengd77.cc opengd77_interface.cc opengd77_codeplug.cc opengd77_callsigndb.cc
    anytone_interface.cc anytone_radio.cc anytone_codeplug.cc
    d868uv.cc d868uv_codeplug.cc d868uv_callsigndb.cc
    d878uv.cc d878uv_codeplug.cc
    d578uv.cc d578uv_codeplug.cc
    d878uv2.cc d878uv2_codeplug.cc d878uv2_callsigndb.cc)
SET(libdmrconf_MOC_HEADERS
    radio.hh ${hid_HEADERS} dfu_libusb.hh usbserial.hh
    configreader.hh csvreader.hh dfufile.hh repeaterdatabase.hh userdatabase.hh logger.hh
    configobject.hh configreference.hh config.hh contact.hh rxgrouplist.hh channel.hh zone.hh
    scanlist.hh gpssystem.hh codeplug.hh roaming.hh callsigndb.hh talkgroupdatabase.hh radioid.hh
    tyt_radio.hh tyt_codeplug.hh tyt_callsigndb.hh tyt_extensions.hh
    md2017.hh md2017_codeplug.hh md2017_callsigndb.hh
    uv390.hh uv390_codeplug.hh uv390_callsigndb.hh
    radioddity_radio.hh radioddity_codeplug.hh radioddity_interface.hh
    rd5r.hh rd5r_codeplug.hh
    gd77.hh gd77_codeplug.hh gd77_callsigndb.hh
    opengd77.hh opengd77_interface.hh opengd77_codeplug.hh opengd77_callsigndb.hh
    anytone_interface.hh anytone_radio.hh anytone_codeplug.hh
    d868uv.hh d868uv_codeplug.hh d868uv_callsigndb.hh
    d878uv.hh d878uv_codeplug.hh
    d578uv.hh d578uv_codeplug.hh
    d878uv2.hh d878uv2_codeplug.hh d878uv2_callsigndb.hh)
SET(libdmrconf_HEADERS libdmrconf.hh radiointerface.hh
    gd77_filereader.hh rd5r_filereader.hh uv390_filereader.hh md2017_filereader.hh
    utils.hh crc32.hh csvwriter.hh signaling.hh codeplugcontext.hh addressmap.hh)

configure_file(config.h.in ${PROJECT_BINARY_DIR}/lib/config.h)

#SET(CMAKE_AUTOMOC ON)
qt5_wrap_cpp(libdmrconf_MOC_SOURCES ${libdmrconf_MOC_HEADERS})

add_library(libdmrconf SHARED ${libdmrconf_SOURCES} ${libdmrconf_MOC_SOURCES})
set_target_properties(libdmrconf PROPERTIES
	MACOSX_RPATH TRUE
  OUTPUT_NAME dmrconf
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
target_link_libraries(libdmrconf ${LIBS})

install(TARGETS libdmrconf DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${libdmrconf_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)
install(FILES ${libdmrconf_MOC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)
install(FILES ${PROJECT_BINARY_DIR}/lib/config.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)

if (UNIX AND NOT APPLE)
  install(FILES "${CMAKE_SOURCE_DIR}/dist/99-qdmr.rules" DESTINATION "/etc/udev/rules.d/")
endif(UNIX AND NOT APPLE)

