SET(qdmr_SOURCES main.cc
	application.cc settings.cc configview.cc contactdialog.cc rxgrouplistdialog.cc channellistview.cc
	analogchanneldialog.cc digitalchanneldialog.cc channelvalidator.cc channelcombobox.cc
	channelselectiondialog.cc zonedialog.cc scanlistview.cc scanlistdialog.cc repeaterdatabase.cc
	ctcssbox.cc verifydialog.cc)
SET(qdmr_MOC_HEADERS
	application.hh settings.hh configview.hh contactdialog.hh rxgrouplistdialog.hh channellistview.hh
	analogchanneldialog.hh digitalchanneldialog.hh channelvalidator.hh channelcombobox.hh
	channelselectiondialog.hh zonedialog.hh scanlistview.hh scanlistdialog.hh repeaterdatabase.hh
	ctcssbox.hh verifydialog.hh)
SET(qdmr_HEADERS )
SET(qdmr_UI_FORMS contactdialog.ui rxgrouplistdialog.ui analogchanneldialog.ui zonedialog.ui
	digitalchanneldialog.ui scanlistdialog.ui configwizardstart.ui verifydialog.ui settingsdialog.ui)

SET(CMAKE_AUTOUIC ON)
qt5_wrap_cpp(qdmr_MOC_SOURCES ${qdmr_MOC_HEADERS})
qt5_add_resources(qdmr_RCC_SOURCES ../shared/resources.qrc)

add_executable(qdmr WIN32 ${qdmr_SOURCES} ${qdmr_MOC_SOURCES} ${qdmr_RCC_SOURCES} ${qdmr_UI_FORMS})
target_link_libraries(qdmr ${LIBS} libdmrconf "-framework IOKit -framework CoreFoundation")


