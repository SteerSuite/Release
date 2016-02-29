--
-- premake4 file to build SteerSuite
-- http://steersuite.cse.yorku.ca
-- Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
-- See license.txt for complete license.
--

local action = _ACTION or ""
local todir = "./" .. action

function file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end


solution "steersuite"
	configurations { 
		"Debug",
		"Release"
	}
	location (todir)

	-- extra warnings, no exceptions or rtti
	flags { 
		"ExtraWarnings",
--		"FloatFast",
--		"NoExceptions",
--		"NoRTTI",
		"Symbols"
	}
	defines { "ENABLE_GUI", "ENABLE_GLFW" }

	-- debug configs
	configuration "Debug*"
		defines { "DEBUG" }
		flags {
			"Symbols",
			Optimize = Off
		}
 
 	-- release configs
	configuration "Release*"
		defines { "NDEBUG" }
		flags { "Optimize" }

	-- windows specific
	configuration "windows"
		defines { "WIN32", "_WINDOWS" }
		libdirs { "lib" }
		targetdir ( "bin" )

	configuration { "linux" }
		linkoptions { 
			-- "-stdlib=libc++" ,
			"-Wl,-rpath," .. path.getabsolute("lib")
		}
		targetdir ( "lib" )
		
	configuration { "macosx" }
        buildoptions { "-stdlib=libc++" }
		linkoptions { 
			"-stdlib=libc++" ,
			"-Wl,-rpath," .. path.getabsolute("lib")
		}
		links {
	        "OpenGL.framework",
        }
        targetdir ( "lib" )
      
	if os.get() == "macosx" then
		premake.gcc.cc = "clang"
		premake.gcc.cxx = "clang++"
		-- buildoptions("-std=c++0x -ggdb -stdlib=libc++" )
	end

project "steersim"
	language "C++"
	kind "ConsoleApp"
	includedirs { 
		"../steerlib/include",
		"../steersim/include",
		"../steersimlib/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../steersim/include/*.h",
		"../steersim/src/*.cpp"
	}
	links { 		
		"steerlib",
		"steersimlib",
		"util",
		"glfw",
	}

	targetdir "bin"
	buildoptions("-std=c++0x -ggdb" )	

	-- linux library cflags and libs
	configuration { "linux", "gmake" }
		buildoptions { 
			"`pkg-config --cflags gl`",
			"`pkg-config --cflags glu`" 
		}
		linkoptions { 
			"-Wl,-rpath," .. path.getabsolute("lib") ,
			"`pkg-config --libs gl`",
			"`pkg-config --libs glu`" 
		}
		libdirs { "lib" }
		links {
			"X11",
			"tinyxml",
			"dl",
			"pthread"
		}

	-- windows library cflags and libs
	configuration { "windows" }
		-- libdirs { "lib" }
		links { 
			"opengl32",
			"glu32",
		}

	-- mac includes and libs
	configuration { "macosx" }
		kind "ConsoleApp" -- xcode4 failes to run the project if using WindowedApp
		includedirs { "/Library/Frameworks/SDL.framework/Headers" }
		buildoptions { "-Wunused-value -Wshadow -Wreorder -Wsign-compare -Wall" }
		linkoptions { 
			"-Wl,-rpath," .. path.getabsolute("lib") ,
		}
		links { 
			"OpenGL.framework", 
			"Cocoa.framework",
			"tinyxml",
			"dl",
			"pthread"
		}

