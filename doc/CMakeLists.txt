#
# man-page generation
#
message(STATUS "Using xsltproc: ${XSLTPROC_EXECUTABLE}")
configure_file("dmrconf.in.xml" "dmrconf.xml")

if(UNIX AND NOT APPLE)
  set(DOCBOOK2MAN_XSLT ${CMAKE_CURRENT_SOURCE_DIR}/docbook_man.debian.xsl)
elseif(UNIX AND APPLE)
  set(DOCBOOK2MAN_XSLT ${CMAKE_CURRENT_SOURCE_DIR}/docbook_man.macports.xsl)
endif(UNIX AND NOT APPLE)

add_custom_command(
  OUTPUT dmrconf.1
  COMMAND ${XSLTPROC_EXECUTABLE}
          -o ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.1
          ${DOCBOOK2MAN_XSLT}
          ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.xml
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating man-page for dmrconf."
  VERBATIM)

if (${BUILD_MAN})
  # Create man page from docbook
  add_custom_target(manpage ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.1)
  # install man page on unix systems
  if (UNIX)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dmrconf.1
      DESTINATION ${CMAKE_INSTALL_FULL_MANDIR}/man1/)
  endif (UNIX)
endif(${BUILD_MAN})

#
# optional API docs
#
if (${BUILD_DOCS})
  set(DOXYGEN_PROJECT_NAME "libdrmconf")
  set(DOXYGEN_PROJECT_BRIEF "A library to program DMR radios.")
  set(DOXYGEN_BRIEF_MEMBER_DESC YES)
  set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
  set(DOXYGEN_EXAMPLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/code")
  set(DOXYGEN_DOTFILE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/fig")
  set(DOXYGEN_ENABLE_PREPROCESSING YES)
  set(DOXYGEN_MACRO_EXPANSION YES)
  set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
  set(DOXYGEN_PREDEFINED __attribute__(x) =)
  set(DOXYGEN_IMAGE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/fig")
  doxygen_add_docs(apidocs
    ALL
    ${CMAKE_SOURCE_DIR}/lib
    COMMENT "Build API documentation")
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
          DESTINATION ${CMAKE_INSTALL_DOCDIR}/libdmrconf/)
endif(${BUILD_DOCS})
