
message(STATUS "Using xsltproc: ${XSLTPROC_EXECUTABLE}")
configure_file("dmrconf.in.xml" "dmrconf.xml")

add_custom_command(
  OUTPUT dmrconf.1
  COMMAND ${XSLTPROC_EXECUTABLE}
          -o ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.1
          ${CMAKE_CURRENT_SOURCE_DIR}/docbook_man.xsl
          ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.xml
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generate man-page for dmrconf."
  VERBATIM)

add_custom_target(manpage ALL DEPENDS dmrconf.1)

if (UNIX AND NOT APPLE)
 install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.1
         DESTINATION ${CMAKE_INSTALL_FULL_MANDIR}/man1/)
endif (UNIX AND NOT APPLE)
