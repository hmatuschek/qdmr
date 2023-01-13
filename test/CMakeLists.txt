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

# Unit tests for AnyTone devices
qt5_wrap_cpp(d868uve_MOC_SOURCES d868uve_test.hh)
add_executable(d868uve_test d868uve_test.cc ${d868uve_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(d868uve_test ${LIBS} libdmrconf)

qt5_wrap_cpp(d878uv_MOC_SOURCES d878uv_test.hh)
add_executable(d878uv_test d878uv_test.cc ${d878uv_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(d878uv_test ${LIBS} libdmrconf)

qt5_wrap_cpp(d878uv2_MOC_SOURCES d878uv2_test.hh)
add_executable(d878uv2_test d878uv2_test.cc ${d878uv2_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(d878uv2_test ${LIBS} libdmrconf)

qt5_wrap_cpp(d578uv_MOC_SOURCES d578uv_test.hh)
add_executable(d578uv_test d578uv_test.cc ${d578uv_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(d578uv_test ${LIBS} libdmrconf)

qt5_wrap_cpp(dmr6x2uv_MOC_SOURCES dmr6x2uv_test.hh)
add_executable(dmr6x2uv_test dmr6x2uv_test.cc ${dmr6x2uv_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(dmr6x2uv_test ${LIBS} libdmrconf)


add_test(NAME Config    COMMAND configtest)
add_test(NAME CRC32     COMMAND crc32test)
add_test(NAME Utils     COMMAND utilstest)
add_test(NAME D868UVE   COMMAND d868uve_test)
add_test(NAME D878UV    COMMAND d878uv_test)
add_test(NAME D878UV2   COMMAND d878uv2_test)
add_test(NAME D578UV    COMMAND d578uv_test)
add_test(NAME DMR6X2UV  COMMAND dmr6x2uv_test)

