project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines { "VK_NO_PROTOTYPES" }
	defines { "GLFW_INCLUDE_VULKAN" }

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Vendor/vulkan-headers/include",
		"Vendor/volk",
		"Vendor/glfw/include",
		"Vendor/glm",
		"Vendor/stb",
		"Vendor/fastgltf/include",
		"Vendor/imgui"
	}

	links { "glfw", "fastgltf", "imgui" }
