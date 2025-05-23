set(GENERATE_ICONS_OUTPUT_FILES "")



function(_create_build_icon_command icon_name icon_source_dir icon_theme icon_context icon_size icon_output_dir)
  cmake_path(APPEND icon_source_dir ${icon_theme} ${icon_context} "${icon_name}.svg"
             OUTPUT_VARIABLE input_file)
  cmake_path(APPEND icon_output_dir ${icon_theme} "${icon_size}x${icon_size}" ${icon_context} "${icon_name}.png"
             OUTPUT_VARIABLE output_file)
  list(APPEND GENERATE_ICONS_OUTPUT_FILES ${output_file})
  add_custom_command(
    OUTPUT ${output_file}
    #COMMAND convert ARGS -background none -resize ${icon_size}x${icon_size} ${input_file} ${output_file}
    COMMAND rsvg-convert ARGS -w ${icon_size} -h ${icon_size} -o ${output_file} ${input_file}
    DEPENDS ${input_file}
    COMMENT "Generate ${output_file}"
    VERBATIM)
  return(PROPAGATE GENERATE_ICONS_OUTPUT_FILES)
endfunction()



function(generate_icons)
  cmake_parse_arguments(GENERATE_ICONS "" "DIRECTORY;CONTEXT;" "THEMES;ICONS;SIZES" ${ARGN})

  cmake_path(APPEND CMAKE_CURRENT_BINARY_DIR "icons" OUTPUT_VARIABLE GENERATE_ICONS_OUTPUT_DIR)

  foreach(ICON_THEME ${GENERATE_ICONS_THEMES})
    file(MAKE_DIRECTORY "${GENERATE_ICONS_OUTPUT_DIR}/${ICON_THEME}/scalable/${GENERATE_ICONS_CONTEXT}")
    foreach(ICON_NAME ${GENERATE_ICONS_ICONS})
      cmake_path(APPEND GENERATE_ICONS_DIRECTORY ${ICON_THEME} ${GENERATE_ICONS_CONTEXT} "${ICON_NAME}.svg" OUTPUT_VARIABLE source_file)
      cmake_path(APPEND GENERATE_ICONS_OUTPUT_DIR ${ICON_THEME} "scalable" ${GENERATE_ICONS_CONTEXT} OUTPUT_VARIABLE output_dir)
      cmake_path(APPEND output_dir "${ICON_NAME}.svg" OUTPUT_VARIABLE output_file)
      list(APPEND GENERATE_ICONS_OUTPUT_FILES ${output_file})
      add_custom_command(
        OUTPUT ${output_file}
        COMMAND ${CMAKE_COMMAND} -E copy ${source_file} ${output_dir}
        DEPENDS ${source_file}
        COMMENT "Copy ${output_file}"
        VERBATIM)
    endforeach()

    foreach(ICON_SIZE ${GENERATE_ICONS_SIZES})
      cmake_path(APPEND GENERATE_ICONS_OUTPUT_DIR ${ICON_THEME} "${ICON_SIZE}x${ICON_SIZE}" ${GENERATE_ICONS_CONTEXT}
                        OUTPUT_VARIABLE SCALED_ICONS_OUTPUT_DIR)
      file(MAKE_DIRECTORY ${SCALED_ICONS_OUTPUT_DIR})
      foreach(ICON_NAME ${GENERATE_ICONS_ICONS})
        cmake_path(APPEND GENERATE_ICONS_DIRECTORY ${ICON_THEME} ${GENERATE_ICONS_CONTEXT} "${ICON_NAME}.svg"
                          OUTPUT_VARIABLE source_file)
        _create_build_icon_command(${ICON_NAME} ${GENERATE_ICONS_DIRECTORY} ${ICON_THEME} ${GENERATE_ICONS_CONTEXT} ${ICON_SIZE} ${GENERATE_ICONS_OUTPUT_DIR})
      endforeach()
    endforeach()
  endforeach()
  return(PROPAGATE GENERATE_ICONS_OUTPUT_FILES)
endfunction()

