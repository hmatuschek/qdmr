IF (APPLE)
 SET(hid_SOURCES hid_macos.cc)
 SET(hid_HEADERS hid_macos.hh)
 #SET(hid_SOURCES hid_libusb.cc)
 #SET(hid_HEADERS hid_libusb.hh)
ELSE (APPLE)
 SET(hid_SOURCES hid_libusb.cc)
 SET(hid_HEADERS hid_libusb.hh)
ENDIF(APPLE)

SET(libdmrconf_SOURCES
    utils.cc crc32.cc addressmap.cc radiointerface.cc errorstack.cc frequency.cc interval.cc
    ranges.cc dummyfilereader.cc chirpformat.cc
    signaling.cc
    radio.cc ${hid_SOURCES} dfu_libusb.cc usbserial.cc radioinfo.cc usbdevice.cc radiolimits.cc
    csvreader.cc dfufile.cc userdatabase.cc logger.cc
    melody.cc
    visitor.cc configlabelingvisitor.cc configcopyvisitor.cc intermediaterepresentation.cc
    configmergevisitor.cc
    configobject.cc configreference.cc config.cc radiosettings.cc contact.cc rxgrouplist.cc
    channel.cc zone.cc scanlist.cc gpssystem.cc codeplug.cc roamingzone.cc roamingchannel.cc
    callsigndb.cc talkgroupdatabase.cc radioid.cc encryptionextension.cc commercial_extension.cc
    smsextension.cc satellitedatabase.cc orbitalelementsdatabase.cc transponderdatabase.cc
    satelliteconfig.cc
    tyt_radio.cc tyt_interface.cc tyt_codeplug.cc tyt_callsigndb.cc tyt_extensions.cc
    md2017.cc md2017_codeplug.cc md2017_callsigndb.cc md2017_filereader.cc md2017_limits.cc
    md390.cc md390_codeplug.cc md390_filereader.cc md390_limits.cc
    uv390.cc uv390_codeplug.cc uv390_callsigndb.cc uv390_filereader.cc uv390_limits.cc
    dm1701.cc dm1701_codeplug.cc dm1701_callsigndb.cc dm1701_filereader.cc dm1701_limits.cc
    radioddity_radio.cc radioddity_codeplug.cc radioddity_extensions.cc radioddity_interface.cc
    rd5r.cc rd5r_codeplug.cc rd5r_filereader.cc rd5r_limits.cc
    gd77.cc gd77_codeplug.cc gd77_callsigndb.cc gd77_filereader.cc gd77_limits.cc
    opengd77_interface.cc opengd77base.cc opengd77base_codeplug.cc opengd77base_callsigndb.cc
    opengd77base_satelliteconfig.cc opengd77_extension.cc opengd77_limits.cc
    opengd77.cc opengd77_codeplug.cc opengd77_callsigndb.cc opengd77_satelliteconfig.cc
    openuv380.cc openuv380_codeplug.cc openuv380_callsigndb.cc openuv380_satelliteconfig.cc
    openrtx.cc openrtx_interface.cc openrtx_codeplug.cc
    c7000device.cc gd73.cc gd73_interface.cc gd73_codeplug.cc gd73_filereader.cc gd73_limits.cc
    anytone_interface.cc anytone_radio.cc anytone_codeplug.cc anytone_extension.cc anytone_limits.cc
    anytone_satelliteconfig.cc
    d868uv.cc d868uv_codeplug.cc d868uv_callsigndb.cc d868uv_limits.cc
    d878uv.cc d878uv_codeplug.cc d878uv_limits.cc
    d578uv.cc d578uv_codeplug.cc d578uv_limits.cc
    d878uv2.cc d878uv2_codeplug.cc d878uv2_limits.cc d878uv2_callsigndb.cc
    dmr6x2uv.cc dmr6x2uv_codeplug.cc dmr6x2uv_limits.cc
    auctus_a6_interface.cc
    dr1801uv.cc dr1801uv_interface.cc dr1801uv_codeplug.cc dr1801uv_filereader.cc dr1801uv_limits.cc)

