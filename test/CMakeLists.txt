
qt_add_resources(TESTDATA resources.qrc)

qt_add_library(libdmrconfigtest STATIC libdmrconfigtest.cc libdmrconfigtest.hh ${TESTDATA})
target_link_libraries(libdmrconfigtest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf ${ADDITIONAL_LIBS})


qt_add_executable(configtest configtest.cc configtest.hh ${TESTDATA})
target_link_libraries(configtest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(tableformattest tableformattest.cc tableformattest.hh ${TESTDATA})
target_link_libraries(tableformattest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(copytest copytest.cc copytest.hh ${TESTDATA})
target_link_libraries(copytest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(labeltest labeltest.cc labeltest.hh ${TESTDATA})
target_link_libraries(labeltest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(trafotest trafotest.cc trafotest.hh ${TESTDATA})
target_link_libraries(trafotest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(crc32test crc32test.cc crc32test.hh ${TESTDATA})
target_link_libraries(crc32test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(utilstest utilstest.cc utilstest.hh ${TESTDATA})
target_link_libraries(utilstest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(chirptest chirptest.cc chirptest.hh ${TESTDATA})
target_link_libraries(chirptest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(mergetest mergetest.cc mergetest.hh ${TESTDATA})
target_link_libraries(mergetest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(smstemplatetest smstemplatetest.cc smstemplatetest.hh ${TESTDATA})
target_link_libraries(smstemplatetest PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})


# Unit tests for Radioddity devices
qt_add_executable(rd5r_test rd5r_test.cc rd5r_test.hh ${TESTDATA})
target_link_libraries(rd5r_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(gd73_test gd73_test.cc gd73_test.hh ${TESTDATA})
target_link_libraries(gd73_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(gd77_test gd77_test.cc gd77_test.hh ${TESTDATA})
target_link_libraries(gd77_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})


# Unit tests for OpenGD77 firmware
qt_add_executable(opengd77_test opengd77_test.cc opengd77_test.hh ${TESTDATA})
target_link_libraries(opengd77_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

# Unit tests for OpenGD77 firmware
qt_add_executable(openuv380_test openuv380_test.cc openuv380_test.hh ${TESTDATA})
target_link_libraries(openuv380_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})


# Unit tests for TyT devices
qt_add_executable(md390_test md390_test.cc md390_test.hh ${TESTDATA})
target_link_libraries(md390_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(uv390_test uv390_test.cc uv390_test.hh ${TESTDATA})
target_link_libraries(uv390_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(md2017_test md2017_test.cc md2017_test.hh ${TESTDATA})
target_link_libraries(md2017_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})


# Unit tests for AnyTone devices
qt_add_executable(d868uve_test d868uve_test.cc d868uve_test.hh ${TESTDATA})
target_link_libraries(d868uve_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(d878uv_test d878uv_test.cc d878uv_test.hh ${TESTDATA})
target_link_libraries(d878uv_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(d878uv2_test d878uv2_test.cc d878uv2_test.hh ${TESTDATA})
target_link_libraries(d878uv2_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(d578uv_test d578uv_test.cc d578uv_test.hh ${TESTDATA})
target_link_libraries(d578uv_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})


# Unit tests for Baofeng devices
qt_add_executable(dmr6x2uv_test dmr6x2uv_test.cc dmr6x2uv_test.hh ${TESTDATA})
target_link_libraries(dmr6x2uv_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(dm1701_test dm1701_test.cc dm1701_test.hh ${TESTDATA})
target_link_libraries(dm1701_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})

qt_add_executable(dr1801_test dr1801_test.cc dr1801_test.hh ${TESTDATA})
target_link_libraries(dr1801_test PRIVATE Qt6::Core Qt6::Network Qt6::Positioning Qt6::SerialPort Qt6::Test ${YAMLCPP_LIBRARIES} libdmrconf libdmrconfigtest ${ADDITIONAL_LIBS})


add_test(NAME Config    COMMAND configtest)
add_test(NAME TableFormat COMMAND tableformattest)
add_test(NAME Copy      COMMAND copytest)
add_test(NAME Label     COMMAND labeltest)
add_test(NAME Transformations COMMAND trafotest)
add_test(NAME CRC32     COMMAND crc32test)
add_test(NAME Utils     COMMAND utilstest)
add_test(NAME CHIRP     COMMAND chirptest)
add_test(NAME Merge     COMMAND mergetest)
add_test(NAME SMSTemplates COMMAND smstemplatetest)

add_test(NAME RD5R      COMMAND rd5r_test)
add_test(NAME GD73      COMMAND gd73_test)
add_test(NAME GD77      COMMAND gd77_test)

add_test(NAME OpenGD77  COMMAND opengd77_test)
add_test(NAME OpenUV380 COMMAND openuv380_test)

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

