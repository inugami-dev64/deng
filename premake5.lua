--    ____         ________     __      _        ______
--   |  _ \_      |  ______|   |  \    | |     _/  ____\
--   | | \_ \_    |  |         |   \   | |    /  _/   
--   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
--   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
--   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
--   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
--   |____/       |________|   |_|    \__|       \____/
--               
-- __________________________________________________________
-- __________________________________________________________
--                       Project DENG 
--
--
-- Copyright (C) 2020 - 2021
-- This Software is licensed under Apache License as described 
-- in the LICENSE.md file, which you should have recieved with 
-- this distribution.
-- 
-- You may reproduce and distribute copies of the
-- Work or Derivative Works thereof in any medium, with or without
-- modifications, and in Source or Object form, provided that You
-- meet the following conditions:
--
-- (a) You must give any other recipients of the Work or
--     Derivative Works a copy of this License; and
--
-- (b) You must cause any modified files to carry prominent notices
--     stating that You changed the files; and
--
-- (c) You must retain, in the Source form of any Derivative Works
--     that You distribute, all copyright, patent, trademark, and
--     attribution notices from the Source form of the Work,
--     excluding those notices that do not pertain to any part of
--     the Derivative Works; and
--
-- (d) If the Work includes a "NOTICE" text file as part of its
--     distribution, then any Derivative Works that You distribute must
--     include a readable copy of the attribution notices contained
--     within such NOTICE file, excluding those notices that do not
--     pertain to any part of the Derivative Works, in at least one
--     of the following places: within a NOTICE text file distributed
--     as part of the Derivative Works; within the Source form or
--     documentation, if provided along with the Derivative Works; or,
--     within a display generated by the Derivative Works, if and
--     wherever such third-party notices normally appear. The contents
--     of the NOTICE file are for informational purposes only and
--     do not modify the License. You may add Your own attribution
--     notices within Derivative Works that You distribute, alongside
--     or as an addendum to the NOTICE text from the Work, provided
--     that such additional attribution notices cannot be construed
--     as modifying the License.
--
-- You may add Your own copyright statement to Your modifications and
-- may provide additional or different license terms and conditions
-- for use, reproduction, or distribution of Your modifications, or
-- for any such Derivative Works as a whole, provided Your use,
-- reproduction, and distribution of the Work otherwise complies with
-- the conditions stated in this License.

workspace "deng"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Linux" }
    includedirs { 
        "./headers",
        "./modules/imgui",
        "./modules/freetype/include/",
        "./modules/freetype/include/freetype"
    }

	architecture "x86_64"
    defines { "ImDrawIdx=unsigned int" }
    targetdir "build"
	pic "On"

    -- Add freetype include directories when using linux
    filter "platforms:Linux"
        includedirs { "/usr/include/freetype2", "/usr/include/freetype2/freetype" }

	-- Ignore safety warnings that MSVC gives
	filter "platforms:Win32"
		defines { "_CRT_SECURE_NO_WARNINGS" }

    -- Enable debug symbols if specified
    filter "configurations:Debug"
        symbols "On"
        optimize "Debug"

    filter "configurations:Release"
        symbols "Off"
        optimize "Speed"
    filter {}

--!!! Add new options to use !!!--
-- Create an option to compile all dependencies from source
newoption {
    trigger = "use-all-modules",
    description = "Compile all dependencies from source (all git submodules must be initialised)"
}


-- Create an option to specify vulkan sdk library location (Windows only)
newoption {
	trigger = "vk-sdk-path",
	description = "Specify Vulkan SDK path for Windows builds (Windows only)"
}


-- Create an option to build DENG sandboxapp using specific sandbox configuration
newoption {
    trigger = "sandbox-mode",
    value = "MODE",
    description = "Select sandbox application mode",
    allowed = {
        { "deng", "Build an application for testing the renderer by loading assets" },
        { "imgui", "Build an application for testing ImGui with DENG renderer" },
        { "none", "Do not build any sandbox applications (default)" },
        { "all", "Build all sandbox applications" }
    }
}


-- Check if given operating system is even supported
function oscheck() 
    if(not os.istarget("linux") and not os.istarget("windows")) then
        local host = os.target()
        host = host:gsub("%a", string.upper, 1)
        print(host .. " is not supported by DENG! :(")
        os.exit();
    end
end


-- Define the clean action 
function cleanoptcheck()
    if _ACTION == "clean" then
        print("Cleaning project files")
        if(os.host() == "linux") then
            os.execute("rm -rf *.make obj Makefile")
            os.exit()
        elseif(os.host() == "windows") then
            os.execute("del /s /q deng.sln obj *.vcxproj*")
            os.exit()
        end
    end
end


-- Check for the SDK path to be used in correct situation
function vksdkoptcheck()
    -- Check if Vulkan SDK path should be and if it is specified
    if os.istarget("windows") and _OPTIONS["vk-sdk-path"] then
        libdirs { _OPTIONS["vk-sdk-path"] .. "\\Lib" }
        includedirs{ _OPTIONS["vk-sdk-path"] .. "\\Include" }
    elseif not os.istarget("windows") and _OPTIONS["vk-sdk-path"] then
        print("Vulkan SDK path should only be specified for Windows builds")
        os.exit()
    elseif os.istarget("windows") and not _OPTIONS["vk-sdk-path"] then
        print("Please specify Vulkan SDK path with vk-sdk-path flag")
        os.exit()
    end
end


-- Check if given options are valid
function optcheck()
    cleanoptcheck()
    vksdkoptcheck()
end


-- Check which modules to build
function modcheck()
    -- These modules are going to be built no matter what
    local imgui = require("premake/imgui")
    imgui.build()

    -- Check if all submodule build configs should be created
    if _OPTIONS["use-all-modules"] then
        local ft = require("premake/ft")
        ft.build()
    elseif os.istarget("windows") then
        print("Please use use-all-modules option on Windows builds!")
        os.exit()
    end
end


-- Setup build destinations
function buildcfg()
    -- Check if sandbox application should be made and copy files if needed
    if not _OPTIONS["sandbox-mode"] then
        _OPTIONS["sandbox-mode"] = "none"
    elseif _OPTIONS["sandbox-mode"] ~= "none" then
        local sandbox_data = require("premake/sandbox_data")
        sandbox_data.dataset();
    end


    -- Check if static libdeng build should be made
    local libdeng = require("premake/libdeng")
    if _OPTIONS["build-static"] then
        libdeng.build(true)
    else 
        libdeng.build(false)
    end

    -- DENG asset manager build configuration
    local dam = require("premake/dam")
    dam.build()

    -- Sandbox application build configuration
    if _OPTIONS["sandbox-mode"] == "deng" or _OPTIONS["sandbox-mode"] == "all" then
        local dengbox = require("premake/dengbox")
        dengbox.build()
    end

    if _OPTIONS["sandbox-mode"] == "imgui" or _OPTIONS["sandbox-mode"] == "all" then
        local imgui_sandbox = require("premake/imgui_sandbox")
        imgui_sandbox.build()
    end
end


-- Script entry point, check if not help
if not _OPTIONS["help"] then
    oscheck()
    optcheck()
    modcheck()
    buildcfg()
end
