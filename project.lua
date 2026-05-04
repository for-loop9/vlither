vk_path = os.getenv("VULKAN_SDK")

workspace "vlither"
  location "build/makefiles"
  configurations { "debug", "release" }
  platforms { "linux", "windows" }

  project "glfw"
    kind "staticlib"
    architecture "x86_64"
    language "c"
    objdir "build/bin-int/%{cfg.platform}_%{cfg.architecture}_%{cfg.buildcfg}/%{prj.name}"
    targetdir "build/bin/%{cfg.platform}_%{cfg.architecture}_%{cfg.buildcfg}"

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

    filter { "platforms:linux" }
      files {
        -- linux:
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

      defines {
        "_GLFW_X11"
      }

    filter { "platforms:windows"}
      files {
        -- windows:
        "glfw/src/wgl_context.c",
        "glfw/src/win32_init.c",
        "glfw/src/win32_joystick.c",
        "glfw/src/win32_module.c",
        "glfw/src/win32_thread.c",
        "glfw/src/win32_time.c",
        "glfw/src/win32_window.c",
        "glfw/src/win32_monitor.c",
      }

      defines {
        "_GLFW_WIN32"
      }

    filter { "configurations:release" }
			linkoptions "-static"
			optimize "speed"
			symbols "off"

    filter { "configurations:debug" }
      symbols "on"
      optimize "off"

  project "thermite"
    kind "staticlib"
    architecture "x86_64"
    language "c"
    location "build/makefiles"
    objdir "build/bin-int/%{cfg.platform}_%{cfg.architecture}_%{cfg.buildcfg}/%{prj.name}"
    targetdir "build/bin/%{cfg.platform}_%{cfg.architecture}_%{cfg.buildcfg}"

    files { "thermite/src/**.c", "thermite/src/**.cpp" }
    
    includedirs { "glfw/include", "thermite/include", vk_path .. "/include" }

    defines { "GLFW_INCLUDE_NONE", "GLFW_INCLUDE_VULKAN", "CGLM_FORCE_DEPTH_ZERO_TO_ONE" }

    filter { "configurations:release" }
			linkoptions "-static"
			optimize "speed"
			symbols "off"

    filter { "configurations:debug" }
      defines { "TDEBUG" }
      symbols "on"
      optimize "off"
  
  project "app"
    kind "consoleapp"
    architecture "x86_64"
    language "c"
    location "build/makefiles"
    objdir "build/bin-int/%{cfg.platform}_%{cfg.architecture}_%{cfg.buildcfg}/%{prj.name}"
    targetdir "build/bin/%{cfg.platform}_%{cfg.architecture}_%{cfg.buildcfg}"
    includedirs { "glfw/include", "thermite/include", "thermite/src", vk_path .. "/include" }
    defines { "GLFW_INCLUDE_NONE", "GLFW_INCLUDE_VULKAN", "CGLM_FORCE_DEPTH_ZERO_TO_ONE", "CIMGUI_NO_EXPORT" }

    libdirs { vk_path .. "/lib" }
    files { "app/src/**.c", "app/src/**.cpp" }
    defines {
      "APP_VERSION=\"2.5\"",
      "SETTINGS_VERSION=\"1.2\""
    }

    filter { "platforms:linux" }
      links { "thermite", "glfw", "vulkan", "stdc++", "m" }

    filter { "platforms:windows" }
      links { "thermite", "glfw", "vulkan-1", "stdc++", "m", "gdi32", "ws2_32" }

    filter { "configurations:release" }
			optimize "speed"
			symbols "off"

    filter { "platforms:windows" }
      linkoptions "-static"

    filter { "configurations:debug" }
      symbols "on"
      optimize "off"
