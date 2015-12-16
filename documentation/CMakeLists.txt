find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(doxyfile ${CMAKE_CURRENT_SOURCE_DIR}/rawinput/doxygen-1.5.8.cfg)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

 install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/CodeReference/ DESTINATION doc)
endif()
