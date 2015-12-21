file(GLOB NAVMESH_SRC src/*.cpp)
file(GLOB NAVMESH_HDR include/*.h)

add_library(navmesh SHARED ${NAVMESH_SRC} ${NAVMESH_HDR})
target_include_directories(navmesh PRIVATE
  ./include
  ../external
  ../external/recastnavigation/Recast/Include
  ../external/recastnavigation/DebugUtils/Include
  ../external/recastnavigation/Detour/Include
  ../external/recastnavigation/DetourTileCache/Include
  ../external/recastnavigation/DetourCrowd/Include
  ../steerlib/include
  ../steersimlib/include
  ../util/include
  ../kdtree/include
)
target_link_libraries(navmesh steerlib steersimlib util Recast DebugUtils Detour DetourCrowd)
add_dependencies(navmesh steerlib steersimlib util Recast DebugUtils Detour DetourCrowd)

install(TARGETS navmesh
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
