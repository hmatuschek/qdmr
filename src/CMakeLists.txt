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

qt5_wrap_cpp(qdmr_MOC_SOURCES ${qdmr_MOC_HEADERS})
qt5_wrap_ui(qdmr_UI_SOURCES ${qdmr_UI_FORMS})
qt5_add_resources(qdmr_RCC_SOURCES ../shared/resources.qrc)

add_executable(qdmr WIN32 ${qdmr_SOURCES} ${qdmr_MOC_SOURCES} ${qdmr_RCC_SOURCES} ${qdmr_UI_SOURCES})
target_link_libraries(qdmr ${LIBS} libdmrconf)
target_include_directories(qdmr PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
install(TARGETS qdmr DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

