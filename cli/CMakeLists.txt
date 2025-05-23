qt_add_executable(dmrconf WIN32
  main.cc
  printprogress.cc printprogress.hh
  detect.cc detect.hh
  verify.cc verify.hh
  readcodeplug.cc readcodeplug.hh
  writecodeplug.cc writecodeplug.hh
  encodecodeplug.cc encodecodeplug.hh
  decodecodeplug.cc decodecodeplug.hh
  infofile.cc infofile.hh
  writecallsigndb.cc writecallsigndb.hh
  encodecallsigndb.cc encodecallsigndb.hh
  progressbar.cc progressbar.hh
  autodetect.cc autodetect.hh)

target_link_libraries(
  dmrconf PRIVATE Qt6::Core Qt6::SerialPort Qt6::Network Qt6::Positioning
                  ${LIBUSB_1_LIBRARIES} ${YAMLCPP_LIBRARIES} libdmrconf)

install(TARGETS dmrconf DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
