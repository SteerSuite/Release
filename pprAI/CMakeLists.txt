file(GLOB PPRAI_SRC src/*.cpp)
file(GLOB PPRAI_HDR include/*.h)

add_library(pprAI SHARED ${PPRAI_SRC} ${PPRAI_HDR})
target_include_directories(pprAI PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(pprAI steerlib util)
add_dependencies(pprAI steerlib util)

install(TARGETS pprAI
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES ${PPRAI_HDR} DESTINATION include/pprAI)
