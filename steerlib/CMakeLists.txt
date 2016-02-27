file(GLOB STEERLIB_SRC src/*.cpp)
file(GLOB STEERLIB_HDR include/*.h)

add_library(steerlib SHARED ${STEERLIB_SRC} ${STEERLIB_HDR})
target_include_directories(steerlib PRIVATE
  ./include
  ../external
  ./include/util
  ../util/include
)
target_link_libraries(steerlib tinyxml util ${GLUT_LIBRARIES})
add_dependencies(steerlib tinyxml util)

install(TARGETS steerlib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/steerlib)
install(DIRECTORY ../external/mersenne DESTINATION include/steerlib)
