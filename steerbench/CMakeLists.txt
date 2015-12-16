file(GLOB STEERBENCH_SRC src/*.cpp)
#file(GLOB STEERBENCH_HDR include/*.h)

add_executable(steerbench ${STEERBENCH_SRC})
target_include_directories(steerbench PRIVATE
  ./src
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(steerbench steerlib util glfw tinyxml)
add_dependencies(steerbench steerlib util glfw tinyxml)

if(WIN32)
elseif(APPLE)
  find_library(COCOA_LIBRARY Cocoa)
  mark_as_advanced(COCOA_LIBRARY)
  target_link_libraries(steerbench ${COCOA_LIBRARY} pthread dl)
else()
  find_package(X11 REQUIRED)
  target_link_libraries(steerbench pthread ${X11_LIBRARIES} dl)
endif()

install(TARGETS steerbench
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
