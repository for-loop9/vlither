workspace "vlither"
	location "build/makefiles"
	configurations { "debug", "release" }
	architecture "x86_64"

	project "glfw"
		location "build/makefiles"
		kind "staticlib"
		toolset "gcc"
		language "c"
		objdir "build/bin-int/"
		targetdir "build/bin/"
		files {
			"glfw/src/context.c",
			"glfw/src/init.c",
			"glfw/src/input.c",
			"glfw/src/monitor.c",
			"glfw/src/platform.c",
			"glfw/src/vulkan.c",
			"glfw/src/window.c",
			"glfw/src/egl_context.c",
			"glfw/src/osmesa_context.c",
			"glfw/src/null_init.c",
			"glfw/src/null_monitor.c",
			"glfw/src/null_window.c",
			"glfw/src/null_joystick.c",
		}

		filter "system:windows"
			defines { 
				"_GLFW_WIN32"
			}
			files {
				"glfw/src/win32_module.c",
				"glfw/src/win32_time.c",
				"glfw/src/win32_thread.c",

				"glfw/src/win32_init.c",
				"glfw/src/win32_joystick.c",
				"glfw/src/win32_monitor.c",
				"glfw/src/win32_window.c",
				"glfw/src/wgl_context.c"
			}
		
		filter "system:linux"
			defines { 
				"_GLFW_X11"
			}
			files {
				"glfw/src/posix_module.c",
				"glfw/src/posix_time.c",
				"glfw/src/posix_thread.c",

				"glfw/src/x11_init.c",
				"glfw/src/linux_joystick.c",
				"glfw/src/x11_monitor.c",
				"glfw/src/x11_window.c",
				"glfw/src/xkb_unicode.c",
				"glfw/src/glx_context.c",
				"glfw/src/posix_poll.c"
			}
		
		filter "configurations:debug"
			optimize "off"
			symbols "on"

		filter "configurations:release"
			linkoptions "-static"
			optimize "speed"
			symbols "off"
	
	project "ignite"
		location "build/makefiles"
		kind "staticlib"
		toolset "gcc"
		language "c"
		objdir "build/bin-int/"
		targetdir "build/bin/"

		defines {
			"GLFW_INCLUDE_NONE",
			"GLFW_INCLUDE_VULKAN"
		}

		files { "ignite/src/**.c", "ignite/src/**.cpp" }
		includedirs {"glfw/include", os.host() == "windows" and os.getenv("VULKAN_SDK") and path.join(os.getenv("VULKAN_SDK"), "Include") or "" }

		filter { "configurations:release" }
			linkoptions "-static"
			optimize "speed"
			symbols "off"	
		
		filter { "configurations:debug" }
			optimize "off"
			symbols "on"

	project "app"
		location "build/makefiles"
		kind "consoleapp"
		toolset "gcc"
		language "c"
		objdir "build/bin-int"
		targetdir "build/bin"

		defines {
			"GLFW_INCLUDE_NONE",
			"GLFW_INCLUDE_VULKAN"
		}
		
		includedirs { "ignite/src", "glfw/include", os.host() == "windows" and os.getenv("VULKAN_SDK") and path.join(os.getenv("VULKAN_SDK"), "Include") or "" }
		files { "app/src/**.c", "app/src/**.cpp" }

		filter { "system:linux" }
			links { "ignite", "glfw", "vulkan", "stdc++", "m", "pthread" }
		filter { "system:windows" }
			local vulkan_sdk = os.getenv("VULKAN_SDK")
			if vulkan_sdk then
				libdirs { vulkan_sdk .. "/Lib" }
			end
			links { "ignite", "glfw", "vulkan-1", "stdc++", "m", "gdi32", "wsock32", "ws2_32" }
		
		filter { "configurations:release", "system:windows" }
			linkoptions "-static"
			optimize "speed"
			symbols "off"
		
		filter { "configurations:release", "system:linux" }
			optimize "speed"
			symbols "off"	
		
		filter { "configurations:debug" }
			optimize "off"
			symbols "on"