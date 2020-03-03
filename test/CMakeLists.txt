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

qt5_wrap_cpp(rd5rtest_MOC_SOURCES rd5rtest.hh)
add_executable(rd5rtest rd5rtest.cc ${rd5rtest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(rd5rtest ${LIBS} libdmrconf)

qt5_wrap_cpp(uv390test_MOC_SOURCES uv390test.hh)
add_executable(uv390test uv390test.cc ${uv390test_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(uv390test ${LIBS} libdmrconf)

add_test(NAME Config COMMAND configtest)
add_test(NAME CRC32  COMMAND crc32test)
add_test(NAME Utils  COMMAND utilstest)
add_test(NAME RD5R   COMMAND rd5rtest)
add_test(NAME UV390  COMMAND uv390test)
