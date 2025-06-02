workspace "VulkanRenderer"
	architecture "x86_64"
	startproject "App"

	configurations { "Debug", "Release" }

	filter { "configurations:Debug"}
		defines { "DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"

	filter { }

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Engine/Vendor/glfw"
	include "Engine/Vendor/fastgltf"
	include "Engine/vendor/imgui"
group ""

group "VulkanRenderer"
	include "Engine"
	include "App"
group ""
