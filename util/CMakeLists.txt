file(GLOB UTIL_SRC src/*.cpp)
file(GLOB UTIL_HDR include/*.h)

add_library(util SHARED ${UTIL_SRC} ${UTIL_HDR})
target_include_directories(util PRIVATE include)

install(TARGETS util
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/util)
