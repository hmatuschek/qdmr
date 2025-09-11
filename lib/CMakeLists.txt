IF (APPLE)
 SET(hid_SOURCES hid_macos.cc hid_macos.hh)
ELSE (APPLE)
 SET(hid_SOURCES hid_libusb.cc hid_libusb.hh)
ENDIF(APPLE)

configure_file(config.h.in ${PROJECT_BINARY_DIR}/lib/config.h)

qt_add_library(libdmrconf SHARED
  libdmrconf.hh
  ${hid_SOURCES}
  ${PROJECT_BINARY_DIR}/lib/config.h
  utils.cc utils.hh
  crc32.cc crc32.hh
  addressmap.cc addressmap.hh
  radiointerface.cc radiointerface.hh
  errorstack.cc errorstack.hh
  frequency.cc frequency.hh
  interval.cc interval.hh
  ranges.cc ranges.hh
  dummyfilereader.cc dummyfilereader.hh
  chirpformat.cc chirpformat.hh
  signaling.cc signaling.hh
  radio.cc radio.hh
  dfu_libusb.cc dfu_libusb.hh
  usbserial.cc usbserial.hh
  radioinfo.cc radioinfo.hh
  usbdevice.cc usbdevice.hh
  radiolimits.cc radiolimits.hh
  csvreader.cc csvreader.hh
  dfufile.cc dfufile.hh
  userdatabase.cc userdatabase.hh
  logger.cc logger.hh
  melody.cc melody.hh
  visitor.cc visitor.hh
  configlabelingvisitor.cc configlabelingvisitor.hh
  configcopyvisitor.cc configcopyvisitor.hh
  intermediaterepresentation.cc intermediaterepresentation.hh
  configmergevisitor.cc configmergevisitor.hh
  configobject.cc configobject.hh
  configreference.cc configreference.hh
  config.cc config.hh
  radiosettings.cc radiosettings.hh
  contact.cc contact.hh
  rxgrouplist.cc rxgrouplist.hh
  channel.cc channel.hh
  zone.cc zone.hh
  scanlist.cc scanlist.hh
  gpssystem.cc gpssystem.hh
  codeplug.cc codeplug.hh
  roamingzone.cc roamingzone.hh
  roamingchannel.cc roamingchannel.hh
  callsigndb.cc callsigndb.hh
  talkgroupdatabase.cc talkgroupdatabase.hh
  radioid.cc radioid.hh
  encryptionextension.cc encryptionextension.hh
  commercial_extension.cc commercial_extension.hh
  smsextension.cc smsextension.hh
  satellitedatabase.cc satellitedatabase.hh
  orbitalelementsdatabase.cc orbitalelementsdatabase.hh
  transponderdatabase.cc transponderdatabase.hh
  satelliteconfig.cc satelliteconfig.hh
  tyt_radio.cc tyt_radio.hh
  tyt_interface.cc tyt_interface.hh
  tyt_codeplug.cc tyt_codeplug.hh
  tyt_callsigndb.cc tyt_callsigndb.hh
  tyt_extensions.cc tyt_extensions.hh
  md2017.cc md2017.hh
  md2017_codeplug.cc md2017_codeplug.hh
  md2017_callsigndb.cc md2017_callsigndb.hh
  md2017_filereader.cc md2017_filereader.hh
  md2017_limits.cc md2017_limits.hh
  md390.cc md390.hh
  md390_codeplug.cc md390_codeplug.hh
  md390_filereader.cc md390_filereader.hh
  md390_limits.cc md390_limits.hh
  uv390.cc uv390.hh
  uv390_codeplug.cc uv390_codeplug.hh
  uv390_callsigndb.cc uv390_callsigndb.hh
  uv390_filereader.cc uv390_filereader.hh
  uv390_limits.cc uv390_limits.hh
  dm1701.cc dm1701.hh
  dm1701_codeplug.cc dm1701_codeplug.hh
  dm1701_callsigndb.cc dm1701_callsigndb.hh
  dm1701_filereader.cc dm1701_filereader.hh
  dm1701_limits.cc dm1701_limits.hh
  radioddity_radio.cc radioddity_radio.hh
  radioddity_codeplug.cc radioddity_codeplug.hh
  radioddity_extensions.cc radioddity_extensions.hh
  radioddity_interface.cc radioddity_interface.hh
  rd5r.cc rd5r.hh
  rd5r_codeplug.cc rd5r_codeplug.hh
  rd5r_filereader.cc rd5r_filereader.hh
  rd5r_limits.cc rd5r_limits.hh
  gd77.cc gd77.hh
  gd77_codeplug.cc gd77_codeplug.hh
  gd77_callsigndb.cc gd77_callsigndb.hh
  gd77_filereader.cc gd77_filereader.hh
  gd77_limits.cc gd77_limits.hh
  opengd77_interface.cc opengd77_interface.hh
  opengd77base.cc opengd77base.hh
  opengd77base_codeplug.cc opengd77base_codeplug.hh
  opengd77base_callsigndb.cc opengd77base_callsigndb.hh
  opengd77base_satelliteconfig.cc opengd77base_satelliteconfig.hh
  opengd77_extension.cc opengd77_extension.hh
  opengd77_limits.cc opengd77_limits.hh
  opengd77.cc opengd77.hh
  opengd77_codeplug.cc opengd77_codeplug.hh
  opengd77_callsigndb.cc opengd77_callsigndb.hh
  opengd77_satelliteconfig.cc opengd77_satelliteconfig.hh
  openuv380.cc openuv380.hh
  openuv380_codeplug.cc openuv380_codeplug.hh
  openuv380_callsigndb.cc openuv380_callsigndb.hh
  openuv380_satelliteconfig.cc openuv380_satelliteconfig.hh
  openrtx.cc openrtx.hh
  openrtx_interface.cc openrtx_interface.hh
  openrtx_codeplug.cc openrtx_codeplug.hh
  c7000device.cc c7000device.hh
  gd73.cc gd73.hh
  gd73_interface.cc gd73_interface.hh
  gd73_codeplug.cc gd73_codeplug.hh
  gd73_filereader.cc gd73_filereader.hh
  gd73_limits.cc gd73_limits.hh
  anytone_interface.cc anytone_interface.hh
  anytone_radio.cc anytone_radio.hh
  anytone_codeplug.cc anytone_codeplug.hh
  anytone_extension.cc anytone_extension.hh
  anytone_limits.cc anytone_limits.hh
  anytone_satelliteconfig.cc anytone_satelliteconfig.hh
  d868uv.cc d868uv.hh
  d868uv_codeplug.cc d868uv_codeplug.hh
  d868uv_callsigndb.cc d868uv_callsigndb.hh
  d868uv_limits.cc d868uv_limits.hh
  d878uv.cc d878uv.hh
  d878uv_codeplug.cc d878uv_codeplug.hh
  d878uv_limits.cc d878uv_limits.hh
  d578uv.cc d578uv.hh
  d578uv_codeplug.cc d578uv_codeplug.hh
  d578uv_limits.cc d578uv_limits.hh
  d878uv2.cc d878uv2.hh
  d878uv2_codeplug.cc d878uv2_codeplug.hh
  d878uv2_limits.cc d878uv2_limits.hh
  d878uv2_callsigndb.cc d878uv2_callsigndb.hh
  dmr6x2uv.cc dmr6x2uv.hh
  dmr6x2uv_codeplug.cc dmr6x2uv_codeplug.hh
  dmr6x2uv_limits.cc dmr6x2uv_limits.hh
  dmr6x2uv2.cc dmr6x2uv2.hh
  dmr6x2uv2_codeplug.cc dmr6x2uv2_codeplug.hh
  dmr6x2uv2_limits.cc dmr6x2uv2_limits.hh
  auctus_a6_interface.cc auctus_a6_interface.hh
  dr1801uv.cc dr1801uv.hh
  dr1801uv_interface.cc dr1801uv_interface.hh
  dr1801uv_codeplug.cc dr1801uv_codeplug.hh
  dr1801uv_filereader.cc dr1801uv_filereader.hh
  dr1801uv_limits.cc dr1801uv_limits.hh
  transferflags.hh transferflags.cc
  anytone_settingsextension.hh anytone_settingsextension.cc)

set_target_properties(libdmrconf PROPERTIES
	MACOSX_RPATH TRUE
  OUTPUT_NAME dmrconf
  VERSION "${PROJECT_VERSION}"
  SOVERSION "${PROJECT_VERSION_MAJOR}")

target_link_libraries(libdmrconf PRIVATE Qt6::Core Qt6::SerialPort Qt6::Positioning Qt6::Network
  ${LIBUSB_1_LIBRARIES} ${YAMLCPP_LIBRARIES} ${ADDITIONAL_LIBS})

install(TARGETS libdmrconf DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

# BUG Fix install of headers
#install(FILES ${libdmrconf_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)
#install(FILES ${libdmrconf_MOC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)
#%install(FILES ${PROJECT_BINARY_DIR}/lib/config.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/libdmrconf)

if (UNIX AND NOT APPLE AND ${INSTALL_UDEV_RULES})
  install(FILES "${CMAKE_SOURCE_DIR}/dist/99-qdmr.rules" DESTINATION ${INSTALL_UDEV_PATH})
endif(UNIX AND NOT APPLE AND ${INSTALL_UDEV_RULES})

