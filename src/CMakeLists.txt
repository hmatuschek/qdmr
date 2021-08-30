SET(qdmr_SOURCES main.cc
  configitemwrapper.cc
  application.cc settings.cc contactdialog.cc rxgrouplistdialog.cc
	analogchanneldialog.cc digitalchanneldialog.cc channelvalidator.cc channelcombobox.cc
  channelselectiondialog.cc zonedialog.cc scanlistdialog.cc
  ctcssbox.cc verifydialog.cc gpssystemdialog.cc contactselectiondialog.cc
  aprssystemdialog.cc releasenotes.cc roamingzonedialog.cc searchpopup.cc)
SET(qdmr_MOC_HEADERS
  configitemwrapper.hh
  application.hh settings.hh contactdialog.hh rxgrouplistdialog.hh
	analogchanneldialog.hh digitalchanneldialog.hh channelvalidator.hh channelcombobox.hh
  channelselectiondialog.hh zonedialog.hh scanlistdialog.hh
  ctcssbox.hh verifydialog.hh gpssystemdialog.hh contactselectiondialog.hh
  aprssystemdialog.hh releasenotes.hh roamingzonedialog.hh searchpopup.hh)
SET(qdmr_HEADERS )
SET(qdmr_UI_FORMS contactdialog.ui rxgrouplistdialog.ui analogchanneldialog.ui zonedialog.ui
  digitalchanneldialog.ui scanlistdialog.ui configwizardstart.ui verifydialog.ui settingsdialog.ui
  gpssystemdialog.ui aprssystemdialog.ui roamingzonedialog.ui)

qt5_wrap_cpp(qdmr_MOC_SOURCES ${qdmr_MOC_HEADERS})
qt5_wrap_ui(qdmr_UI_SOURCES ${qdmr_UI_FORMS})
qt5_add_resources(qdmr_RCC_SOURCES ../shared/resources.qrc)

add_executable(qdmr WIN32 ${qdmr_SOURCES} ${qdmr_MOC_SOURCES} ${qdmr_RCC_SOURCES} ${qdmr_UI_SOURCES})
target_link_libraries(qdmr ${LIBS} libdmrconf)
target_include_directories(qdmr PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
install(TARGETS qdmr DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

