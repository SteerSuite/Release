file(GLOB RVO2AI_SRC src/*.cpp)
file(GLOB RVO2AI_HDR include/*.h)

add_library(rvo2AI SHARED ${RVO2AI_SRC} ${RVO2AI_HDR})
target_include_directories(rvo2AI PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
  ../kdtree/include
)
target_link_libraries(rvo2AI steerlib util)
add_dependencies(rvo2AI steerlib util)

install(TARGETS rvo2AI
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES ${RVO2AI_HDR} DESTINATION include/rvo2AI)
