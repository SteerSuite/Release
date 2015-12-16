file(GLOB GLFW_SRC lib/*.c)
file(GLOB GLFW_HDR lib/*.h)
if(WIN32)
  file(GLOB GLFW_OS_SRC lib/win32/*.c)
  file(GLOB GLFW_OS_HDR lib/win32/*.h)
  set(GLFW_OS_SRC ${GLFW_OS_SRC} lib/win32/glfwdll.def)
elseif(APPLE)
  file(GLOB GLFW_OS_SRC lib/cocoa/*.c)
  file(GLOB GLFW_OS_HDR lib/cocoa/*.h)
  file(GLOB GLFW_OS_TMP lib/cocoa/*.m)
  set(GLFW_OS_SRC ${GLFW_OS_SRC} ${GLFW_OS_TMP})
else()
  file(GLOB GLFW_OS_SRC lib/x11/*.c)
  file(GLOB GLFW_OS_HDR lib/x11/*.h)
endif()

add_library(glfw SHARED ${GLFW_SRC} ${GLFW_HDR} ${GLFW_OS_SRC} ${GLFW_OS_HDR})
target_include_directories(glfw PRIVATE
  ./lib
  ./include
  ./include/GL
)
target_compile_definitions(glfw PRIVATE -DGLFW_BUILD_DLL)
target_link_libraries(glfw ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLUT_LIBRARIES})

if(WIN32)
  target_compile_definitions(glfw PRIVATE -D_GLFW_USE_LINUX_JOYSTICKS -D_GLFW_HAS_XRANDR -D_GLFW_HAS_PTHREAD -D_GLFW_HAS_SCHED_YIELD -D_GLFW_HAS_GLXGETPROCADDRESS -DGLFW_BUILD_DLL)
  target_include_directories(glfw PRIVATE lib/win32)
elseif(APPLE)
  target_include_directories(glfw PRIVATE lib/cocoa)
  find_library(COCOA_LIBRARY Cocoa)
  find_library(IOKIT_LIBRARY IOKit)
  mark_as_advanced(COCOA_LIBRARY IOKIT_LIBRARY)
  target_link_libraries(glfw ${COCOA_LIBRARY} ${IOKIT_LIBRARY} pthread)
else()
  find_package(X11 REQUIRED)
  find_library(XRANDR_LIBRARY Xrandr)
  mark_as_advanced(XRANDR_LIBRARY)
  target_compile_definitions(glfw PRIVATE -D_GLFW_USE_LINUX_JOYSTICKS -D_GLFW_HAS_XRANDR -D_GLFW_HAS_PTHREAD -D_GLFW_HAS_SCHED_YIELD -D_GLFW_HAS_GLXGETPROCADDRESS)
  target_include_directories(glfw PRIVATE lib/x11 ${X11_INCLUDE_DIR})
  target_link_libraries(glfw pthread ${X11_LIBRARIES} ${XRANDR_LIBRARY})
endif()

install(TARGETS glfw
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
