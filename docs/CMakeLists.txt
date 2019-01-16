include(FindDoxygen)

option(BUILD_DOCS "Build doxygen documentation when building all" OFF)
mark_as_advanced(BUILD_DOCS)

if(NOT DOXYGEN_DOT_FOUND) 
  message(WARNING "Graphviz doesn't seem to be installed. Doxygen will not be able to generate graphs. Consider installing this package.")
endif(NOT DOXYGEN_DOT_FOUND) 

if(DOXYGEN_FOUND)
  
  # Configure the doxygen config file with current settings:
  configure_file(documentation-config.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/documentation-config.doxygen @ONLY)

  # Set the name of the target : "docs" if it doesn't already exist and "docs<projectname>" if it does.
  # This way we make sure to have a single "docs" target. Either it is the one of the top directory or
  # it is the one of the subproject that we are compiling alone.
  set(DOC_TARGET_NAME "doc")
  if(TARGET doc)
    set(DOC_TARGET_NAME "doc${PROJECT_NAME}")
  endif()

  add_custom_target(${DOC_TARGET_NAME} ${TARGET_ALL}
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/documentation-config.doxygen
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation using doxygen for ${PROJECT_NAME}" VERBATIM)

  set(INSTALL_DOC_DIR ${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME}/html)
  file(MAKE_DIRECTORY ${INSTALL_DOC_DIR}) # needed for install

  install(DIRECTORY ${INSTALL_DOC_DIR} DESTINATION share/${PROJECT_NAME}-${VERSION_MAJOR} COMPONENT doc)

endif(DOXYGEN_FOUND)