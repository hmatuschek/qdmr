SET(qdmr_SOURCES main.cc
  application.cc settings.cc contactdialog.cc rxgrouplistdialog.cc channellistview.cc
	analogchanneldialog.cc digitalchanneldialog.cc channelvalidator.cc channelcombobox.cc
	channelselectiondialog.cc zonedialog.cc scanlistview.cc scanlistdialog.cc
  ctcssbox.cc verifydialog.cc gpssystemdialog.cc)
SET(qdmr_MOC_HEADERS
  application.hh settings.hh contactdialog.hh rxgrouplistdialog.hh channellistview.hh
	analogchanneldialog.hh digitalchanneldialog.hh channelvalidator.hh channelcombobox.hh
	channelselectiondialog.hh zonedialog.hh scanlistview.hh scanlistdialog.hh
  ctcssbox.hh verifydialog.hh gpssystemdialog.hh)
SET(qdmr_HEADERS )
SET(qdmr_UI_FORMS contactdialog.ui rxgrouplistdialog.ui analogchanneldialog.ui zonedialog.ui
  digitalchanneldialog.ui scanlistdialog.ui configwizardstart.ui verifydialog.ui settingsdialog.ui
  gpssystemdialog.ui)

qt5_wrap_cpp(qdmr_MOC_SOURCES ${qdmr_MOC_HEADERS})
qt5_wrap_ui(qdmr_UI_SOURCES ${qdmr_UI_FORMS})
qt5_add_resources(qdmr_RCC_SOURCES ../shared/resources.qrc)

add_executable(qdmr WIN32 ${qdmr_SOURCES} ${qdmr_MOC_SOURCES} ${qdmr_RCC_SOURCES} ${qdmr_UI_SOURCES})
target_link_libraries(qdmr ${LIBS} libdmrconf)
target_include_directories(qdmr PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
install(TARGETS qdmr DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

if (UNIX AND NOT APPLE)
  install(FILES ${PROJECT_SOURCE_DIR}/dist/qdmr.desktop DESTINATION share/applications/)
  install(FILES ${PROJECT_SOURCE_DIR}/dist/qdmr.png DESTINATION share/icons/hicolor/64x64/)
endif(UNIX AND NOT APPLE)

