qt5_add_resources(testlib_RCC_SOURCES resources.qrc)

qt5_wrap_cpp(libdmrconfigtest_MOC_SOURCES libdmrconfigtest.hh)
add_library(libdmrconfigtest STATIC libdmrconfigtest.cc ${libdmrconfigtest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(libdmrconfigtest ${LIBS} libdmrconf)


qt5_wrap_cpp(configtest_MOC_SOURCES configtest.hh)
add_executable(configtest configtest.cc ${configtest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(configtest ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(tableformattest_MOC_SOURCES tableformattest.hh)
add_executable(tableformattest tableformattest.cc ${tableformattest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(tableformattest ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(copytest_MOC_SOURCES copytest.hh)
add_executable(copytest copytest.cc ${copytest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(copytest ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(labeltest_MOC_SOURCES labeltest.hh)
add_executable(labeltest labeltest.cc ${labeltest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(labeltest ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(trafotest_MOC_SOURCES trafotest.hh)
add_executable(trafotest trafotest.cc ${trafotest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(trafotest ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(crc32test_MOC_SOURCES crc32test.hh)
add_executable(crc32test crc32test.cc ${crc32test_MOC_SOURCES})
target_link_libraries(crc32test ${LIBS} libdmrconf)

qt5_wrap_cpp(utilstest_MOC_SOURCES utilstest.hh)
add_executable(utilstest utilstest.cc ${utilstest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(utilstest ${LIBS} libdmrconf)

qt5_wrap_cpp(chirptest_MOC_SOURCES chirptest.hh)
add_executable(chirptest chirptest.cc ${chirptest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(chirptest ${LIBS} libdmrconf)

qt5_wrap_cpp(mergetest_MOC_SOURCES mergetest.hh)
add_executable(mergetest mergetest.cc ${mergetest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(mergetest ${LIBS} libdmrconf)

qt5_wrap_cpp(smstemplatetest_MOC_SOURCES smstemplatetest.hh)
add_executable(smstemplatetest smstemplatetest.cc ${smstemplatetest_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(smstemplatetest ${LIBS} libdmrconf libdmrconfigtest)


# Unit tests for Radioddity devices
qt5_wrap_cpp(rd5r_MOC_SOURCES rd5r_test.hh)
add_executable(rd5r_test rd5r_test.cc ${rd5r_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(rd5r_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(gd73_MOC_SOURCES gd73_test.hh)
add_executable(gd73_test gd73_test.cc ${gd73_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(gd73_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(gd77_MOC_SOURCES gd77_test.hh)
add_executable(gd77_test gd77_test.cc ${gd77_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(gd77_test ${LIBS} libdmrconf libdmrconfigtest)

# Unit tests for OpenGD77 firmware
qt5_wrap_cpp(opengd77_MOC_SOURCES opengd77_test.hh)
add_executable(opengd77_test opengd77_test.cc ${opengd77_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(opengd77_test ${LIBS} libdmrconf libdmrconfigtest)

# Unit tests for OpenGD77 firmware
qt5_wrap_cpp(openuv380_MOC_SOURCES openuv380_test.hh)
add_executable(openuv380_test openuv380_test.cc ${openuv380_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(openuv380_test ${LIBS} libdmrconf libdmrconfigtest)


# Unit tests for TyT devices
qt5_wrap_cpp(md390_MOC_SOURCES md390_test.hh)
add_executable(md390_test md390_test.cc ${md390_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(md390_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(uv390_MOC_SOURCES uv390_test.hh)
add_executable(uv390_test uv390_test.cc ${uv390_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(uv390_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(md2017_MOC_SOURCES md2017_test.hh)
add_executable(md2017_test md2017_test.cc ${md2017_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(md2017_test ${LIBS} libdmrconf libdmrconfigtest)


# Unit tests for AnyTone devices
qt5_wrap_cpp(d868uve_MOC_SOURCES d868uve_test.hh)
add_executable(d868uve_test d868uve_test.cc ${d868uve_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(d868uve_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(d878uv_MOC_SOURCES d878uv_test.hh)
add_executable(d878uv_test d878uv_test.cc ${d878uv_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(d878uv_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(d878uv2_MOC_SOURCES d878uv2_test.hh)
add_executable(d878uv2_test d878uv2_test.cc ${d878uv2_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(d878uv2_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(d578uv_MOC_SOURCES d578uv_test.hh)
add_executable(d578uv_test d578uv_test.cc ${d578uv_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(d578uv_test ${LIBS} libdmrconf libdmrconfigtest)


# Unit tests for Baofeng devices
qt5_wrap_cpp(dmr6x2uv_MOC_SOURCES dmr6x2uv_test.hh)
add_executable(dmr6x2uv_test dmr6x2uv_test.cc ${dmr6x2uv_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(dmr6x2uv_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(dm1701_MOC_SOURCES dm1701_test.hh)
add_executable(dm1701_test dm1701_test.cc ${dm1701_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(dm1701_test ${LIBS} libdmrconf libdmrconfigtest)

qt5_wrap_cpp(dr1801_MOC_SOURCES dr1801_test.hh)
add_executable(dr1801_test dr1801_test.cc ${dr1801_MOC_SOURCES} ${testlib_RCC_SOURCES})
target_link_libraries(dr1801_test ${LIBS} libdmrconf libdmrconfigtest)


add_test(NAME Config    COMMAND configtest)
add_test(NAME CRC32     COMMAND crc32test)
add_test(NAME Utils     COMMAND utilstest)
add_test(NAME CHIRP     COMMAND chirptest)

add_test(NAME RD5R      COMMAND rd5r_test)
add_test(NAME GD77      COMMAND gd77_test)

add_test(NAME OpenGD77  COMMAND opengd77_test)

add_test(NAME MD390     COMMAND md390_test)
add_test(NAME UV390     COMMAND uv390_test)
add_test(NAME MD2017    COMMAND md2017_test)

add_test(NAME D868UVE   COMMAND d868uve_test)
add_test(NAME D878UV    COMMAND d878uv_test)
add_test(NAME D878UV2   COMMAND d878uv2_test)
add_test(NAME D578UV    COMMAND d578uv_test)

add_test(NAME DMR6X2UV  COMMAND dmr6x2uv_test)
add_test(NAME DM1701    COMMAND dm1701_test)
add_test(NAME DR1801    COMMAND dr1801_test)

