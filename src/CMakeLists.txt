SET(qdmr_SOURCES main.cc
  configitemwrapper.cc
  application.cc settings.cc dmrcontactdialog.cc dtmfcontactdialog.cc rxgrouplistdialog.cc
	analogchanneldialog.cc digitalchanneldialog.cc channelvalidator.cc channelcombobox.cc
  channelselectiondialog.cc zonedialog.cc scanlistdialog.cc
  ctcssbox.cc verifydialog.cc gpssystemdialog.cc contactselectiondialog.cc
  aprssystemdialog.cc releasenotes.cc roamingzonedialog.cc searchpopup.cc
  configobjectlistview.cc configobjecttableview.cc
  generalsettingsview.cc radioidlistview.cc contactlistview.cc grouplistsview.cc channellistview.cc
  zonelistview.cc scanlistsview.cc positioningsystemlistview.cc roamingzonelistview.cc
  collapsablewidget.cc extensionview.cc extensionwrapper.cc propertydelegate.cc errormessageview.cc
  deviceselectiondialog.cc radioselectiondialog.cc dmriddialog.cc configobjecttypeselectiondialog.cc
  repeaterbookcompleter.cc)
SET(qdmr_MOC_HEADERS
  configitemwrapper.hh
  application.hh settings.hh dmrcontactdialog.hh dtmfcontactdialog.hh rxgrouplistdialog.hh
	analogchanneldialog.hh digitalchanneldialog.hh channelvalidator.hh channelcombobox.hh
  channelselectiondialog.hh zonedialog.hh scanlistdialog.hh
  ctcssbox.hh verifydialog.hh gpssystemdialog.hh contactselectiondialog.hh
  aprssystemdialog.hh releasenotes.hh roamingzonedialog.hh searchpopup.hh
  configobjectlistview.hh configobjecttableview.hh
  generalsettingsview.hh radioidlistview.hh contactlistview.hh grouplistsview.hh channellistview.hh
  zonelistview.hh scanlistsview.hh positioningsystemlistview.hh roamingzonelistview.hh
  collapsablewidget.hh extensionview.hh extensionwrapper.hh propertydelegate.hh errormessageview.hh
  deviceselectiondialog.hh radioselectiondialog.hh dmriddialog.hh configobjecttypeselectiondialog.hh
  repeaterbookcompleter.hh)
SET(qdmr_HEADERS )
SET(qdmr_UI_FORMS dmrcontactdialog.ui dtmfcontactdialog.ui rxgrouplistdialog.ui analogchanneldialog.ui zonedialog.ui
  digitalchanneldialog.ui scanlistdialog.ui configwizardstart.ui verifydialog.ui settingsdialog.ui
  gpssystemdialog.ui aprssystemdialog.ui roamingzonedialog.ui
  configobjectlistview.ui configobjecttableview.ui
  generalsettingsview.ui radioidlistview.ui contactlistview.ui grouplistsview.ui channellistview.ui
  zonelistview.ui scanlistsview.ui positioningsystemlistview.ui roamingzonelistview.ui
  errormessageview.ui extensionview.ui
  deviceselectiondialog.ui radioselectiondialog.ui dmriddialog.ui configobjecttypeselectiondialog.ui)

set(qdmr_TS_FILES ../i18n/de.ts ../i18n/en_US.ts)
qt5_wrap_cpp(qdmr_MOC_SOURCES ${qdmr_MOC_HEADERS})
qt5_wrap_ui(qdmr_UI_SOURCES ${qdmr_UI_FORMS})

qt5_add_translation(qdmr_QM_FILES ${qdmr_TS_FILES})
message(STATUS "Translations at: ${qdmr_QM_FILES}")

qt5_add_resources(qdmr_RCC_SOURCES ../shared/resources.qrc)

add_executable(qdmr WIN32
  ${qdmr_SOURCES} ${qdmr_MOC_SOURCES} ${qdmr_RCC_SOURCES} ${qdmr_UI_SOURCES} ${qdmr_QM_FILES})

if (UNIX AND APPLE AND INSTALL_BUNDLE)
	set_target_properties(qdmr PROPERTIES
		MACOSX_BUNDLE ON
		MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME}
		MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
		MACOSX_BUNDLE_GUI_IDENTIFIER "de.darc.dm3mat.qdmr"
		MACOSX_BUNDLE_ICON_FILE "Resources/qdmr.icns"
		MACOSX_BUNDLE_LONG_VERSION_STRING ${CMAKE_PROJECT_VERSION}
		MACOSX_BUNDLE_SHORT_VERSION_STRING "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}"
    RESOURCE "${CMAKE_SOURCE_DIR}/dist/macosx/qdmr.icns" ${qdmr_QM_FILES})
endif(UNIX AND APPLE AND INSTALL_BUNDLE)

target_link_libraries(qdmr ${LIBS} libdmrconf)
target_include_directories(qdmr PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

# Install binary only if not a bundle under MacOS X
if (UNIX AND APPLE AND INSTALL_BUNDLE)
	install(TARGETS qdmr
    BUNDLE DESTINATION ${BUNDLE_PATH}/
    RESOURCE DESTINATION "${BUNDLE_PATH}/${CMAKE_PROJECT_NAME}.app/Contents/Resources")
else()
	install(TARGETS qdmr DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
  foreach(QM_FILE ${qdmr_QM_FILES})
     get_filename_component(QM_LANGUAGE ${QM_FILE} NAME_WE)
     install(FILES ${QM_FILE}
       DESTINATION "${LOCALE_DIRECTORY}/${QM_LANGUAGE}/LC_MESSAGES/"
       RENAME "qdmr_qt.qm")
  endforeach()
endif(UNIX AND APPLE AND INSTALL_BUNDLE)
