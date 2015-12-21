file(GLOB STEERSIMLIB_SRC src/*.cpp)
file(GLOB STEERSIMLIB_HDR include/*.h)

add_library(steersimlib SHARED ${STEERSIMLIB_SRC} ${STEERSIMLIB_HDR})
target_include_directories(steersimlib PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(steersimlib steerlib util tinyxml glfw)
add_dependencies(steersimlib steerlib util tinyxml glfw)
#if(${Qt5OpenGL_FOUND})
#  qt5_use_modules(steersimlib Gui OpenGL Core)
#endif()

install(TARGETS steersimlib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

if(NOT WIN32)
  target_link_libraries(steersimlib dl)
endif()
install(DIRECTORY include/ DESTINATION include/steersimlib)
