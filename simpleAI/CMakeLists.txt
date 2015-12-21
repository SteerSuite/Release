file(GLOB SIMPLEAI_SRC src/*.cpp)
file(GLOB SIMPLEAI_HDR include/*.h)

add_library(simpleAI SHARED ${SIMPLEAI_SRC} ${SIMPLEAI_HDR})
target_include_directories(simpleAI PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(simpleAI steerlib util)
add_dependencies(simpleAI steerlib util)

install(TARGETS simpleAI
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES ${SIMPLEAI_HDR} DESTINATION include/simpleAI)
