#include <MeshManager.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <stb_image.h>

#include <Transform.h>
#include <Model.h>
#include <Mesh.h>
#include <MeshInstance.h>
#include <VulkanTexture.h>

using namespace VulkanRenderer;

MeshManager::MeshManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout)
	: device(device), descriptorSetLayout(descriptorSetLayout)
{

}

MeshManager::~MeshManager()
{
	meshes.clear();
}

std::shared_ptr<Mesh> MeshManager::GetMesh(const std::string& name)
{
	auto it = meshes.find(name);
	if (it != meshes.end())
		return it->second;
	return nullptr;
}

std::shared_ptr<Model> MeshManager::LoadModel(const std::string& name, const std::filesystem::path& path)
{
	auto it = models.find(name);
	if (it != models.end())
	{
		return it->second;
	}
	
	fastgltf::Parser parser;

	auto data = fastgltf::GltfDataBuffer::FromPath(path);
	if (data.error() != fastgltf::Error::None)
	{
		std::cout << "fastgltf get buffer error: " << fastgltf::getErrorMessage(data.error()) << std::endl;
		return false;
	}
	
	auto asset = parser.loadGltfBinary(data.get(), path.parent_path(), fastgltf::Options::None);
	if (asset.error() != fastgltf::Error::None)
	{
		std::cout << "fastgltf get data error: " << fastgltf::getErrorMessage(asset.error()) << std::endl;
		return false;
	}
	auto& gltfAsset = asset.get();

	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->name = name;
	model->gltfAsset = std::move(gltfAsset);
	
	//LoadTextures(gltfAsset, name);
	
	for (size_t meshIndex = 0; meshIndex < gltfAsset.meshes.size(); ++meshIndex)
	{
		const fastgltf::Mesh& gltfMesh = gltfAsset.meshes[meshIndex];

		for (size_t primitiveIndex = 0; primitiveIndex < gltfMesh.primitives.size(); ++primitiveIndex)
		{
			const fastgltf::Primitive& primitive = gltfMesh.primitives[primitiveIndex];
			
			auto positionIt = primitive.findAttribute("POSITION");
			auto normalIt = primitive.findAttribute("NORMAL");
			auto tangentIt = primitive.findAttribute("TANGENT");
			
			MeshInfo info{};
			
			if (positionIt != primitive.attributes.end())
			{
				const auto& positionAccessor = gltfAsset.accessors[positionIt->accessorIndex];
				info.vertices.resize(positionAccessor.count);

				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltfAsset, positionAccessor, [&](fastgltf::math::fvec3 position, std::size_t verticeIndex)
					{
						info.vertices[verticeIndex].position = glm::vec3(position.x(), position.y(), position.z());
					});
			}

			if (primitive.indicesAccessor.has_value())
			{
				const auto& indexAccessor = gltfAsset.accessors[primitive.indicesAccessor.value()];
				fastgltf::iterateAccessorWithIndex<std::uint16_t>(gltfAsset, indexAccessor, [&](std::uint16_t index, std::size_t verticeIndex)
					{
						info.indices.push_back(index);
					});
			}
		}
	}
	
	models[name] = model;

	return model;
}

std::shared_ptr<Mesh> MeshManager::LoadMesh(const std::string& name, const MeshInfo& info)
{
	auto it = meshes.find(name);
	if (it != meshes.end())
	{
		return it->second;
	}

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(device, descriptorSetLayout, name, info);
	meshes[name] = mesh;

	return mesh;
}

void MeshManager::LoadTextures(const fastgltf::Asset& asset)
{
	for (size_t imageIndex = 0; imageIndex < asset.images.size(); ++imageIndex)
	{
		const auto& image = asset.images[imageIndex];

		bool sRGB = false;
		TextureType textureType = TextureType::Unknown;
		size_t materialIndex = SIZE_MAX;

		for (size_t matIndex = 0; matIndex < asset.materials.size(); ++matIndex)
		{
			const auto& material = asset.materials[matIndex];
			if (material.pbrData.baseColorTexture.has_value())
			{
				size_t textureIndex = material.pbrData.baseColorTexture.value().textureIndex;
				if (textureIndex < asset.textures.size() && asset.textures[textureIndex].imageIndex == static_cast<int>(imageIndex))
				{
					sRGB = true;
					textureType = TextureType::BaseColor;
					materialIndex = matIndex;
					break;
				}
			}
			else if (material.pbrData.metallicRoughnessTexture.has_value())
			{
				size_t textureIndex = material.pbrData.metallicRoughnessTexture.value().textureIndex;
				if (textureIndex < asset.textures.size() && asset.textures[textureIndex].imageIndex == static_cast<int>(imageIndex))
				{
					textureType = TextureType::MetallicRoughness;
					materialIndex = matIndex;
					break;
				}
			}
			else if (material.occlusionTexture.has_value())
			{
				size_t textureIndex = material.occlusionTexture.value().textureIndex;
				if (textureIndex < asset.textures.size() && asset.textures[textureIndex].imageIndex == static_cast<int>(imageIndex))
				{
					textureType = TextureType::Occlusion;
					materialIndex = matIndex;
					break;
				}
			}
			else if (material.normalTexture.has_value())
			{
				size_t textureIndex = material.normalTexture.value().textureIndex;
				if (textureIndex < asset.textures.size() && asset.textures[textureIndex].imageIndex == static_cast<int>(imageIndex))
				{
					textureType = TextureType::Normal;
					materialIndex = matIndex;
					break;
				}
			}
			if (textureType == TextureType::Unknown || materialIndex == SIZE_MAX)
			{
				std::cout << "Image" << imageIndex << " not associated with a supported texture type." << std::endl;
				continue;
			}

			VulkanTexture* texture = LoadTexture(asset, image, sRGB);
			if (texture)
			{

			}
		}
	}
}

VulkanTexture* MeshManager::LoadTexture(const fastgltf::Asset& asset, const fastgltf::Image& image, bool sRGB)
{
	VulkanTexture* texture = nullptr;
	
	std::visit(fastgltf::visitor
		{
			[&](const fastgltf::sources::URI& filePath)
			{
				std::cout << "External GLTF textures are not yet supported. Please use GLB models." << std::endl;
			},
			[&](const fastgltf::sources::Array& vector)
			{
				std::cout << "GLTF array textures are not yet supported." << std::endl;
			},
			[&](const fastgltf::sources::BufferView& view)
			{
				if (view.bufferViewIndex >= asset.bufferViews.size())
				{
					std::cerr << "Invalid buffer view index in image." << std::endl;
					return;
				}
				
				const auto& bufferView = asset.bufferViews[view.bufferViewIndex];
				const auto& buffer = asset.buffers[bufferView.bufferIndex];

				stbi_set_flip_vertically_on_load(false);

				std::visit(fastgltf::visitor
					{
						[&](const fastgltf::sources::Array& vector)
						{
							int width, height, nrChannels;
							stbi_uc* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data() + bufferView.byteOffset),
																		static_cast<int>(bufferView.byteLength), &width, &height, &nrChannels, 4);

							if (!pixels)
							{
								std::cerr << "Failed to decode image from GLTF buffer" << std::endl;
								return;
							}
							
							texture = new VulkanTexture(device, pixels, width, height, sRGB);

							stbi_image_free(pixels);
						},
						[](const auto& arg)
						{
							std::cerr << "Unsupported buffer source type in image buffer view." << std::endl;
						}
					}, buffer.data);
			},
			[](const auto& arg)
			{
				std::cerr << "Unsupported image source type." << std::endl;
			},
		}, image.data);

	return texture;
}