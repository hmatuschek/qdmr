
qt6_add_resources(qdmr_RCC_SOURCES ../shared/resources.qrc)

qt6_add_executable(qdmr WIN32
  main.cc
  configitemwrapper.cc configitemwrapper.hh
  application.cc application.hh
  settings.cc settings.hh settingsdialog.ui
  dmrcontactdialog.cc dmrcontactdialog.hh dmrcontactdialog.ui
  dtmfcontactdialog.cc dtmfcontactdialog.hh dtmfcontactdialog.ui
  rxgrouplistdialog.cc rxgrouplistdialog.hh rxgrouplistdialog.ui
  analogchanneldialog.cc analogchanneldialog.hh analogchanneldialog.ui
  digitalchanneldialog.cc digitalchanneldialog.hh digitalchanneldialog.ui
  channelvalidator.cc channelvalidator.hh
  channelcombobox.cc channelcombobox.hh
  channelselectiondialog.cc channelselectiondialog.hh
  zonedialog.cc zonedialog.hh zonedialog.ui
  scanlistdialog.cc scanlistdialog.hh scanlistdialog.ui
  verifydialog.cc verifydialog.hh verifydialog.ui
  gpssystemdialog.cc gpssystemdialog.hh gpssystemdialog.ui
  contactselectiondialog.cc contactselectiondialog.hh
  searchpopup.cc searchpopup.hh
  aprssystemdialog.cc aprssystemdialog.hh aprssystemdialog.ui
  releasenotes.cc releasenotes.hh
  roamingzonedialog.cc roamingzonedialog.hh roamingzonedialog.ui
  roamingchannellistview.cc roamingchannellistview.hh roamingchannellistview.ui
  roamingchanneldialog.cc roamingchanneldialog.hh roamingchanneldialog.ui
  roamingchannelselectiondialog.cc roamingchannelselectiondialog.hh
  configobjectlistview.cc configobjectlistview.hh configobjectlistview.ui
  configobjecttableview.cc configobjecttableview.hh configobjecttableview.ui
  generalsettingsview.cc generalsettingsview.hh generalsettingsview.ui
  radioidlistview.cc radioidlistview.hh radioidlistview.ui
  contactlistview.cc contactlistview.hh contactlistview.ui
  grouplistsview.cc grouplistsview.hh grouplistsview.ui
  channellistview.cc channellistview.hh channellistview.ui
  zonelistview.cc zonelistview.hh zonelistview.ui
  scanlistsview.cc scanlistsview.hh scanlistsview.ui
  positioningsystemlistview.cc positioningsystemlistview.hh positioningsystemlistview.ui
  roamingzonelistview.cc roamingzonelistview.hh roamingzonelistview.ui
  collapsablewidget.cc collapsablewidget.hh
  extensionview.cc extensionview.hh extensionview.ui
  extensionwrapper.cc extensionwrapper.hh
  propertydelegate.cc propertydelegate.hh
  errormessageview.cc errormessageview.hh errormessageview.ui
  deviceselectiondialog.cc deviceselectiondialog.hh deviceselectiondialog.ui
  radioselectiondialog.cc radioselectiondialog.hh radioselectiondialog.ui
  dmriddialog.cc dmriddialog.hh dmriddialog.ui
  configobjecttypeselectiondialog.cc configobjecttypeselectiondialog.hh configobjecttypeselectiondialog.ui
  configmergedialog.cc configmergedialog.hh configmergedialog.ui
  satellitedatabasedialog.cc satellitedatabasedialog.hh satellitedatabasedialog.ui
  satelliteselectiondialog.cc satelliteselectiondialog.hh satelliteselectiondialog.ui
  repeaterdatabase.cc repeaterdatabase.hh
  repeatercompleter.cc repeatercompleter.hh
  repeaterbooksource.cc repeaterbooksource.hh
  repeatermapsource.cc repeatermapsource.hh
  hearhamrepeatersource.cc hearhamrepeatersource.hh
  radioidrepeatersource.cc radioidrepeatersource.hh
  selectivecallbox.cc selectivecallbox.hh
  transponderfrequencydelegate.cc transponderfrequencydelegate.hh
  mainwindow.cc mainwindow.hh mainwindow.ui
  ${qdmr_RCC_SOURCES})

#
# Translations
#
qt6_add_translations(qdmr TS_FILES
  ${PROJECT_SOURCE_DIR}/i18n/de.ts ${PROJECT_SOURCE_DIR}/i18n/en_US.ts
  ${PROJECT_SOURCE_DIR}/i18n/sv.ts ${PROJECT_SOURCE_DIR}/i18n/it.ts
  ${PROJECT_SOURCE_DIR}/i18n/nl.ts ${PROJECT_SOURCE_DIR}/i18n/pl.ts
  ${PROJECT_SOURCE_DIR}/i18n/fr.ts ${PROJECT_SOURCE_DIR}/i18n/pt_BR.ts)

#
# Icon generation
#
generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS application-qdmr
  THEMES light dark
  CONTEXT apps
  SIZES 16 32 48 64)

generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS application-exit application-settings
        device-read device-write-callsign device-write device-search device-write-satellites
        document-new document-open document-verify document-save-as document-download
        document-import document-export
        edit-move-10-down edit-move-10-up edit-move-down edit-move-up edit-move-bottom edit-move-top
        edit-satellites edit-clear item-filter
        help-contents help-about
        symbol-minus symbol-plus symbol-none
  THEMES light dark
  CONTEXT actions
  SIZES 16 32)

generate_icons(
  DIRECTORY ${CMAKE_SOURCE_DIR}/shared/icons
  ICONS symbol-info symbol-warning symbol-error
  THEMES light dark
  CONTEXT emblems
  SIZES 16 32)

qt_add_resources(qdmr "icons"
  PREFIX "icons"
  BASE "${CMAKE_CURRENT_BINARY_DIR}/icons"
  FILES ${GENERATE_ICONS_OUTPUT_FILES})

qt_add_resources(qdmr "icons-themes"
  PREFIX "icons"
  BASE "${CMAKE_SOURCE_DIR}/shared/icons"
  FILES "${CMAKE_SOURCE_DIR}/shared/icons/light/index.theme"
        "${CMAKE_SOURCE_DIR}/shared/icons/dark/index.theme")


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

target_link_libraries(qdmr PRIVATE Qt6::Core Qt6::Widgets Qt6::Network Qt6::Positioning Qt6::UiTools
  ${LIBUSB_1_LIBS} libdmrconf)
target_include_directories(qdmr PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

# Install binary only if not a bundle under MacOS X
if (UNIX AND APPLE AND INSTALL_BUNDLE)
  install(TARGETS qdmr
    BUNDLE DESTINATION ${BUNDLE_PATH}/
    RESOURCE DESTINATION "${BUNDLE_PATH}/${CMAKE_PROJECT_NAME}.app/Contents/Resources")
else()
  install(TARGETS qdmr DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
endif(UNIX AND APPLE AND INSTALL_BUNDLE)
