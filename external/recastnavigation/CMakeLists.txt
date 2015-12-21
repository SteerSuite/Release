file(GLOB RECAST_SRC Recast/Source/*.cpp)
file(GLOB RECAST_HDR Recast/Include/*.h)
add_library(Recast STATIC ${RECAST_SRC} ${RECAST_HDR})
target_include_directories(Recast PRIVATE
  ./Recast/Include
  ..
  ../../navmeshBuilder/include
  ../../steerlib/include
  ../../util/include
)
target_link_libraries(Recast steerlib util)
add_dependencies(Recast steerlib util)

file(GLOB DETOUR_SRC Detour/Source/*.cpp)
file(GLOB DETOUR_HDR Detour/Include/*.h)
add_library(Detour STATIC ${DETOUR_SRC} ${DETOUR_HDR})
target_include_directories(Detour PRIVATE
  ./Detour/Include
  ./Recast/Include
  ..
  ../../navmeshBuilder/include
  ../../steerlib/include
  ../../util/include
)
target_link_libraries(Detour Recast)
add_dependencies(Detour Recast)

file(GLOB DETOURCROWD_SRC DetourCrowd/Source/*.cpp)
file(GLOB DETOURCROWD_HDR DetourCrowd/Include/*.h)
add_library(DetourCrowd STATIC ${DETOURCROWD_SRC} ${DETOURCROWD_HDR})
target_include_directories(DetourCrowd PRIVATE
  ./DetourCrowd/Include
  ./Detour/Include
  ./Recast/Include
  ./DetourTileCache/Include
  ./DebugUtils/Include
  ..
  ../../navmeshBuilder/include
  ../../steerlib/include
  ../../util/include
)
target_link_libraries(DetourCrowd Detour Recast)
add_dependencies(DetourCrowd Detour Recast)

file(GLOB DEBUGUTILS_SRC DebugUtils/Source/*.cpp)
file(GLOB DEBUGUTILS_HDR DebugUtils/Include/*.h)
add_library(DebugUtils STATIC ${DEBUGUTILS_SRC} ${DEBUGUTILS_HDR})
target_include_directories(DebugUtils PRIVATE
  ./DebugUtils/Include
  ./Detour/Include
  ./Recast/Include
  ./DetourTileCache/Include
  ..
  ../../navmeshBuilder/include
  ../../steerlib/include
  ../../util/include
)
target_link_libraries(DebugUtils Recast Detour)
add_dependencies(DebugUtils Recast Detour)

install(TARGETS Recast Detour DetourCrowd DebugUtils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
