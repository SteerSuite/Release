file(GLOB KDTREE_SRC src/*.cpp)
file(GLOB KDTREE_HDR include/*.h)

add_library(kdtree SHARED ${KDTREE_SRC} ${KDTREE_HDR})
target_include_directories(kdtree PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(kdtree steerlib util)
add_dependencies(kdtree steerlib util)

install(TARGETS kdtree
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