SET(libdmrconf_MOC_HEADERS
    radio.hh ${hid_HEADERS} dfu_libusb.hh usbserial.hh radiolimits.hh
    csvreader.hh dfufile.hh userdatabase.hh logger.hh
    melody.hh
    configobject.hh configreference.hh config.hh radiosettings.hh contact.hh rxgrouplist.hh
    channel.hh zone.hh scanlist.hh gpssystem.hh codeplug.hh roamingzone.hh roamingchannel.hh
    callsigndb.hh talkgroupdatabase.hh radioid.hh encryptionextension.hh commercial_extension.hh
    smsextension.hh satellitedatabase.hh orbitalelementsdatabase.hh transponderdatabase.hh
    satelliteconfig.hh
    tyt_radio.hh tyt_interface.hh tyt_codeplug.hh tyt_callsigndb.hh tyt_extensions.hh
    md2017.hh md2017_codeplug.hh md2017_callsigndb.hh md2017_limits.hh
    md390.hh md390_codeplug.hh md390_limits.hh
    uv390.hh uv390_codeplug.hh uv390_callsigndb.hh uv390_limits.hh
    dm1701.hh dm1701_codeplug.hh dm1701_callsigndb.hh dm1701_filereader.hh dm1701_limits.hh
    radioddity_radio.hh radioddity_codeplug.hh radioddity_extensions.hh radioddity_interface.hh
    rd5r.hh rd5r_codeplug.hh rd5r_limits.hh
    gd77.hh gd77_codeplug.hh gd77_callsigndb.hh gd77_limits.hh
    opengd77_interface.hh opengd77base.hh opengd77base_codeplug.hh opengd77base_callsigndb.hh
    opengd77base_satelliteconfig.hh opengd77_extension.hh opengd77_limits.hh
    opengd77.hh opengd77_codeplug.hh opengd77_callsigndb.hh opengd77_satelliteconfig.hh
    openuv380.hh openuv380_codeplug.hh openuv380_callsigndb.hh openuv380_satelliteconfig.hh
    c7000device.hh gd73.hh gd73_interface.hh gd73_codeplug.hh gd73_limits.hh
    openrtx.hh openrtx_interface.hh openrtx_codeplug.hh
    anytone_interface.hh anytone_radio.hh anytone_codeplug.hh anytone_extension.hh anytone_limits.hh
    anytone_satelliteconfig.hh
    d868uv.hh d868uv_codeplug.hh d868uv_callsigndb.hh d868uv_limits.hh
    d878uv.hh d878uv_codeplug.hh d878uv_limits.hh
    d578uv.hh d578uv_codeplug.hh d578uv_limits.hh
    d878uv2.hh d878uv2_codeplug.hh d878uv2_limits.hh d878uv2_callsigndb.hh
    dmr6x2uv.hh dmr6x2uv_codeplug.hh dmr6x2uv_limits.hh
    auctus_a6_interface.hh
    dr1801uv.hh dr1801uv_interface.hh dr1801uv_codeplug.hh dr1801uv_limits.hh)

SET(libdmrconf_HEADERS libdmrconf.hh radiointerface.hh radioinfo.hh usbdevice.hh signaling.hh
    gd77_filereader.hh rd5r_filereader.hh uv390_filereader.hh md2017_filereader.hh gd73_filereader.hh
    md390_filereader.hh dr1801uv_filereader.hh dummyfilereader.hh
    utils.hh crc32.hh signaling.hh addressmap.hh errorstack.hh frequency.hh interval.hh ranges.hh
    chirpformat.hh
    visitor.hh configlabelingvisitor.hh configcopyvisitor.hh intermediaterepresentation.hh
    configmergevisitor.hh)



configure_file(config.h.in ${PROJECT_BINARY_DIR}/lib/config.h)

#SET(CMAKE_AUTOMOC ON)
qt5_wrap_cpp(libdmrconf_MOC_SOURCES ${libdmrconf_MOC_HEADERS})

add_library(libdmrconf SHARED ${libdmrconf_SOURCES} ${libdmrconf_MOC_SOURCES})
set_target_properties(libdmrconf PROPERTIES
	MACOSX_RPATH TRUE
  OUTPUT_NAME dmrconf
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}")
target_link_libraries(libdmrconf ${CORE_LIBS})

install(TARGETS libdmrconf DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
install(FILES ${libdmrconf_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)
install(FILES ${libdmrconf_MOC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)
install(FILES ${PROJECT_BINARY_DIR}/lib/config.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)

if (UNIX AND NOT APPLE AND ${INSTALL_UDEV_RULES})
  install(FILES "${CMAKE_SOURCE_DIR}/dist/99-qdmr.rules" DESTINATION ${INSTALL_UDEV_PATH})
endif(UNIX AND NOT APPLE AND ${INSTALL_UDEV_RULES})

