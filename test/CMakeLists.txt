qt5_add_resources(testlib_RCC_SOURCES resources.qrc)

qt5_wrap_cpp(configtest_MOC_SOURCES configtest.hh)
add_executable(configtest configtest.cc ${configtest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(configtest ${LIBS} libdmrconf)

qt5_wrap_cpp(crc32test_MOC_SOURCES crc32test.hh)
add_executable(crc32test crc32test.cc ${crc32test_MOC_SOURCES})
target_link_libraries(crc32test ${LIBS} libdmrconf)

qt5_wrap_cpp(utilstest_MOC_SOURCES utilstest.hh)
add_executable(utilstest utilstest.cc ${utilstest_MOC_SOURCES})
target_link_libraries(utilstest ${LIBS} libdmrconf)

add_test(NAME Config COMMAND configtest)
add_test(NAME CRC32  COMMAND crc32test)
add_test(NAME Utils  COMMAND utilstest)

