SET(qdmr_SOURCES main.cc
	application.cc settings.cc config.cc contact.cc rxgrouplist.cc channel.cc zone.cc scanlist.cc radio.cc
	hidinterface.cc hid_macos.cc codeplug.cc rd5r_codeplug.cc rd5r.cc repeaterdatabase.cc
	ctcssbox.cc verifydialog.cc)
SET(qdmr_MOC_HEADERS
	application.hh settings.hh config.hh contact.hh rxgrouplist.hh channel.hh zone.hh scanlist.hh radio.hh
	hidinterface.hh hid_macos.hh codeplug.hh rd5r_codeplug.hh rd5r.hh repeaterdatabase.hh
	ctcssbox.hh verifydialog.hh)
SET(qdmr_HEADERS )
SET(qdmr_UI_FORMS contactdialog.ui rxgrouplistdialog.ui analogchanneldialog.ui zonedialog.ui
	digitalchanneldialog.ui scanlistdialog.ui configwizardstart.ui verifydialog.ui)

SET(CMAKE_AUTOUIC ON)
qt5_wrap_cpp(qdmr_MOC_SOURCES ${qdmr_MOC_HEADERS})
qt5_add_resources(qdmr_RCC_SOURCES ../shared/resources.qrc)

add_executable(qdmr WIN32 ${qdmr_SOURCES} ${qdmr_MOC_SOURCES} ${qdmr_RCC_SOURCES} ${qdmr_UI_FORMS})

target_link_libraries(qdmr ${LIBS} "-framework IOKit -framework CoreFoundation")