project "steersimlib"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../steersimlib/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../steersimlib/include/*.h",
		"../steersimlib/include/**.h",
		"../steersimlib/src/*.cpp"
	}
	links { 		
		"steerlib",
		"util",
		"glfw",
	}

	
	buildoptions("-std=c++0x -ggdb" )	

	-- linux library cflags and libs
	configuration { "linux" }
		buildoptions { 
			"`pkg-config --cflags gl`",
			"`pkg-config --cflags glu`",
			"-fPIC"
		}
		linkoptions { 
			"-Wl,-rpath," .. path.getabsolute("lib") ,
			"`pkg-config --libs gl`",
			"`pkg-config --libs glu`",
			"-fPIC"
		}
		libdirs { "lib" }
		links {
			"Xrandr",
			"X11",
			"dl",
			"pthread",
			"tinyxml",
		}

	-- windows library cflags and libs
	configuration { "windows" }
		-- libdirs { "../RecastDemo/Contrib/SDL/lib/x86" }
		links { 
			"opengl32",
			"glu32",
			"glfw"
		}
		defines { "GLFW_BUILD_DLL" }

	configuration { "macosx" }
		links {
			"dl",
			"pthread",
			"tinyxml",
		}
		linkoptions { 
			"-install_name @rpath/libsteersimlib.dylib"
		}
		

--[====[
	-- mac includes and libs
	configuration { "macosx" }
		kind "ConsoleApp" -- xcode4 failes to run the project if using WindowedApp
		includedirs { "/Library/Frameworks/SDL.framework/Headers" }
		buildoptions { "-Wunused-value -Wshadow -Wreorder -Wsign-compare -Wall" }
		links { 
			"OpenGL.framework", 
			"Cocoa.framework",
		}
--]====]
		
project "tinyxml"
        language "C++"
        kind "SharedLib"
        includedirs {
                "../external/tinyxml"
        }
        files {
                "../external/tinyxml/*.h",
                "../external/tinyxml/*.cpp"
        }
        
		
        configuration { "macosx" }
			linkoptions { 
				"-install_name @rpath/libtinyxml.dylib"
			}
			buildoptions("-std=c++0x -ggdb" )	
			
		 configuration { "linux" }
			buildoptions("-std=c++0x -ggdb" )	
		
		
			
project "util"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../util/include"
	}
	files { 
		"../util/include/*.h",
		"../util/include/**.h",
		"../util/src/*.cpp"
	}
	-- targetdir "bin"
	configuration { "macosx" }
		linkoptions { 
			"-install_name @rpath/libutil.dylib"
		}
		buildoptions("-std=c++0x -ggdb" )	
		
	 configuration { "linux" }
		buildoptions("-std=c++0x -ggdb" )	

project "steerlib"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include" ,
		"../external" ,
		"../steerlib/include/util", 
		"../util/include" 
		
	}
	files { 
		"../steerlib/include/*.h",
		"../steerlib/include/**.h",
		"../steerlib/src/*.cpp" 
	}
	links { 
		-- "tinyxml",
		"util"
	}
	
	
	
	configuration { "windows" }
		files {
			"../external/tinyxml/*.h",
			"../external/tinyxml/*.cpp" 
		}
	
	configuration { "macosx" }
		links {
			"OpenGL.framework",
			"tinyxml"
		}
		linkoptions { 
			"-install_name @rpath/libsteerlib.dylib"
		}
		buildoptions("-std=c++0x -ggdb" )	
			
	 configuration { "linux" }
		buildoptions("-std=c++0x -ggdb" )	
		links { "tinyxml" }

project "glfw"
	kind "SharedLib"
    	language "C"
   	includedirs { 
		"../external/glfw/include",
		"../external/glfw/include/GL",
		"../external/glfw/lib",
		-- "../external/glfw/lib/x11"
	}
	files { 
	--	"../external/glfw/lib/x11/*.h",
	--	"../external/glfw/lib/x11/*.c",
		"../external/glfw/lib/*.h",
		"../external/glfw/lib/*.c"
	}
	defines { "GLFW_BUILD_DLL" }
	

    configuration {"linux"}
        files { 
		"../external/glfw/lib/x11/*.c",
		 "../external/glfw/x11/*.h" 
		}
        includedirs { "../external/glfw/lib/x11" }
        defines { "_GLFW_USE_LINUX_JOYSTICKS", "_GLFW_HAS_XRANDR", "_GLFW_HAS_PTHREAD" ,"_GLFW_HAS_SCHED_YIELD", "_GLFW_HAS_GLXGETPROCADDRESS" }
        links { 
			"pthread",
		}

        
        buildoptions { 
			"-pthread",
			"`pkg-config --cflags gl`",
			"`pkg-config --cflags glu`",  
			"-fPIC",
		}
       
    configuration {"windows"}
        files { 
		"../external/glfw/lib/win32/*.c",
		"../external/glfw/win32/*.h" 
		}
		links {
			"opengl32"
		}
        includedirs { "../external/glfw/lib/win32" }
        defines { "_GLFW_USE_LINUX_JOYSTICKS", "_GLFW_HAS_XRANDR", "_GLFW_HAS_PTHREAD" ,"_GLFW_HAS_SCHED_YIELD", "_GLFW_HAS_GLXGETPROCADDRESS" }
       
    configuration {"macosx"}
        files { "../external/glfw/lib/cocoa/*.c",
		 "../external/glfw/lib/cocoa/*.h",
		 "../external/glfw/lib/cocoa/*.m" 
	}
        includedirs { "../external/glfw/lib/cocoa" }
        defines { }
        links { 
			"pthread",
		}

        
--	removebuildoptions "-std=c++0x"
        linkoptions { 
		"-framework OpenGL", 
		"-framework Cocoa", 
		"-framework IOKit", 
                "-install_name @rpath/libglfw.dylib"
	}
	buildoptions {
                        "-fPIC",
                }

project "simpleAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../simpleAI/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../simpleAI/include/*.h",
		"../simpleAI/src/*.cpp"
	}
	links { 
		"util",
		"steerlib"
	}
	
		
	
	configuration { "macosx" }
		linkoptions { 
			"-install_name @rpath/libutil.dylib"
		}
		buildoptions("-std=c++0x -ggdb" )	
		
	 configuration { "linux" }
		buildoptions("-std=c++0x -ggdb" )	
	
project "sfAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../socialForcesAI/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../socialForcesAI/include/*.h",
		"../socialForcesAI/src/*.cpp"
	}
	links { 
		"steerlib",
		"util"
	}
		
	buildoptions("-std=c++0x -ggdb" )	
	
project "rvo2dAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../rvo2AI/include",
		"../external",
		"../util/include",
		"../kdtree/include", 
		-- "../meshdatabase/include",
		-- "../acclmesh/include"
	}
	files { 
		"../rvo2AI/include/*.h",
		"../rvo2AI/src/*.cpp"
	}
	links { 
		"steerlib",
		"util",
		-- "meshdatabase"
	}
		
	buildoptions("-std=c++0x -ggdb" )	

project "pprAI"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../pprAI/include",
		"../external",
		"../util/include",
	}
	files { 
		"../pprAI/include/*.h",
		"../pprAI/src/*.cpp"
	}
	links { 
		"steerlib",
		"util"
	}
		
	buildoptions("-std=c++0x -ggdb" )	
	
	
project "kdtree"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../kdtree/include",
		"../external",
		"../util/include",
	}
	files { 
		"../kdtree/include/*.h",
		"../kdtree/src/*.cpp"
	}
	links { 
		"steerlib",
		"util",
	}
	
	buildoptions("-std=c++0x -ggdb" )	
	configuration { "macosx" }
                linkoptions {
                        "-install_name @rpath/libkdtree.dylib"
                }

project "Recast"
	language "C++"
	kind "StaticLib"
	includedirs { 
		"../navmeshBuilder/include",
		"../external/recastnavigation/Recast/Include",
		"../util/include",
	}
	files { 
		"../external/recastnavigation/Recast/Include/*.h",
		"../external/recastnavigation/Recast/Source/*.cpp",
	}
	links { 
		"steerlib",
		"util",
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	
	configuration { "macosx" }
        linkoptions {
                "-install_name @rpath/libRecast.dylib"
        }
		buildoptions { 
			"-fPIC"
		}

project "DebugUtils"
	language "C++"
	kind "StaticLib"
	includedirs { 
		"../navmeshBuilder/include",
		"../external/recastnavigation/DebugUtils/Include",
		"../external/recastnavigation/Detour/Include",
		"../external/recastnavigation/Recast/Include",
		"../external/recastnavigation/DetourTileCache/Include",
		"../util/include",
	}
	files { 
		"../external/recastnavigation/DebugUtils/Include/*.h",
		"../external/recastnavigation/DebugUtils/Source/*.cpp",
	}
	links { 
		"Recast",
		"Detour"
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	configuration { "macosx" }
        linkoptions {
                "-install_name @rpath/libDebugUtils.dylib"
        }

project "Detour"
	language "C++"
	kind "StaticLib"
	includedirs { 
		"../navmeshBuilder/include",
		"../external/recastnavigation/DebugUtils/Include",
		"../external/recastnavigation/Detour/Include",
		"../external/recastnavigation/Recast/Include",
		"../external/recastnavigation/DetourTileCache/Include",
		"../util/include",
	}
	files { 
		"../external/recastnavigation/Detour/Include/*.h",
		"../external/recastnavigation/Detour/Source/*.cpp",
	}
	links { 
		"Recast",
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	configuration { "macosx" }
        linkoptions {
                "-install_name @rpath/libDetour.dylib"
        }

project "DetourCrowd"
	language "C++"
	kind "StaticLib"
	includedirs { 
		"../navmeshBuilder/include",
		"../external/recastnavigation/DebugUtils/Include",
		"../external/recastnavigation/Detour/Include",
		"../external/recastnavigation/Recast/Include",
		"../external/recastnavigation/DetourTileCache/Include",
		"../external/recastnavigation/DetourCrowd/Include",
		"../util/include",
	}
	files { 
		"../external/recastnavigation/DetourCrowd/Include/*.h",
		"../external/recastnavigation/DetourCrowd/Source/*.cpp",
	}
	links { 
		"Recast",
		"Detour"
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	configuration { "macosx" }
        linkoptions {
                "-install_name @rpath/libDetourCrowd.dylib"
        }	

project "navmesh"
	language "C++"
	kind "SharedLib"
	includedirs { 
		"../steerlib/include",
		"../navmeshBuilder/include",
		"../external/recastnavigation/Recast/Include",
		"../external/recastnavigation/DebugUtils/Include",
		"../external/recastnavigation/Detour/Include",
		"../external/recastnavigation/DetourTileCache/Include",
		"../external/recastnavigation/DetourCrowd/Include",
		"../steersimlib/include",
		"../external",
		"../util/include",
	}
	files { 
		"../navmeshBuilder/include/*.h",
		"../navmeshBuilder/src/*.cpp"
	}
	links { 
		"steerlib",
		"steersimlib",
		"util",
		"Recast",
		"DebugUtils",
		"Detour",
		"DetourCrowd",
		
	}
	
	buildoptions("-std=c++0x -ggdb -fPIC" )	
	configuration { "macosx" }
                linkoptions {
                        "-install_name @rpath/libnavmesh.dylib"
                }

	
project "steerbench"
	language "C++"
	kind "ConsoleApp"
	includedirs { 
		"../steerlib/include",
		"../steerbench/include",
		"../external",
		"../util/include" 
	}
	files { 
		"../steerbench/include/*.h",
		"../steerbench/src/*.cpp"
	}
	links { 		
		"steerlib",
		"util",
		"glfw"
	}


	targetdir "bin"
	buildoptions("-std=c++0x -ggdb" )	

	-- linux library cflags and libs
	configuration { "linux", "gmake" }
		-- kind "ConsoleApp"
		buildoptions { 
			"`pkg-config --cflags gl`",
			"`pkg-config --cflags glu`" 
		}
		linkoptions { 
			-- "-Wl,-rpath,./lib",
			"-Wl,-rpath," .. path.getabsolute("lib") ,
			"`pkg-config --libs gl`",
			"`pkg-config --libs glu`" 
		}
		links { 		
			"GLU",
			"GL",
			"dl",
			"tinyxml"
		}
		libdirs { "lib" }

	-- windows library cflags and libs
	configuration { "windows" }
		libdirs { "../RecastDemo/Contrib/SDL/lib/x86" }
		links { 
			"opengl32",
			"glu32",
		}

	-- mac includes and libs
	configuration { "macosx" }
		kind "ConsoleApp" -- xcode4 failes to run the project if using WindowedApp
		buildoptions { "-Wunused-value -Wshadow -Wreorder -Wsign-compare -Wall" }
		links { 
			"OpenGL.framework", 
			"Cocoa.framework",
			"dl",
			"tinyxml"
		}


if file_exists("premake4-dev.lua")
	then
	dofile("premake4-dev.lua")
end
