project "ImGui"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines { "IMGUI_IMPL_VULKAN_USE_VOLK" }

	files { "**.h", "**.cpp" }

	includedirs
	{
		"%{wks.location}/Engine/Vendor/vulkan-headers/include",
		"%{wks.location}/Engine/Vendor/volk",
		"%{wks.location}/Engine/Vendor/glfw/include"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
