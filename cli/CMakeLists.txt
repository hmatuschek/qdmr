set(dmrconf_SOURCES main.cc
	printprogress.cc detect.cc verify.cc readcodeplug.cc writecodeplug.cc encodecodeplug.cc
	decodecodeplug.cc infofile.cc)
set(dmrconf_MOC_HEADERS )
set(dmrconf_HEADERS
	printprogress.hh detect.hh verify.hh readcodeplug.hh writecodeplug.hh encodecodeplug.hh
	decodecodeplug.hh infofile.hh
	${dmrconf_MOC_HEADERS})


qt5_wrap_cpp(dmrconf_MOC_SOURCES ${dmrconf_MOC_HEADERS})

add_executable(dmrconf WIN32 ${dmrconf_SOURCES} ${dmrconf_MOC_SOURCES})
target_link_libraries(dmrconf ${LIBS} libdmrconf "-framework IOKit -framework CoreFoundation")
