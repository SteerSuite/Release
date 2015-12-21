file(GLOB SFAI_SRC src/*.cpp)
file(GLOB SFAI_HDR include/*.h)

add_library(sfAI SHARED ${SFAI_SRC} ${SFAI_HDR})
target_include_directories(sfAI PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(sfAI steerlib util)
add_dependencies(sfAI steerlib util)

install(TARGETS sfAI
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES ${SFAI_HDR} DESTINATION include/sfAI)
